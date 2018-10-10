/* -*-c++-*- C++ mode for emacs */
/* Events History */
#ifndef MARTO_HISTORY_STREAM_H
#define MARTO_HISTORY_STREAM_H

#ifdef __cplusplus

namespace marto {

/** \brief Base class to read of write events in history */
class EventsStreamBase {
    /** \brief forbid copy of this kind of objects */
    EventsStreamBase(const EventsStreamBase &) = delete;
    /** \brief forbid assignment of this kind of objects */
    EventsStreamBase &operator=(const EventsStreamBase &) = delete;

  public:
    /** \brief type used to store the size of an event in the history
     */
    typedef unsigned int eventsize_t;

  protected:
    char *buf;
    size_t bufsize;
    size_t eventsize;
    unsigned int eofbit : 1;

    /** \brief Create a object that consumes a bounded buffer
     *
     * It will be inherited by Events[IO]Stream
     */
    EventsStreamBase(char *buffer, size_t lim)
        : buf(buffer), bufsize(lim), eventsize(0), eofbit(0){};
    size_t eventSize() { return eventsize; };

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
template <typename T> class EventsStreamBase::CompactInt { // no-forward-decl
  private:
    T val;

  public:
    CompactInt() : val(0){};
    CompactInt(T value) : val(value) {
        static_assert(std::is_arithmetic<T>::value, "T must be numeric");
    };
    explicit operator T() { return val; }
    void read(EventsIStream &istream);
    void write(EventsOStream &ostream) const;
};

/** \brief Class to read one event from history */
class EventsIStream : public EventsStreamBase {
  private:
    /** \brief Create a object that will allow read anything in a buffer
     *
     * This will be created by EventsIterator::read*()
     */
    EventsIStream(char *buffer, size_t lim) : EventsStreamBase(buffer, lim) {
        eventsize_t evsize;
        read(evsize);
        eventsize = evsize;
        if (marto_unlikely(eventsize == 0)) {
            /* Event not yet fully written (finalized not called)
             * or read fails
             */
            throw HistoryIncompleteEvent("Event not yet all written");
        }
        if (marto_unlikely(eventsize > lim)) {
            /* the event to read is longer than the buffer in the current chunk!
             */
            throw HistoryOutOfBound("Event too long for the current buffer!");
        }
        /* limiting the following reads to the event data */
        bufsize = eventsize - (buf - buffer);
    };
    friend event_access_t EventsIterator::loadNextEvent(Event *ev);
    template <typename T>
#if defined(__clang__)
    friend class EventsStreamBase::CompactInt;
#else
    friend void EventsStreamBase::CompactInt<T>::read(EventsIStream &istream);
#endif

    template <typename T> void read(T &var);

    /* convolution as partial template specialization is not allowed for
     * function Idea taken from
     * https://stackoverflow.com/questions/8061456/c-function-template-partial-specialization/21218271
     */
    template <bool is_integral, typename T>
    inline typename std::enable_if<is_integral, void>::type readvar(T &var) {
        CompactInt<T> cval;
        cval.read(*this);
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
    template <typename T> EventsIStream &operator>>(T &var) {
        static_assert(std::is_arithmetic<T>::value, "T must be numeric");
        readvar<std::is_integral<T>::value>(var);
        return *this;
    }

    /** \brief >> input stream operator that will compact the value
     */
    template <typename T> EventsIStream &operator>>(CompactInt<T> &var) {
        static_assert(std::is_integral<T>::value, "T must be integral");
        var.read(*this);
        return *this;
    }
};

/** \brief Class to write the content of one event in a buffer
 */
class EventsOStream : public EventsStreamBase {
  private:
    eventsize_t *eventSizePtr;
    /** \brief Create a object that will allow write anything in a buffer
     *
     * This will be created by EventsIterator::store*()
     */
    EventsOStream(char *buffer, size_t lim) : EventsStreamBase(buffer, lim) {
        eventSizePtr = write((eventsize_t)0);
    };
    friend event_access_t EventsIterator::storeNextEvent(Event *ev);
    template <typename T>
#if defined(__clang__)
    friend class EventsStreamBase::CompactInt;
#else
    friend void
    EventsStreamBase::CompactInt<T>::write(EventsOStream &ostream) const;
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
    template <typename T> EventsOStream &operator<<(const T &var) {
        static_assert(std::is_arithmetic<T>::value, "T must be numeric");
        writevar<std::is_integral<T>::value>(var);
        return *this;
    }
    /** \brief called when a store is interrupted */
    void abort() { eofbit = 1; }
    /** \brief finalize the write of the event in the history
     */
    event_access_t finalize();

  public:
    template <typename T> EventsOStream &operator<<(const CompactInt<T> &var) {
        static_assert(std::is_arithmetic<T>::value, "T must be numeric");
        var.write(*this);
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
