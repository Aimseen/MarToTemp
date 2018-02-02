#include <marto/event.h>
#include <marto/global.h>
#include <marto/eventsHistory.h>
#include <cstdint>
#include <iostream>

#include <assert.h>

using std::ostream;

ostream &operator << (ostream &out, const marto::FormalParameters &fp) {
    // FIXME:
    for (auto it=fp;;) {
        out << it << std::endl;
    }
}

ostream &operator << (ostream &out, const marto::EventType &ev) {
    out << "EventType : " << ev.name << std::endl;
    out << "-> " << ev.transition << std::endl;
    out << "-> parameters : " << std::endl;
    out << ev.parameters << std::endl;
}

namespace marto {

EventType::EventType(string idEvT, double evtRate, string idTr, FormalParameters params) {
    name = idEvT;
    rate = evtRate;
    transition = Global::getConfig()->getTransition(idTr);
    parameters = params;
}

int EventType::findIndex(string name) {
    auto couple = parameters.find(name);
    assert(couple != parameters.end());
    return couple->second.first;   //couple is a pair, whose first element is the index in the parameters table
}

ParameterValues *Event::getParameter(string name) {
    if (int index = type->findIndex(name)) {
        return parameters[index];
    }
}

template <typename T>
T ParameterValues::get(int index) {
    switch (kind) {
    case ARRAY:
        T *array = (T *) u.array.values;
        return array[index];
    case GENERATOR:
        if (u.generator.cache.size() <= index) {
            for (int i=u.generator.cache.size(); i<index+1; i++)
                u.generator.cache[i] = u.generator.g.next();
        }
        return (T) u.generator.cache[index];
    }
}

size_t ParameterValues::size() {
    switch (kind) {
    case ARRAY:
        return u.array.nbValues;
    case GENERATOR:
        return SIZE_MAX;
    }
}

Event::Event(EventType *t) {
    type = t;
}

void Event::apply(Point *p) {
    type->transition->apply(p, this);
}

/*  an eventType is stored compactly as an integer
   (example of event type : arrival in queue 1)
   it is read from a table built from user config file
 */
size_t Event::load(EventsIterator * hit) {
    // TODO : store iterator ? (Vince)
    void *buffer = hit->getCurrentBuffer();
    parameters.clear();     /* clears previously stored event information */
    uint32_t *intBuffer = (uint32_t *) buffer;
    auto code = intBuffer[0];       //eventype is encoded in the first integer of the event buffer.
    type = hit->history()->getConfig()->getEventType(code);
    for (auto pair:type->parameters) {
        /* pair iterates on all elements in fp (list of pairs) */
        //parameters.insert(pair.first, pair.second.load(intBuffer+1);/* inserts actual parameters computed using the load method of formalParameterValue class */
    }
}
    
size_t Event::size() {
    // FIXME
    return 0;
}

}
