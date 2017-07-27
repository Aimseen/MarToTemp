/* Event Generator */
#ifndef MARTO_EVENTS_HISTORY_H
#define MARTO_EVENTS_HISTORY_H

#ifdef __cplusplus

#include <stdint.h>
#include <marto/global.h>

namespace marto {

	class EventsChunk;
	class EventsIterator;
	class EventsHistory;

	class EventsChunk {
	public:
		EventsChunk(uint32_t capacity, EventsChunk *prev, EventsChunk *next);
	private:
		bool allocOwner; // true if bufferMemory is malloc'ed
		void *bufferMemory;// beginning of allocated memory for chunk
		void *bufferStart; // beginning of history chunk; same as chunkStart ptr in the plain backward scheme. First event in chronological order
		void *bufferEnd; // end of history chunk = after last event in chronological order;
		size_t freeSpace;
		uint32_t eventsCapacity; // maximum number of events in this chunk and possible additional chunks
		uint32_t nbEvents; // current number of events in the chunk
		EventsChunk *nextChunk;
		EventsChunk *prevChunk;
		
		//size_t end; // end of buffer
	};
	
	class EventsIterator {
	private:
		EventsIterator(EventsHistory* hist);
		friend class EventsHistory;
	
	public:	
		/* Moving within the history */
		
		/** Fill ev with the next event, loading from the history
		 * Return 0 if no more events are available
		 */
		int loadNextEvent(Event *ev);
		
		/** TODO
		 * use only one of these methods to avoid overwrite
		 * Reminder :
		 * - when storing backward, if the current chunk has a previous chunk,
		 *   on should check that the capacity of this previous chunk is sufficient
		 */
		 int storeNextEvent(Event *ev);
		 int storePrevEvent(Event *ev);// for reverse trajectory algorithm

		/** return the current position in the current buffer
		 * To be used by Event::load only
		 */
		void *getCurrentBuffer();

	private:
		enum { UNDEF, FORWARD, BACKWARD } direction;
		EventsChunk *curChunk;
		size_t position; // current position in chunk in bytes
	};
	
	class EventsHistory {
	public:
		/* Returns a new generator starting at the new available stream associated
		 * to the current simulation context
		 * One stream per chunk (to be able to regenerate the same events)
		 */
		Random nextStream;// provides a clone and advances to netxt strem
		/** Initialize a new history of events */
		EventsHistory(Configuration* conf);
		EventsIterator *iterator(); // returns an iterator positioned at the begining

		/** Add some space in history for nbEvents *previous* events
		 * iterator() can be called to start at the (new) begining of the history
		 */
		void backward(uint32_t nbEvents); 
	
	private:
		Configuration *configuration;
		EventsChunk * firstChunk;// beginning of history
		friend EventsIterator::EventsIterator(EventsHistory *hist);
		//uint32_t _nbEvents; // useful ?
	};

}

#endif
	
#endif
