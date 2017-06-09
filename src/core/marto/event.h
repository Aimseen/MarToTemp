/* Event Generator */
#ifndef MARTO_EVENT_GENERATOR_H
#define MARTO_EVENT_GENERATOR_H

#ifdef __cplusplus

#include <marto/transition.h>

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

	enum Type {
		MartoQueueList,
		MartoIntList,
		MartoDoubleList,
		//MartoString,
	};

	class FormalParameterValue {
	public: FormalParameterValue(Type type, size_t l);
	private:
		Type paramType;
		size_t length;
	}

	class FormalConstantList : public FormalParameterValue {
		List of double
	}
	
	class FormalDistribution : public FormalParameterValue {
	public:
		FormalDistribution(String idRandom,
				   FormalParameters fp);
	}
	
	class FormalDistributionFixedList : public FormalDistribution {
	public:
		FormalDistributionFixedList(String idRandom,
				   FormalParameters fp);
	}
	
	class FormalDistributionVariadicList : public FormalDistribution {
	public:
		FormalDistributionVariadicList(String idRandom,
				   FormalParameters fp);
	}

	// collection of FormalParameter
	class FormalParameters {	       
		addParam(String name, FormalParameterValue *value);
	};
	
	class EventType {
	public:
		EventType(String idEvT, double rate, String idTr,
			FormalParameters fp);
	private:
		marto::Transition* transition;
		double rate;
	public:
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
		int generate(EventType *type);

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
		/** Initialize a new history of events */
		EventsHistory();
		int nbEvents();
		int curID(); /* numbered from 0 to nvEvents-1 */

		/* Fill ev with the current event, loading from the history
		 * Return 0 if no events are available at the current place
		 */
		int curLoad(Event *ev);
		
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
	}
}
#endif
	
#endif
