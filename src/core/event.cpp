#include <marto/event.h>
#include <marto/global.h>
#include <marto/eventsHistory.h>
#include <cstdint>

#include <assert.h>

using namespace marto;

int EventType::findIndex(string name) {
    auto couple = fp.find(name);
    assert(couple != fp.end());
    return couple->second.first;        //couple is a pair, whose first element is the index in the parameters table
}

ParameterValues *Event::getParameter(string name) {
    if (int index = type->findIndex(name)) {
        return parameters[index];
    }
}

template < typename T > T ParameterValues::get(int index) {
    switch (kind) {
    case ARRAY:
        T * array = (T *) u.values;
        return array[index];
    case GENERATOR:
        if (u.s.cache.size() <= index) {
            for (int i = u.s.cache.size(); i < index + 1; i++) {
                //u.s.cache[i] = g.nextU01();
            }
        }
        return (T) u.s.cache[index];
    }
}

size_t ParameterValues::size() {
    switch (kind) {
    case ARRAY:
        return u.nbValues;
    case GENERATOR:
        return SIZE_MAX;
    }
}

/*	an eventType is stored compactly as an integer 
	(example of event type : arrival in queue 1)
	it is read from a table built from user config file
*/
size_t Event::load(EventsIterator * hit) {
    // TODO : store iterator ? (Vince)
    void *buffer = hit->getCurrentBuffer();
    parameters.clear();         /* clears previously stored event information */
    uint32_t *intBuffer = (uint32_t *) buffer;
    auto code = intBuffer[0];   //eventype is encoded in the first integer of the event buffer.
    type = hit->history()->getConfig()->getEventType(code);
  for (auto pair:type->fp) {   /* pair iterates on all elements in fp (list of pairs) */
        //parameters.insert(pair.first, pair.second.load(intBuffer+1);/* inserts actual parameters computed using the load method of formalParameterValue class */
    }
}
