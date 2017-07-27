#include <marto/event.h>
#include <marto/global.h>
#include <cstdint>

using namespace marto;
using std::ostream;

ostream &operator << (ostream &out, FormalParameters &fp) {
	for (auto it=fp)
}

ostream &operator << (ostream &out, EventType &ev) {
	out << "EventType : " << ev.id << endl;
	out << "-> " << ev->transition << endl;
	out << "-> parameters : " << endl;
	out << ev.fp << endl;
}

int EventType::findIndex(string name) {
	auto couple = fp.find(name);
	return couple[0];//couple is a pair, whose first element is the index in the parameters table
}

ParameterValues *Event::getParameters(string name) {
	if (int index = type.findIndex(name)) {
		return parameters[index];
	}
}

template <typename T>
	T ParameterValues::get(int index) {
		switch (kind) {
			case ARRAY:
				T *array = (T *) values;
				return array[index];
			case GENERATOR:
				if (cache.size() <= index) {
					for (int i=cache.size(); i<index+1; i++)
						cache[i] = g.next();
				}
				return (T) cache[index];
		}
	}
	
size_t ParameterValues::size() {
	switch (kind) {
		case ARRAY:
			return nbValues;
		case GENERATOR:
			return SIZE_MAX;
	}
}

/*	an eventType is stored compactly as an integer 
	(example of event type : arrival in queue 1)
	it is read from a table built from user config file
*/ 
size_t Event::load(EventsHistory *h) {
	void *buffer = h->getCurrentBuffer();
	parameters.clear();/* clears previously stored event information */
	uint32_t *intBuffer = (uint32_t *) buffer;
	auto code = intBuffer[0]; //eventype is encoded in the first integer of the event buffer.
	type_ = h->getConfig()->getEventType(code);
	for (auto pair : type_->fp) {/* pair iterates on all elements in fp (list of pairs)*/ 
		//parameters.insert(pair.first, pair.second.load(intBuffer+1);/* inserts actual parameters computed using the load method of formalParameterValue class */
	}
}



