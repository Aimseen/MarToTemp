/* Event Generator */
#ifndef MARTO_EVENT_GENERATOR_H
#define MARTO_EVENT_GENERATOR_H

#ifdef __cplusplus

#include <marto/transition.h>

namespace marto {

	class EventType {
	public:
		marto::transition* transition;
		double rate;
		FormalParameters fp;
		GetParameter gp; /* ??? FIXME */
		int nbIntStaticParameters();
		int nbDoubleStaticParameters();
	};

	/* each simulation sequence only uses 1 object of type Event */
	class Event {
	public:
		/* Create an empty (unusable) event */
		Event(); 
		/* Load the event data from its serialization
		*  returns 1 upon success, 0 if the compact representation 
		*  does not match a known event type */
		int load(void* buffer);
		/* Stores a compact representation of the event
		*  returns the size of stored object or 0 upon failure
		*/
		int store(void *buffer, size_t buf_size);
		/* Creates (generate) a new Event
		*  returns 1 */
		int generate(EventType type);

		/* return the type of the Event */
		EventType *type();
		
		/* Parameters accessors */
		int8_t int8Parameter(int index);
		int16_t int16Parameter(int index);
		int32_t int32Parameter(int index);
		int64_t int64Parameter(int index);
		double doubleParameter(int index);
		int8_t int8Parameter(const String &pname);
		int16_t int16Parameter(const String &pname);
		int32_t int32Parameter(const String &pname);
		int64_t int64Parameter(const String &pname);
		double doubleParameter(const String &pname);

		void set(int index, int8_t value);
		void set(int index, int16_t value);
		void set(int index, int32_t value);
		void set(int index, int64_t value);
		void set(int index, double value);
	}
	
	class EventsHistory {
	public:
		int nbEvents();
		int curID(); /* numbered from 0 to nvEvents-1 */

		/* Get the current Event (NULL if empty) */
		Event* curEvent();
		
		/* Move between Events */
		void restart(); /* restart from start */
		int next(Event *ev); /* return 0 if last */

		/* Add new events */
		void pushEvent(Event ev); /* error if not at the end of history */
	}
}
#endif
	
#endif
