/* Event Generator */
#ifndef MARTO_EVENT_GENERATOR_H
#define MARTO_EVENT_GENERATOR_H

#ifdef __cplusplus

#include <marto/transition.h>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <map>


namespace marto {

	class EventsChunk;
	class EventsIterator;
	class EventsHistory;
	
	using std::string;
	
	typedef uint32_t Queue;

	enum ParamType {
		QueueList,
		IntList,
		DoubleList,
		//Martostring,
		InvalidType
	};

	class FormalParameterValue {
	public: FormalParameterValue(ParamType type, size_t l);
	private:
		ParamType paramType;
		size_t length;
	};

	// collection of FormalParameter
	class FormalParameters : public std::vector<std::pair<string, FormalParameterValue>> {	       
		//void addParam(string name, FormalParameterValue *value);
	};
	
	class FormalConstantList : public FormalParameterValue {
		std::list<double> l;
	};
	
	class FormalDistribution : public FormalParameterValue {
	public:
		FormalDistribution(string idRandom, FormalParameters fp);
	};
	
	class FormalDistributionFixedList : public FormalDistribution {
	public:
		FormalDistributionFixedList(string idRandom, FormalParameters fp);
	};
	
	class FormalDistributionVariadicList : public FormalDistribution {
	public:
		FormalDistributionVariadicList(string idRandom, FormalParameters fp);
	};
	
	class EventType {
	public:
	/* idTr indicates which transition function will be used. 
	idEvt indicates the detailed event.
	fp include all parameters needed to generate the event */
		EventType(string idEvT, double rate, string idTr,
			FormalParameters fp);
	private:
		marto::Transition* transition;
		double rate;
	public:
		FormalParameters fp;
		//GetParameter gp; /* ??? FIXME */
		int nbIntStaticParameters();
		int nbDoubleStaticParameters();
	};

/* list of values for this parameter (for instance : input queues) */ 
	template <typename T>
	class ParameterValues {
		private :
			std::vector<T> values;
		public :
			T get(int index);
	};
	
	/* each simulation sequence only uses 1 object of type Event */
	class Event {
	public:
		/* Create an empty (unusable) event */
		Event(); 
		/* Load the event data from its serialization
		*  returns the number of byte read upon success,
		*  0 if the compact representation 
		*  does not match a known event type */
		size_t load(marto::EventsHistory *h);
		/* Stores a compact representation of the event
		*  returns the size of stored object or 0 upon failure
		*/
		size_t store(void *buffer, size_t buf_size);
		/* Creates (generate) a new Event
		*  returns 1 */
		static int generate(EventType *type);

		/* return the type of the Event */
		EventType *type();
		
		template<typename T>
			ParameterValues<T> *getParameters(string name);
		
		/* Parameters accessors */
		int8_t int8Parameter(int index);
		int16_t int16Parameter(int index);
		int32_t int32Parameter(int index);
		int64_t int64Parameter(int index);
		double doubleParameter(int index);
		int8_t int8Parameter(const string &pname);
		int16_t int16Parameter(const string &pname);
		int32_t int32Parameter(const string &pname);
		int64_t int64Parameter(const string &pname);
		double doubleParameter(const string &pname);

		void set(int index, int8_t value);
		void set(int index, int16_t value);
		void set(int index, int32_t value);
		void set(int index, int64_t value);
		void set(int index, double value);
		
	private:
		std::map<string,void *> parameters;/* actual parameters (not formal), used to apply transition */
		EventType *type_;
	};
	
	class EventsHistory {
	public:
		/** Initialize a new history of events */
		EventsHistory();
		uint32_t nbEvents();
		EventsIterator *iterator(); // returns an iterator positioned at the begining

		/** Add some space in history for nbEvents *previous* events
		 * iterator() can be called to start at the (new) begining of the history
		 */
		void backward(uint32_t nbEvents); 
	
	private:
		Configuration *configuration;
		EventsChunk * firstChunk;// beginning of history
		uint32_t _nbEvents;
	};
	
	class EventsChunk {
		EventsChunk(uint32_t capacity, EventsChunk *prev, EventsChunk *next);
		//void *allocatedMemory;
		void *bufferStart; // beginning of history chunk; same as allocatedMemory ptr in the plain backward scheme
		void *bufferEnd; // end of history chunk;
		uint32_t eventsCapacity; // maximum number of events in this chunk and possible additional chunks
		uint32_t nbEvents; // current number of events in the chunk
		EventsChunk *nextChunk;
		EventsChunk *prevChunk;
		
		//size_t end; // end of buffer
	}
	
	class EventsIterator {
		/* Moving within the history */
		
		/** Fill ev with the next event, loading from the history
		 * Return 0 if no more events are available
		 */
		int loadNextEvent(Event *ev);
		
		/** TODO
		 * use only one of these methods to avoid overwrite
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
			size_t position; // current position in bytes
			uint32_t nbEvents; // numbers of events from the beginning of history
	}
}
#endif
	
#endif
