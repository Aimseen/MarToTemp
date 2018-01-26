#include <marto/eventsHistory.h>
#include <marto/global.h>
#include <cstdint>

using namespace marto;

// EventsHistory

EventsHistory::EventsHistory(Configuration * conf):
configuration(conf), firstChunk(nullptr) {

}

EventsIterator *iterator() {
    if (firstChunk == nullptr) {
        // Empty history, creating a chunk
        // no need to restrict the number of events
        backward(UNIT32_MAX);
    }
    return new EventsIterator::EventsIterator(this);
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
    bufferMemory = malloc(chunkSize);
    bufferStart = bufferMemory;
    bufferEnd = bufferMemory + chuckSize;
    freeSpace = chunkSize;
}

// EventsIterator

EventsIterator::EventsIterator(EventsHistory * hist):
direction(UNDEF), curChunk(hist->firstChunk)
    // position not set, as this depends on the direction
{

}


void *EventsHistory::getCurrentBuffer() {
    curChunk return buffer + position;
}

Configuration *EventsHistory::getConfig() {
    return configuration;
}

int EventsHistory::loadNextEvent(Event * ev) {
    // TODO: vérifier qu'on est pas à la fin d'un chunk d'events et passer au suivant si besoin
    auto nbRead = ev->load(this);
    position += nbRead;
    return nbRead;
}
