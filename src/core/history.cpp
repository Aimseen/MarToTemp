#include <cstdint>
#include <marto/event.h>
#include <marto/history.h>
#include <marto/historyStream.h>

#include <cassert>

namespace marto {
// EventsChunk

HistoryChunk::HistoryChunk(uint32_t capacity, HistoryChunk *prev,
                           HistoryChunk *next,
                           /*Random *rand, */ History *hist)
    : allocOwner(true), eventsCapacity(capacity), nbEvents(0), nextChunk(next),
      prevChunk(prev), history(hist) {
    size_t bufferSize;
    bufferMemory = history->allocChunkBuffer(&bufferSize);
    assert(bufferMemory != nullptr);
    bufferStart = bufferMemory;
    bufferEnd = bufferMemory + bufferSize;
    // if (rand == nullptr) {
    //    //TODO
    //} else {
    //
    //}
}

HistoryChunk::~HistoryChunk() {
    if (allocOwner) {
        // TODO: a reference counter should probably be used when/if we use
        // the same buffer fore different chunks
        free(bufferMemory);
    }
}

HistoryChunk *HistoryChunk::getNextChunk() { return nextChunk; }

HistoryChunk *HistoryChunk::allocateNextChunk() {
    uint32_t capacity;
    if (marto_unlikely(eventsCapacity == UINT32_MAX)) {
        capacity = UINT32_MAX;
    } else {
        capacity = eventsCapacity - nbEvents;
    }
    HistoryChunk *newChunk =
        new HistoryChunk(capacity, this, nextChunk, history);
    if (nextChunk) {
        nextChunk->prevChunk = newChunk;
    }
    nextChunk = newChunk;
    eventsCapacity = nbEvents;
    return nextChunk;
}

// HistoryIterator

HistoryIterator::HistoryIterator(History *hist) {
    setNewChunk(hist->firstChunk);
}

HistoryChunk *HistoryIterator::setNewChunk(HistoryChunk *chunk) {
    curChunk = chunk;
    if (marto_unlikely(chunk == nullptr)) {
        return nullptr;
    }
    position = curChunk->bufferStart;
    eventNumber = 0;
    return chunk;
};

history_access_t HistoryIterator::loadNextEvent(Event *ev) {
    assert(curChunk != nullptr);
    while (marto_unlikely(eventNumber >= curChunk->eventsCapacity)) {
        if (marto_unlikely(setNewChunk(curChunk->getNextChunk()) == nullptr)) {
            return HISTORY_END_HISTORY;
        }
    }
    if (marto_unlikely(eventNumber >= curChunk->nbEvents)) {
        return HISTORY_END_DATA;
    }
    char *buffer = position;
    HistoryIStream istream(buffer, curChunk->bufferEnd - buffer);
    auto evRead = loadEventContent(istream, ev);

    if (evRead == HISTORY_DATA_LOADED) {
        position += istream.objectSize();
        eventNumber++;
    }

    return evRead;
}

history_access_t HistoryIterator::readyToStore() {
    assert(curChunk != nullptr);
    while (marto_unlikely(eventNumber >= curChunk->eventsCapacity)) {
        if (marto_unlikely(setNewChunk(curChunk->getNextChunk()) == nullptr)) {
            return HISTORY_END_HISTORY;
        }
    }
    /* We must be at the end of a chunk */
    if (eventNumber != curChunk->nbEvents) {
        return HISTORY_DATA_STORE_ERROR;
    }
    return HISTORY_END_DATA;
}

history_access_t HistoryIterator::generateNextEvent(Event *ev) {
    if (readyToStore() != HISTORY_END_DATA) {
        return HISTORY_DATA_STORE_ERROR;
    }
    // TODO
}

history_access_t HistoryIterator::getNextEvent(Event *ev) {
    history_access_t ret;
    ret = loadNextEvent(ev);
    if (ret == HISTORY_END_DATA) {
        ret = generateNextEvent(ev);
    }
    return ret;
}

history_access_t HistoryIterator::storeNextEvent(Event *ev) {
    bool new_chunk = false;
    do {
        /* We must be at the end of a chunk */
        if (readyToStore() != HISTORY_END_DATA) {
            return HISTORY_DATA_STORE_ERROR;
        }

        char *buffer = position;

        try {
            HistoryOStream ostream(
                buffer,
                curChunk->bufferEnd - buffer // available size
            );

            history_access_t access = storeEventContent(ostream, ev);
            if (access != HISTORY_DATA_STORED) {
                ostream.abort();
            } else {
                access =
                    ostream.finalize(); // used to store the event size in chunk
                if (access != HISTORY_DATA_STORED) {
                    ostream.abort();
                } else {
                    position += ostream.objectSize();
                    eventNumber++;
                    curChunk->nbEvents++;
                }
            }
            return access;
        } catch (HistoryOutOfBound const &h) {
            if (marto_unlikely(new_chunk)) {
                // event too big to be stored in one chunk
                return HISTORY_OBJECT_TOO_BIG;
            }
            if (marto_unlikely(setNewChunk(curChunk->allocateNextChunk()) ==
                               nullptr)) {
                return HISTORY_END_HISTORY;
            }
            new_chunk = true;
        }
    } while (true); // start over after creating new chunk
}

history_access_t HistoryIterator::storeEventContent(HistoryOStream &ostream,
                                                    Event *ev) {
    assert(ev != nullptr);
    if (marto_unlikely(!ev->valid())) {
        return HISTORY_STORE_INVALID_EVENT;
    }
    EventType *type = ev->type();
    if (ostream << type->code()) {
        return type->store(ostream, ev);
    }
    return HISTORY_DATA_STORE_ERROR;
}

/*  an eventType is stored compactly as an integer
   (example of event type : arrival in queue 1)
   it is read from a table built from user config file
 */
history_access_t HistoryIterator::loadEventContent(HistoryIStream &istream,
                                                   Event *ev) {
    Event::code_t code;
    assert(ev != nullptr);
    ev->clear();
    if (istream >> code) { // eventype is encoded in the first integer of the
        // event buffer.
        History *hist = curChunk->history;
        EventType *type = hist->config()->getEventType(code);
        if (ev->setType(type)) {
            return type->load(istream, ev);
        }
    }
    return HISTORY_DATA_LOAD_ERROR;
}

// History

History::History(Configuration *conf)
    : WithConfiguration(conf), firstChunk(nullptr) {}

HistoryIterator *History::iterator() {
    if (firstChunk == nullptr) {
        // Empty history, creating a chunk
        // no need to restrict the number of events
        firstChunk = new HistoryChunk(UINT32_MAX, nullptr, nullptr, this);
    }
    return new HistoryIterator(this);
}

void History::backward(uint32_t nbEvents) {
    if (nbEvents == 0) {
        return;
    }
    HistoryChunk *chunk = new HistoryChunk(nbEvents, nullptr, firstChunk, this);
    if (firstChunk) {
        firstChunk->prevChunk = chunk;
    }
    firstChunk = chunk;
}
} // namespace marto
