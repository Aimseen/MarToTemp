#include <cstdint>
#include <marto/event.h>
#include <marto/eventsHistory.h>

#include <cassert>

namespace marto {
// EventsChunk

EventsChunk::EventsChunk(uint32_t capacity, EventsChunk *prev,
                         EventsChunk *next, EventsHistory *hist)
    : allocOwner(true), eventsCapacity(capacity), nbEvents(0), nextChunk(next),
      prevChunk(prev), history(hist) {
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

EventsChunk *EventsChunk::getNextChunk() { return nextChunk; }

EventsChunk *EventsChunk::allocateNextChunk() {
    uint32_t capacity;
    if (marto_unlikely(eventsCapacity == UINT32_MAX)) {
        capacity = UINT32_MAX;
    } else {
        capacity = eventsCapacity - nbEvents;
    }
    EventsChunk *newChunk = new EventsChunk(capacity, this, nextChunk, history);
    if (nextChunk) {
        nextChunk->prevChunk = newChunk;
    }
    nextChunk = newChunk;
    eventsCapacity = nbEvents;
    return nextChunk;
}

// EventsIterator

EventsIterator::EventsIterator(EventsHistory *hist) {
    setNewChunk(hist->firstChunk);
}

EventsChunk *EventsIterator::setNewChunk(EventsChunk *chunk) {
    curChunk = chunk;
    if (marto_unlikely(chunk == nullptr)) {
        return nullptr;
    }
    position = curChunk->bufferStart;
    eventNumber = 0;
    return chunk;
};

event_access_t EventsIterator::loadNextEvent(Event *ev) {
    assert(curChunk != nullptr);
    while (marto_unlikely(eventNumber >= curChunk->eventsCapacity)) {
        if (marto_unlikely(setNewChunk(curChunk->getNextChunk()) == nullptr)) {
            return END_OF_HISTORY;
        }
    }
    if (marto_unlikely(eventNumber >= curChunk->nbEvents)) {
        return UNDEFINED_EVENT;
    }
    char *buffer = position;
    EventsIStream istream(buffer, curChunk->bufferEnd - buffer);
    auto evRead = loadEventContent(istream, ev);
    position += istream.eventSize();
    eventNumber++;

    // For now, failed read should never occurs
    assert(evRead == EVENT_LOADED);

    return EVENT_LOADED;
}

event_access_t EventsIterator::storeNextEvent(Event *ev) {
    assert(curChunk != nullptr);
    while (marto_unlikely(eventNumber >= curChunk->eventsCapacity)) {
        if (marto_unlikely(setNewChunk(curChunk->getNextChunk()) == nullptr)) {
            return END_OF_HISTORY;
        }
    }
    do {
        /* We must be at the end of a chunk */
        assert(eventNumber == curChunk->nbEvents);

        char *buffer = position;
        EventsOStream ostream(buffer,
                              curChunk->bufferEnd - buffer // available size
                              );

        try {
            event_access_t access = storeEventContent(ostream, ev);
            if (access != EVENT_STORED) {
                ostream.abort();
            } else {
                ostream.finalize(); // used to store the event size in chunk
                position += ostream.eventSize();
                eventNumber++;
                curChunk->nbEvents++;
            }
            return access;
        } catch (HistoryOutOfBound const &h) {
            if (marto_unlikely(setNewChunk(curChunk->allocateNextChunk()) ==
                               nullptr)) {
                return END_OF_HISTORY;
            }
        }
    } while (true); // start over after creating new chunk
}

event_access_t EventsIterator::storeEventContent(EventsOStream &ostream,
                                                 Event *ev) {
    assert(ev != nullptr);
    if (marto_unlikely(!ev->valid())) {
        return EVENT_STORE_UNDEFINED_ERROR;
    }
    EventType *type = ev->type();
    if (ostream << type->code()) {
        return type->store(ostream, ev, curChunk->history);
    }
    return EVENT_STORE_ERROR;
}

/*  an eventType is stored compactly as an integer
   (example of event type : arrival in queue 1)
   it is read from a table built from user config file
 */
event_access_t EventsIterator::loadEventContent(EventsIStream &istream,
                                                Event *ev) {
    Event::code_t code;
    assert(ev != nullptr);
    ev->clear();
    if (istream >> code) { // eventype is encoded in the first integer of the
        // event buffer.
        EventsHistory *hist = curChunk->history;
        EventType *type = hist->getConfig()->getEventType(code);
        if (ev->setType(type)) {
            if (type->load(istream, ev, hist)) {
                return EVENT_LOADED;
            }
        }
    }
    return EVENT_LOAD_CODE_ERROR;
}

// EventsHistory

EventsHistory::EventsHistory(Configuration *conf)
    : configuration(conf), firstChunk(nullptr) {}

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
        firstChunk->prevChunk = chunk;
    }
    firstChunk = chunk;
}
}
