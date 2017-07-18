/* Event Generator */
#ifndef MARTO_EVENT_GENERATOR_H
#define MARTO_EVENT_GENERATOR_H

#ifdef __cplusplus

#include <marto/transition.h>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <map>

/* Notes relatives au générateur de Lecuyer

Etat courant de PSI3 :
- pas de IncreasedPrec
- pas de SetAntithetic
- pas de AdvanceState

Questions :
- Get/WriteState ne travaille que sur Cg (WriteStateFull, verbeux travaille sur les 3)
- AdvanceState : semble être un déplacement dans le stream, non utilisé par PSI3, utile ?
- ResetStartStream utilisé par psi. Restaure Cg et Bg à partir de Cg, MAIS Get/WriteState ne travaille que sur Cg
- SetSeed : restaure la graine donnée dans Cg, Bg et Ig -> plutôt utiliser ça, non ?
- ResetNextSubStream : apparemment pour passer au substream suivant mais non utilisé dans psi
- Le constructeur semble créer un générateur sur le stream suivant

Objectif :
[ chunk d'evènements contenant dans evènements à nombre variable de paramètres (streams ? substreams ?) ]



*/

namespace marto {

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
		size_t load(void* buffer);
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
		int nbEvents();
		int curID(); /* numbered from 0 to nvEvents-1 */

		/* Fill ev with the current event, loading from the history
		 * Return 0 if no events are available at the current place
		 */
		int curLoad(Event *ev);//indicate to ev where to fetch the event in history
		
		/* Moving within the history */
		
		/** Set current event to the first event in the history */
		void restart();
		/** Fill ev with the next event, loading from the history
		 * Return 0 if no more events are available
		 */
		int nextLoad(Event *ev);
		/** Add some space in history for nbEvents *previous* events
		 * The current position is reset, so pushEvent can be called
		 * immediately.
		 */
		void backward(int nbEvents); 

		/* Add new events */
		void pushEvent(Event *ev); /* throw an error if the next event already exists */
	};
}
#endif
	
#endif
