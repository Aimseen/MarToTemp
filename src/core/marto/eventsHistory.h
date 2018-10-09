/* -*-c++-*- C++ mode for emacs */
/* Events History */
#ifndef MARTO_EVENTS_HISTORY_H
#define MARTO_EVENTS_HISTORY_H

#ifdef __cplusplus

#include <cassert>
#include <marto/except.h>
#include <marto/forwardDecl.h>
#include <marto/macros.h>
#include <marto/random.h>
#include <marto/types.h>
#include <memory>
#include <stdint.h>
#include <stdlib.h>

namespace marto {

//////////////////////////////////////////////////////////////////////////////
/** \brief class to manages chunk of events in memory
 *
 * These chunks will be linked in memory to create an history.
 *
 * The chunk notion should be transparent to the user of EventsHistory.
 *
 * Chunk are also used when some events must be inserted into an existing
 * history (not yet implemented)
 */
class EventsChunk {
    friend EventsIterator;
    friend EventsHistory;

  private:
    EventsChunk(uint32_t capacity, EventsChunk *prev, EventsChunk *next,
                EventsHistory *hist);
    ~EventsChunk();
    bool allocOwner; ///< true if bufferMemory is malloc'ed
    char *bufferMemory;
    char
        *bufferStart; ///< beginning of history chunk; same as chunkStart ptr in
    /// the plain backward scheme
    char *bufferEnd;         ///< end of history chunk;
    uint32_t eventsCapacity; ///< maximum number of allowed events in this chunk
    /// and possible additional chunks
    uint32_t nbEvents;      ///< current number of events in the chunk
    EventsChunk *nextChunk; ///< always later in simulated time
    EventsChunk *prevChunk; ///< always earlier in simulated time
    EventsHistory *history; ///< history this chunk belong to

    /** \brief return the next chunk in the history
     *
     * \return NULL at the end of the history.
     *
     * \note this accessor is provided as synchronization will be required
     * when simulating concurrent trajectories (not yet implemented)
     */
    EventsChunk *getNextChunk();
    /** \brief allocate a new chunk in the history
     *
     * needed when current chunk is full
     * The new chunk is placed just after the current one (in simulated time)
     * Its event capacity is set to the remaining of the current one
     * The capacity of the current chunk is adjusted its current number of
     * events
     */
    EventsChunk *allocateNextChunk();
};

//////////////////////////////////////////////////////////////////////////////
/** \brief class to allow one to read the history and to write new events
 *
 * \note some parallel version will be to be implemented
 */
class EventsIterator {
  private:
    EventsIterator(EventsHistory *hist);
    // friend EventsIterator *EventsHistory::iterator();
    friend class EventsHistory;

  public:
    /** \brief Fill ev with the next event, loading from the history
     */
    event_access_t loadNextEvent(Event *ev);

    /** \brief Write the event in the history.
     *
     * Some place (for events) must be available at the current position
     */
    event_access_t storeNextEvent(Event *ev);
    /* we do not store events reversely. Never. If really required, we
     * should go back for several events and generate and store them
     * forward.

    int storePrevEvent(Event * ev); // for reverse trajectory algorithm
    */

  private:
    EventsChunk *setNewChunk(EventsChunk *chunk);

    EventsChunk *curChunk;
    char *position; ///< current position in the chunk buffer
    uint32_t
        eventNumber; ///< # event in the current chunk to be read or written

    /** Load the event data from its serialization
     *
     */
    event_access_t loadEventContent(EventsIStream &istream, Event *event);
    /** Stores a compact representation of the event
     *
     *  Note: the store can be aborted (with an exception)
     *  if the current chunk buffer is not bif enough
     *  In this case, storeNextEvent will restart the
     *  call to this function in a new chunk.
     */
    event_access_t storeEventContent(EventsOStream &ostream, Event *event);
};

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
template <typename T> class EventsStreamBase::CompactInt {
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
    friend void EventsStreamBase::CompactInt<T>::read(EventsIStream &istream);

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
    friend void
    EventsStreamBase::CompactInt<T>::write(EventsOStream &ostream) const;

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

/** \brief Class to manage an events history
 */
class EventsHistory : protected WithConfiguration {
  public:
    /** \brief Initialize a new history of events */
    EventsHistory(Configuration *conf);
    /** \brief Get an iterator positioned at the begining of the history */
    EventsIterator *iterator();

    /** \brief Add some space in history
     *
     * the space has a capacity of nbEvents and is inserted *before*
     * the previous start of the history
     *
     * iterator() can be called to start at the (new) begining of the
     * history
     */
    void backward(uint32_t nbEvents);

    // DELETED : the next stream is provided by the configuration (globalized
    // service)
    /* \brief generator for the current history
     *
     * This generator is starting at a new available stream
     * associated to the current simulation context One stream per
     * chunk (to be able to regenerate the same events)
     */
    // Random *stream; // FIXME: attribute or method? provides a clone and
    // advances to next stream

  private:
    /// EventsIterator needs to access to firstChunk
    friend EventsIterator::EventsIterator(EventsHistory *hist);
    /// loadEventContent needs to access to the configuration
    friend event_access_t
    EventsIterator::loadEventContent(EventsIStream &istream, Event *ev);
    EventsChunk *firstChunk; ///< beginning of history
                             // uint32_t _nbEvents; // useful ?
  protected:
    /** \brief Allocate Memory for a chunk
     * \param size must be filled with the size of the allocated buffer if not
     * NULL
     * \return address of a buffer that can be used by an EventsChunk
     *
     * The default implementation allocates 4096 bytes but any derived
     * class can choose different sizes
     */
    virtual char *allocChunkBuffer(size_t *size) {
        const size_t defaultBufferSize = 4096;
        char *buffer = (char *)malloc(defaultBufferSize);
        if (size) {
            *size = buffer ? defaultBufferSize : 0;
        }
        return buffer;
    }
    friend class EventsChunk;
};
} // namespace marto

#ifndef MARTO_H
// In case of direct inclusion (instead of using <marto.h>),
// we try to include the implementation, hoping to avoid include loops
#include <marto/eventsHistory-impl.h>
#endif

#endif
#endif
