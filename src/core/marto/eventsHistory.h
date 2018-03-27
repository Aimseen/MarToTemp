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
    explicit operator bool() { return !eof(); };
    bool eof() { return eofbit; };
};

/** \brief Class to read one event from history */
class EventsIStream : public EventsStreamBase {
  private:
    /** \brief Create a object that will allow read anything in a buffer
     *
     * This will be created by EventsIterator::read*()
     */
    EventsIStream(char *buffer, size_t lim) : EventsStreamBase(buffer, lim) {
        *this >> eventsize;
        if (marto_unlikely(eventsize == 0)) {
            /* Event not yet fully written (finalized not called)
             * or read fails
             */
            throw new HistoryIncompleteEvent("Event not yet all written");
        }
        if (marto_unlikely(eventsize > lim)) {
            /* the event to read is longer than the buffer in the current chunk!
             */
            throw new HistoryOutOfBound(
                "Event too long for the current buffer!");
        }
        /* limiting the following reads to the event data */
        bufsize = eventsize - (buf - buffer);
    };
    friend event_access_t EventsIterator::loadNextEvent(Event *ev);

    template <typename T> void external_func_for_compiler(T &var);

    template <typename T> void read(T &var);

  public:
    /** \brief classical >> input stream operator
     */
    template <typename T> EventsIStream &operator>>(T &var) {
        static_assert(std::is_arithmetic<T>::value, "T must be numeric");
        read(var);
        return *this;
    }
    /** \brief conversion in bool for while loops */
};

/** \brief Class to write the content of one event in a buffer
 */
class EventsOStream : public EventsStreamBase {
  private:
    size_t *eventSizePtr;
    /** \brief Create a object that will allow write anything in a buffer
     *
     * This will be created by EventsIterator::store*()
     */
    EventsOStream(char *buffer, size_t lim) : EventsStreamBase(buffer, lim) {
        eventSizePtr = write((size_t)0);
    };
    ~EventsOStream() { finalize(); };
    friend event_access_t EventsIterator::storeNextEvent(Event *ev);

    template <typename T> T *write(const T &value);

  public:
    /** \brief classical << output stream operator
     */
    template <typename T> EventsOStream &operator<<(const T &var) {
        static_assert(std::is_arithmetic<T>::value, "T must be numeric");
        write(var);
        return *this;
    }
    /** \brief called when a store is interrupted */
    void abort() { eofbit = 1; }
    /** \brief finalize the write of the event in the history
     *
     * If not explicitely called, it will be called from destructor
     */
    void finalize() {
        if (eof()) {
            return;
        }
        size_t size = eventSize();
        assert(*eventSizePtr == 0 || *eventSizePtr == size);
        *eventSizePtr = size;
    };
};

/** \brief Class to manage an events history
 */
class EventsHistory {
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

    /* \brief Returns a new generator
     *
     * The new generator is starting at the new available stream
     * associated to the current simulation context One stream per
     * chunk (to be able to regenerate the same events)
     */
    Random nextStream; // FIXME: attribute or method? provides a clone and
                       // advances to next stream

  private:
    /// EventsIterator need to access to firstChunk
    friend EventsIterator::EventsIterator(EventsHistory *hist);
    Configuration *configuration;
    EventsChunk *firstChunk; ///< beginning of history
                             // uint32_t _nbEvents; // useful ?
  public:
    Configuration *getConfig() { return configuration; };

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
        const size_t s = 4096;
        char *buffer = (char *)malloc(s);
        if (size) {
            *size = buffer ? s : 0;
        }
        return buffer;
    }
    friend class EventsChunk;
};
}

#ifndef MARTO_H
// In case of direct inclusion (instead of using <marto.h>),
// we try to include the implementation, hoping to avoid include loops
#include <marto/eventsHistory-impl.h>
#endif

#endif
#endif
