#include <marto/eventsHistory.h>
#include <marto/event.h>
#include <cstdint>

#include <cassert>

namespace marto {
// EventsChunk

EventsChunk::EventsChunk(uint32_t capacity, EventsChunk * prev,
                         EventsChunk * next, EventsHistory * hist):
    allocOwner(true), eventsCapacity(capacity), nbEvents(0),
    nextChunk(next), prevChunk(prev), history(hist)
{
    size_t bufferSize;
    bufferMemory = history->allocChunkBuffer(&bufferSize);
    assert(bufferMemory != nullptr);
    bufferStart = bufferMemory;
    bufferEnd = bufferMemory + bufferSize;
}

EventsChunk::~EventsChunk() {
    if (allocOwner) {
        // TODO: a reference counter should probably be used when/if we use
        // the same buffer fore different chunks
        free(bufferMemory);
    }
}

EventsChunk* EventsChunk::getNextChunk() {
    return nextChunk;
}

EventsChunk* EventsChunk::allocateNextChunk() {
    uint32_t capacity;
    if (marto_unlikely(eventsCapacity == UINT32_MAX)) {
        capacity=UINT32_MAX;
    } else {
        capacity=eventsCapacity-nbEvents;
    }
    EventsChunk* newChunk=new EventsChunk(capacity, this, nextChunk, history);
    if (nextChunk) {
        nextChunk->prevChunk=newChunk;
    }
    nextChunk=newChunk;
    eventsCapacity=nbEvents;
    return nextChunk;
}

// EventsIterator

EventsIterator::EventsIterator(EventsHistory * hist) {
    setNewChunk(hist->firstChunk);
}

EventsChunk *EventsIterator::setNewChunk(EventsChunk *chunk) {
    curChunk=chunk;
    if (marto_unlikely(chunk == nullptr)) {
        return nullptr;
    }
    position=curChunk->bufferStart;
    eventNumber=0;
    return chunk;
};

EventsIterator::event_access_t EventsIterator::loadNextEvent(Event * ev) {
    assert(curChunk != nullptr);
    while (marto_unlikely(eventNumber >= curChunk->eventsCapacity)) {
        if (marto_unlikely(setNewChunk(curChunk->getNextChunk())==nullptr)) {
            return END_OF_HISTORY;
        }
    }
    if (marto_unlikely(eventNumber >= curChunk->nbEvents)) {
        return UNDEFINED_EVENT;
    }
    char*buffer=position;
    EventsIStream istream(
        buffer,
        curChunk->bufferEnd-buffer
    );
    auto evRead = ev->load(curChunk->history, istream);
    position += istream.eventSize();
    eventNumber ++;

    // For now, failed read should never occurs
    assert(evRead);

    return EVENT_LOADED;
}

EventsIterator::event_access_t EventsIterator::storeNextEvent(Event *ev) {
    assert(curChunk != nullptr);
    while (marto_unlikely(eventNumber >= curChunk->eventsCapacity)) {
        if (marto_unlikely(setNewChunk(curChunk->getNextChunk())==nullptr)) {
            return END_OF_HISTORY;
        }
    }
    do {
        /* We must be at the end of a chunk */
        assert(eventNumber == curChunk->nbEvents);

        char*buffer=position;
        EventsOStream ostream(
            buffer,
            curChunk->bufferEnd-buffer   // available size
        );

        try {
            auto evWritten=ev->store(curChunk->history, ostream);
            ostream.finalize();  // used to store the event size in chunk
            position += ostream.eventSize();
            eventNumber ++;

            // For now, failed write should never occurs
            assert(evWritten);

            break; // escape the infinite loop
        } catch (HistoryOutOfBound const& h) {
            if (marto_unlikely(setNewChunk(curChunk->allocateNextChunk())==nullptr)) {
                return END_OF_HISTORY;
            }
        }
    } while(true); // start over after creating new chunk

    return EVENT_WRITTEN;
}

// EventsHistory

EventsHistory::EventsHistory(Configuration * conf):configuration(conf), firstChunk(nullptr) {

}

EventsIterator *EventsHistory::iterator() {
    if (firstChunk == nullptr) {
        // Empty history, creating a chunk
        // no need to restrict the number of events
        firstChunk = new EventsChunk(UINT32_MAX, nullptr, nullptr, this);
    }
    return new EventsIterator(this);
}

void EventsHistory::backward(uint32_t nbEvents) {
    if (nbEvents == 0) {
        return;
    }
    EventsChunk *chunk = new EventsChunk(nbEvents, nullptr, firstChunk, this);
    if (firstChunk) {
        firstChunk->prevChunk=chunk;
    }
    firstChunk = chunk;
}

}
