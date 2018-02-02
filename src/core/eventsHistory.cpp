#include <marto/eventsHistory.h>
#include <marto/event.h>
#include <cstdint>

#include <assert.h>

namespace marto {
// EventsHistory

EventsHistory::EventsHistory(Configuration * conf):configuration(conf), firstChunk(nullptr) {

}

EventsIterator *EventsHistory::iterator() {
    if (firstChunk == nullptr) {
        // Empty history, creating a chunk
        // no need to restrict the number of events
        backward(UINT32_MAX);
    }
    return new EventsIterator(this);
}

void EventsHistory::backward(uint32_t nbEvents) {
    if (nbEvents <= 0) {
        return;
    }
    EventsChunk *chunk = new EventsChunk(nbEvents, nullptr, firstChunk);
    firstChunk = chunk;
}

// EventsChunk

EventsChunk::EventsChunk(uint32_t capacity, EventsChunk * prev, EventsChunk * next):
    allocOwner(true), eventsCapacity(capacity), nbEvents(0), nextChunk(next), prevChunk(prev) {
    const size_t chunkSize = 4096;
    bufferMemory = (char *) malloc(chunkSize);
    bufferStart = bufferMemory;
    bufferEnd = bufferMemory + chunkSize;
    freeSpace = chunkSize;
}

// EventsIterator

EventsIterator::EventsIterator(EventsHistory * hist):
    direction(UNDEF), curChunk(hist->firstChunk), _history(hist)
    // position not set, as this depends on the direction
{

}


char *EventsIterator::getCurrentBuffer() {
    // TODO: check for invalid pointer and allocate if required
    return curChunk->bufferStart + position;
}

int EventsIterator::loadNextEvent(Event * ev) {
    // TODO: handle null chunk and other error/initialisation cases
    // TODO: handle backward run
    if (direction == UNDEF) {
        /* we should be at the start of the history */
    }
    char*buffer=getCurrentBuffer();
    EventsIStream istream(
        buffer,
        curChunk->bufferEnd-buffer
    );
    // TODO: vérifier qu'on est pas à la fin d'un chunk d'events et passer au suivant si besoin
    auto evRead = ev->load(history(), istream);
    position += istream.eventSize();

    // For now, failed read should never occurs
    assert(evRead);

    return evRead;
}

int EventsIterator::storeNextEvent(Event *ev) {
    if (direction == UNDEF) {
        direction = FORWARD;
    }
    assert(direction == FORWARD);
    // TODO :: handle null chunk and other error/initialisation cases
    char*buffer=getCurrentBuffer();
    EventsOStream ostream(
        buffer,
        curChunk->bufferEnd-buffer
    );
    // TODO: vérifier qu'on est pas à la fin d'un chunk d'events et passer au suivant si besoin
    auto evWritten=ev->store(history(), ostream);
    ostream.finalize();
    position += ostream.eventSize();

    // For now, failed read should never occurs
    assert(evWritten);

    return evWritten;
}

}
