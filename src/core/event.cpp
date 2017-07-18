#include <marto/event.h>
#include <marto/global.h>

using namespace marto;

template <typename T>
ParameterValues<T> *Event::getParameters(string name) {
	if (auto it = parameters.find(name)) {
		return dynamic_cast<ParameterValues<T>>(*it);
	}
}

/*	an eventType is stored compactly as an integer 
	(example of event type : arrival in queue 1)
	it is read from a table built from user config file
*/ 
size_t Event::load(void *buffer) {
	parameters.clear();/* clears previously stored event information */
	uint32_t *intBuffer = (uint32_t *) buffer;
	auto code = intBuffer[0]; //eventype is encoded in the first integer of the event buffer.
	type_ = Global::getConfig()->getEventType(code);
	for (auto pair : type_->fp) {/* pair iterates on all elements in fp (list of pairs)*/ 
		//parameters.insert(pair.first, pair.second.load(intBuffer+1);/* inserts actual parameters computed using the load method of formalParameterValue class */
	}
}

int EventsHistory::curLoad(Event *ev) {
	//ev->load()
	return 0;
}