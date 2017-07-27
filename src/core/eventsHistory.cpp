#include <marto/eventsHistory.h>
#include <cstdint>

using namespace marto;

// EventsChunk

EventsChunk::EventsChunk(uint32_t capacity, EventsChunk *prev, EventsChunk *next) :
  allocOwner(true), eventsCapacity(capacity), nbEvents(0),
  nextChunk(next), prevChunk(prev)
{
	const size_t chunkSize=4096;
	bufferMemory = malloc(chunkSize);
	bufferStart = bufferMemory;
	bufferEnd = (char*)bufferMemory + chunkSize;
	freeSpace = chunkSize;
}

// EventsIterator

EventsIterator::EventsIterator(EventsHistory *hist) :
  direction(UNDEF)
  // position not set, as this depends on the direction
{
	curChunk=hist->firstChunk;
}


void *EventsHistory::getCurrentBuffer() {
	curChunk 
	return buffer+position;
}

int EventsIterator::loadNextEvent(Event *ev) {
	if (direction == UNDEF) {
		/* we should be at the start of the history */
	}
	// TODO: vérifier qu'on est pas à la fin d'un chunk d'events et passer au suivant si besoin
	auto nbRead = ev->load(this);
	position += nbRead;
	return nbRead;
}

int EventsIterator::storeNextEvent(Event *ev) {
	if (direction == UNDEF) {
		direction = FORWARD;
	}
	assert(direction == FORWARD);
	auto evSize = ev->size();
	if (! curChunk->reserveSpace(evSize)) {
		
	}
	// TODO: vérifier qu'on est pas à la fin d'un chunk d'events et passer au suivant si besoin
	auto nbRead = ev->load(this);
	position += nbRead;
	return nbRead;
}

int EventsIterator::storePrevEvent(Event *ev) {
	if (direction == UNDEF) {
		direction = BACKWARD;
	}
	assert(direction == BACKWARD);
	// TODO: vérifier qu'on est pas à la fin d'un chunk d'events et passer au suivant si besoin
	auto nbRead = ev->load(this);
	position += nbRead;
	return nbRead;
}

// EventsHistory

EventsHistory::EventsHistory(Configuration* conf) :
  configuration(conf), firstChunk(nullptr)
{

}

EventsIterator* EventsHistory::iterator() {
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
	EventsChunk	*chunk=new EventsChunk(nbEvents, nullptr, firstChunk);
	firstChunk = chunk;
}

