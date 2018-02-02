/* -*-c++-*- C++ mode for emacs */
/* Event Generator */
#ifndef MARTO_EVENTS_HISTORY_H
#define MARTO_EVENTS_HISTORY_H

#ifdef __cplusplus

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <marto/random.h>
#include <assert.h>

namespace marto {

class EventsChunk;
class EventsIterator;
class EventsHistory;
class Configuration;
class Event;

class EventsChunk {
    friend EventsIterator;
public:
    EventsChunk(uint32_t capacity, EventsChunk * prev, EventsChunk * next);
private:
    bool allocOwner;       // true if bufferMemory is malloc'ed
    char *bufferMemory;
    char *bufferStart;      // beginning of history chunk; same as chunkStart ptr in the plain backward scheme
    char *bufferEnd;        // end of history chunk;
    size_t freeSpace;
    uint32_t eventsCapacity;        // maximum number of events in this chunk and possible additional chunks
    uint32_t nbEvents;      // current number of events in the chunk
    EventsChunk *nextChunk;
    EventsChunk *prevChunk;

    //size_t end; // end of buffer
};

class EventsIterator {
private:
    EventsIterator(EventsHistory * hist);
    //friend EventsIterator *EventsHistory::iterator();
    friend class EventsHistory;
public:
    /* Moving within the history */

    /** Fill ev with the next event, loading from the history
    * Return 0 if no more events are available
    	 */
    int loadNextEvent(Event * ev);

    /** TODO
    * use only one of these methods to avoid overwrite
    	 * Reminder :
    	 * - when storing backward, if the current chunk has a previous chunk,
    	 *   on should check that the capacity of this previous chunk is sufficient
    	 */
    int storeNextEvent(Event * ev);
    /* we do not store events reversly. Never. If really required, we
     * should go back for several events and generate and store them
     * forward.

    int storePrevEvent(Event * ev); // for reverse trajectory algorithm
    */

    EventsHistory *history() {
        return _history;
    }

private:
    /** return the current position in the current buffer
    * To be used by Event::load only
    	 */
    char *getCurrentBuffer();

private:
    enum { UNDEF, FORWARD, BACKWARD } direction;
    EventsChunk *curChunk;
    size_t position;        // current position in chunk in bytes
    EventsHistory *_history;
};

// FIXME: move exception class at better place
class ChunkHistoryOutOfMemory {};
class ChunkHistoryIncompleteEvent {};
// FIXME: define this kind of macros in a better place
#define marto_likely(x)       __builtin_expect((x),1)
#define marto_unlikely(x)     __builtin_expect((x),0)

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
        if (marto_unlikely(eventsize == -1)) {
            throw new ChunkHistoryIncompleteEvent();
        }
        if (marto_unlikely(eventsize > lim)) {
            /* the event to read is longer than the buffer in the current chunk! */
            throw new ChunkHistoryOutOfMemory();
        }
        /* limiting the following reads to the event data */
        bufsize = eventsize - (buf-buffer);
    };
    friend int EventsIterator::loadNextEvent(Event *ev);

    template<typename T> T read() {
        void* ptr=(void*)buf;
        if (!std::align(alignof(T), sizeof(T), ptr, bufsize)) {
            /* no enough place in the buffer! */
            throw new ChunkHistoryOutOfMemory();
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
        eventSizePtr = write((size_t)-1);
    };
    ~EventsOStream() {
        finalize();
    };
    friend int EventsIterator::storeNextEvent(Event *ev);

    template<typename T> T* write(const T &value) {
        void* ptr=(void*)buf;
        if (!std::align(alignof(T), sizeof(T), ptr, bufsize)) {
            throw new ChunkHistoryOutOfMemory();
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
        assert(*eventSizePtr == -1 || *eventSizePtr==size);
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
