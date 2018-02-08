#include <marto/event.h>
#include <marto/global.h>
#include <marto/eventsHistory.h>
#include <cstdint>
#include <iostream>

#include <cassert>

using std::ostream;

ostream &operator << (ostream &out, const marto::FormalParameters &fp) {
    // FIXME:
    for (auto it=fp;;) {
        out << it << std::endl;
    }
    return out;
}

ostream &operator << (ostream &out, const marto::EventType &ev) {
    out << "EventType : " << ev.name << std::endl;
    out << "-> " << ev.transition << std::endl;
    out << "-> parameters : " << std::endl;
    out << ev.parameters << std::endl;
    return out;
}

namespace marto {

EventType::EventType(string idEvT, double evtRate, string idTr, FormalParameters *params) {
    name = idEvT;
    rate = evtRate;
    transition = Global::getConfig()->getTransition(idTr);
    parameters = params;
}

int EventType::findIndex(string name) {
    auto couple = parameters->find(name);
    assert(couple != parameters->end());
    return couple->second.first;   //couple is a pair, whose first element is the index in the parameters table
}

ParameterValues *Event::getParameter(string name) {
    if (int index = type->findIndex(name)) {
        return parameters[index];
    }
    return nullptr;
}

size_t ParameterValues::size() {
    switch (kind) {
    case ARRAY:
        return u.array.nbValues;
    case GENERATOR:
        return SIZE_MAX;
    case REFERENCE:
        marto_BUG(); // TODO: handle this case
    }
    marto_BUG(); // Never reached
    return 0; // for compiler
}

Event::Event() {
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
int Event::load(EventsHistory * hist, EventsIStream &istream) {
    parameters.clear();     /* clears previously stored event information */
    int code;
    // FIXME: next line is just to show
    istream >> code >> code;   //eventype is encoded in the first integer of the event buffer.
    type = hist->getConfig()->getEventType(code);
    for (auto pair:*(type->parameters)) {
        /* pair iterates on all elements in fp (list of pairs) */
        //parameters.insert(pair.first, pair.second.load(intBuffer+1);/* inserts actual parameters computed using the load method of formalParameterValue class */
    }
    return true;
}

int Event::store(EventsHistory * hist, EventsOStream &ostream) {
    int code=12;
    // FIXME: next line is just to show
    ostream << code << code;  //eventype is encoded in the first integer of the event buffer.
    type = hist->getConfig()->getEventType(code);
    for (auto pair:*(type->parameters)) {
        /* pair iterates on all elements in fp (list of pairs) */
        //parameters.insert(pair.first, pair.second.load(intBuffer+1);/* inserts actual parameters computed using the load method of formalParameterValue class */
    }
    return true;
}

}
