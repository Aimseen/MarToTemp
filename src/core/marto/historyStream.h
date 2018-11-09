/* -*-c++-*- C++ mode for emacs */
/* Events History */
#ifndef MARTO_HISTORY_STREAM_H
#define MARTO_HISTORY_STREAM_H

#ifdef __cplusplus

#include <type_traits>

namespace marto {

/** \brief an abstract class for object that can be writen into a HistoryOStream
 */
class IsWritable {
  public:
    virtual void writeToHOS(HistoryOStream &ostream) const = 0;
};

/** \brief an abstract class for object that can be read from a HistoryIStream
 */
class IsReadable {
  public:
    virtual void readFromHIS(HistoryIStream &istream) = 0;
};

/** \brief an abstract class for object that can be written/read from a HistoryStream
 */
class IsStreamable : public IsReadable, public IsWritable {
};

/** \brief Base class to read of write content in history */
class HistoryStreamBase {
    /** \brief forbid copy of this kind of objects */
    HistoryStreamBase(const HistoryStreamBase &) = delete;
    /** \brief forbid assignment of this kind of objects */
    HistoryStreamBase &operator=(const HistoryStreamBase &) = delete;

  public:
    /** \brief type used to store the size of an object in the history
     */
    typedef uint16_t objectsize_t;

  protected:
    char *buf;
    size_t bufsize;
    size_t objectsize;
    unsigned int eofbit : 1;

    /** \brief Create a object that consumes a bounded buffer
     *
     * It will be inherited by History[IO]Stream
     */
    HistoryStreamBase(char *buffer, size_t lim)
        : buf(buffer), bufsize(lim), objectsize(0), eofbit(0){};
    size_t objectSize() { return objectsize; };

  public:
    /** \brief conversion in bool for while loops */
    explicit operator bool() const { return !eof(); };
    bool eof() const { return eofbit; };

  public:
    template <typename T> class CompactInt;
};

/** \brief Helper class to store integral types compactly
 *
 * A constructor from and an explicit cast operator to the targeted
 * type are provided.
 *
 * When saved in history, bits of the value are saved 7 by 7 in bytes.
 * The MSB of the byte indicate if this is the last byte or not.
 * If the type is signed, the 6th bit of the first byte is a sign bit.
 * In case the value is negative, then all bits to be saved are reversed.
 *
 * Examples:
 * * 0 => 00000000
 * * 1 => 00000001
 * * (unsigned)63 => 00111111
 * * (int)63 => 00111111
 * * (unsigned)64 => 01000000
 * * (int)64 => 10000000 00000001
 * *  => one more byte is required as there is a sign bit in the first byte
 * * (int)-1 => 010000000
 * * (int)-2 => 010000001
 */
template <typename T> class HistoryStreamBase::CompactInt : public IsStreamable { // no-forward-decl
  private:
    T val;

  public:
    CompactInt() : val(0){};
    CompactInt(T value) : val(value) {
        static_assert(std::is_arithmetic<T>::value, "T must be numeric");
    };
    explicit operator T() { return val; }
    void readFromHIS(HistoryIStream &istream);
    void writeToHOS(HistoryOStream &ostream) const;
};

/** \brief Class to read one object from history */
class HistoryIStream : public HistoryStreamBase {
  private:
    /** \brief Create a object that will allow read anything in a buffer
     *
     * This will be created by HistoryIterator::read*() for example
     */
    HistoryIStream(char *buffer, size_t lim) : HistoryStreamBase(buffer, lim) {
        objectsize_t obsize;
        read(obsize);
        objectsize = obsize;
        if (marto_unlikely(objectsize == 0)) {
            /* Object not yet fully written (finalized not called)
             * or read fails
             */
            throw HistoryIncompleteObject("Object not yet all written");
        }
        if (marto_unlikely(objectsize > lim)) {
            /* the object to read is longer than the buffer in the current
             * chunk!
             *
             * This should never occurs
             */
            throw HistoryOutOfBound("Object too long for the current buffer!");
        }
        /* limiting the following reads to the object data */
        bufsize = objectsize - (buf - buffer);
    };
    friend history_access_t HistoryIterator::loadNextEvent(Event *ev);

    template <typename T, size_t sizeof_T = sizeof(T), size_t alignof_T = alignof(T)>
    typename std::enable_if<alignof_T != 1>::type
    read(T &var);
    template <typename T, size_t sizeof_T = sizeof(T), size_t alignof_T = alignof(T)>
    typename std::enable_if<alignof_T == 1>::type
    read(T &var);

    /* convolution as partial template specialization is not allowed for
     * function Idea taken from
     * https://stackoverflow.com/questions/8061456/c-function-template-partial-specialization/21218271
     */
    template <bool is_integral, typename T>
    inline typename std::enable_if<is_integral, void>::type readvar(T &var) {
        CompactInt<T> cval;
        *this >> cval;
        var = (T)cval;
    }
    template <bool is_integral, typename T>
    inline typename std::enable_if<!is_integral, void>::type readvar(T &var) {
        read(var);
    }

  public:
    /** \brief classical >> input stream operator
     *
     * Integral types are compacted
     */
    template <typename T> inline HistoryIStream &operator>>(T &var);

    /** \brief >> input stream operator for CompactInt objects
     */
    template <typename T> inline HistoryIStream &operator>>(CompactInt<T> &var);

    /** \brief >> input stream operator for IsReadable objects
     */
    //template<>
    //HistoryIStream &operator>>(IsReadable &var) {
    //    var.read(*this);
    //    return *this;
    //}
};

/** \brief Class to write the content of one object into a chunk
 */
class HistoryOStream : public HistoryStreamBase {
  private:
    objectsize_t *objectSizePtr;
    /** \brief Create a object that will allow write anything in a buffer
     *
     * This will be created by HistoryIterator::store*()
     */
    HistoryOStream(char *buffer, size_t lim) : HistoryStreamBase(buffer, lim) {
        objectSizePtr = write((objectsize_t)0);
    };
    friend history_access_t HistoryIterator::storeNextEvent(Event *ev);
    template <typename T>
#if defined(__clang__)
    friend class HistoryStreamBase::CompactInt;
#else
    friend void
    HistoryStreamBase::CompactInt<T>::writeToHOS(HistoryOStream &ostream) const;
#endif

    template <typename T> T *write(const T &value);

    /* convolution as partial template specialization is not allowed for
     * function Idea taken from
     * https://stackoverflow.com/questions/8061456/c-function-template-partial-specialization/21218271
     */
    template <bool is_integral, typename T>
    inline typename std::enable_if<is_integral, void>::type
    writevar(const T &var) {
        *this << (CompactInt<T>)var;
    }
    template <bool is_integral, typename T>
    inline typename std::enable_if<!is_integral, void>::type
    writevar(const T &var) {
        write(var);
    }

  public:
    /** \brief classical << output stream operator
     */
    template <typename T> HistoryOStream &operator<<(const T &var) {
        static_assert(std::is_arithmetic<T>::value, "T must be numeric");
        writevar<std::is_integral<T>::value>(var);
        return *this;
    }
    /** \brief called when a store is interrupted */
    void abort() { eofbit = 1; }
    /** \brief finalize the write of the object in the history
     */
    history_access_t finalize();

  public:
    template <typename T> HistoryOStream &operator<<(const CompactInt<T> &var) {
        static_assert(std::is_arithmetic<T>::value, "T must be numeric");
        var.writeToHOS(*this);
        return *this;
    }
};
} // namespace marto

#ifndef MARTO_H
// In case of direct inclusion (instead of using <marto.h>),
// we try to include the implementation, hoping to avoid include loops
#include <marto/historyStream-impl.h>
#endif

#endif
#endif
