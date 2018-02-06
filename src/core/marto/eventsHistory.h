/* -*-c++-*- C++ mode for emacs */
/* Event Generator */
#ifndef MARTO_EVENTS_HISTORY_H
#define MARTO_EVENTS_HISTORY_H

#ifdef __cplusplus

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <marto/random.h>
#include <marto/except.h>
#include <cassert>

namespace marto {

class EventsChunk;
class EventsIterator;
class EventsHistory;
class Configuration;
class Event;

//////////////////////////////////////////////////////////////////////////////
/** This class manages chunk of events in memory
 *
 * These chunks will be linked in memory to create an history.
 * The chunk notion should be transparent to the user of EventsHistory.
 * Chunk are also used when some events must be inserted into an existing history.
 */
class EventsChunk {
    friend EventsIterator;
    friend EventsHistory;
private:
    EventsChunk(uint32_t capacity, EventsChunk * prev, EventsChunk * next);
    bool allocOwner;       // true if bufferMemory is malloc'ed
    char *bufferMemory;
    char *bufferStart;      // beginning of history chunk; same as chunkStart ptr in the plain backward scheme
    char *bufferEnd;        // end of history chunk;
    uint32_t eventsCapacity;// maximum number of events in this chunk and possible additional chunks
    uint32_t nbEvents;      // current number of events in the chunk
    EventsChunk *nextChunk;
    EventsChunk *prevChunk;

    /** return the next chunk in the history
     *
     * return NULL at the end of the history.
     */
    EventsChunk *getNextChunk();
    /** allocate a new chunk in the history
     *
     * The new chunk is placed just after the current one
     * Its capacity is set to the remainding of the current one
     * The capacity of the current chunk is adjusted its current number of events
     */
    EventsChunk *allocateNextChunk();
};

//////////////////////////////////////////////////////////////////////////////
/** This class allows one to read the history and to write new events
 *
 * Note: some parallel version will be to be implemented
 */
class EventsIterator {
private:
    EventsIterator(EventsHistory * hist);
    //friend EventsIterator *EventsHistory::iterator();
    friend class EventsHistory;
public:
    typedef enum {
        EVENT_LOADED=0,
        EVENT_WRITTEN=0,
        END_OF_HISTORY, /* the end of history is reached */
        UNDEFINED_EVENT, /* trying to load an event not yet generated */

    } event_access_t;

    /** Fill ev with the next event, loading from the history
     */
    event_access_t loadNextEvent(Event * ev);

    /** Write the event in the history.
     *
     * Some place (for events) must be available at the current position
     */
    event_access_t storeNextEvent(Event * ev);
    /* we do not store events reversly. Never. If really required, we
     * should go back for several events and generate and store them
     * forward.

    int storePrevEvent(Event * ev); // for reverse trajectory algorithm
    */

    /** Get the history linked to this iterator */
    EventsHistory *history() {
        return _history;
    }

private:
    EventsChunk *setNewChunk(EventsChunk *chunk);

    EventsChunk *curChunk;
    char *position;         // current position in the chunk buffer
    uint32_t eventNumber;   // # event in the current chunk to be read or written

    EventsHistory *_history;
};

class EventsStreamBase {
    /** forbid copy of this kind of objects */
    EventsStreamBase(const EventsStreamBase &) = delete;
    /** forbid assignment of this kind of objects */
    EventsStreamBase &operator=(const EventsStreamBase &) = delete;
protected:
    char *buf;
    size_t bufsize;
    size_t eventsize;

    /** Create a object that consumes a bounded buffer
     * It will be inherited by Events[IO]Stream
     */
    EventsStreamBase(char* buffer, size_t lim):
        buf(buffer), bufsize(lim), eventsize(0) {};
    size_t eventSize() {
        return eventsize;
    };
};

class EventsIStream : EventsStreamBase {
private:
    /** Create a object that will allow read anything in a buffer
     * This will be created by EventsIterator::read*()
     */
    EventsIStream(char* buffer, size_t lim):
        EventsStreamBase(buffer, lim) {

        *this >> eventsize;
        if (marto_unlikely(eventsize == 0)) {
            throw new HistoryIncompleteEvent("Event not yet all written");
        }
        if (marto_unlikely(eventsize > lim)) {
            /* the event to read is longer than the buffer in the current chunk! */
            throw new HistoryOutOfBound("Event too long for the current buffer!");
        }
        /* limiting the following reads to the event data */
        bufsize = eventsize - (buf-buffer);
    };
    friend EventsIterator::event_access_t EventsIterator::loadNextEvent(Event *ev);

    template<typename T> T read() {
        void* ptr=(void*)buf;
        if (!std::align(alignof(T), sizeof(T), ptr, bufsize)) {
            /* no enough place in the buffer! */
            throw new HistoryOutOfBound("No enough place in the current buffer to read the requested data!");
        }
        T *newbuf=((T*)ptr)+1;
        size_t size = ((char*)newbuf - buf);
        bufsize -= size;
        buf = (char*)newbuf;
        return *(T*)ptr;
    };

public:
    template<typename T> EventsIStream& operator>>(T& var) {
        var=read<T>();
        return *this;
    };
};

/** Class to write the content of an event in a buffer
 */
class EventsOStream : EventsStreamBase {
private:
    size_t *eventSizePtr;
    /** Create a object that will allow write anything in a buffer
     * This will be created by EventsIterator::store*()
     */
    EventsOStream(char* buffer, size_t lim):
        EventsStreamBase(buffer, lim) {
        eventSizePtr = write((size_t)0);
    };
    ~EventsOStream() {
        finalize();
    };
    friend EventsIterator::event_access_t EventsIterator::storeNextEvent(Event *ev);

    template<typename T> T* write(const T &value) {
        void* ptr=(void*)buf;
        if (!std::align(alignof(T), sizeof(T), ptr, bufsize)) {
            throw new HistoryOutOfBound("Not enough place for the current event");
        }
        T *newbuf=((T*)ptr)+1;
        size_t size = ((char*)newbuf - buf);
        bufsize -= size;
        eventsize += size;
        buf = (char*)newbuf;
        *(T*)ptr=value;
        return (T*)ptr;
    };

public:
    template<typename T> EventsOStream& operator<<(const T& var) {
        write(var);
        return *this;
    };
    void finalize() {
        size_t size=eventSize();
        assert(*eventSizePtr == 0 || *eventSizePtr==size);
        *eventSizePtr=size;
    };
};

class EventsHistory {
public:
    /** Initialize a new history of events */
    EventsHistory(Configuration * conf);
    /** Get an iterator positioned at the begining of the history */
    EventsIterator *iterator();

    /** Add some space in history for nbEvents *before* the previous begining
     * iterator() can be called to start at the (new) begining of the history
     */
    void backward(uint32_t nbEvents);

    /* Returns a new generator starting at the new available stream
     * associated to the current simulation context One stream per
     * chunk (to be able to regenerate the same events)
     */
    Random nextStream;// provides a clone and advances to next stream

private:
    // EventsIterator need to access to firstChunk
    friend EventsIterator::EventsIterator(EventsHistory *hist);
    Configuration * configuration;
    EventsChunk *firstChunk;        // beginning of history
    //uint32_t _nbEvents; // useful ?
public:
    Configuration * getConfig() {
        return configuration;
    };
};

}
#endif
#endif
