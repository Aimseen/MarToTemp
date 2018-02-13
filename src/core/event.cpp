#include <marto/event.h>
#include <marto.h>
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

EventType::EventType(Configuration *config, string idEvT, double evtRate, string idTr, FormalParameters *fp):
    name(idEvT), transition(config->getTransition(idTr)), rate(evtRate), parameters(fp)
{
    config->registerEventType(this);
}

int EventType::findIndex(string parameterName) {
    auto couple = parameters->find(parameterName);
    assert(couple != parameters->end());
    return couple->second.first;   //couple is a pair, whose first element is the index in the parameters table
}

event_access_t EventType::load(EventsIStream &istream, Event *ev, EventsHistory * hist) {
    // FIXME: avoir unused parameter;
    if (!istream) {
        istream >> ev >> hist;
    }
    for (auto pair:*parameters) {
        /* pair iterates on all elements in fp (list of pairs) */
        //parameters.insert(pair.first, pair.second.load(intBuffer+1);/* inserts actual parameters computed using the load method of formalParameterValue class */
    }
    return EVENT_LOADED;
}

event_access_t EventType::store(EventsOStream &ostream, Event *ev, EventsHistory * hist) {
    // FIXME: avoir unused parameter;
    if (!ostream) {
        ostream << ev << hist;
    }
    for (auto pair:*parameters) {
        /* pair iterates on all elements in fp (list of pairs) */
        //parameters.insert(pair.first, pair.second.load(intBuffer+1);/* inserts actual parameters computed using the load method of formalParameterValue class */
    }
    return EVENT_STORED;
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

Event::Event():
    parameters(), status(EVENT_STATUS_INVALID)
{
}

Event::Event(EventType *t):
    Event()
{
    setTypeAndCode(t);
}

void Event::apply(Point *p) {
    type->transition->apply(p, this);
}

/*  an eventType is stored compactly as an integer
   (example of event type : arrival in queue 1)
   it is read from a table built from user config file
 */
event_access_t Event::load(EventsIStream &istream, EventsHistory * hist) {
    parameters.clear();     /* clears previously stored event information */
    if (istream >> code) {   //eventype is encoded in the first integer of the event buffer.
        if (setTypeFromCode(hist->getConfig())) {
            if (type->load(istream, this, hist)) {
                status = EVENT_STATUS_FILLED;
                return EVENT_LOADED;
            }
        }
    }
    return EVENT_LOAD_CODE_ERROR;
}

event_access_t Event::store(EventsOStream &ostream, EventsHistory * hist) {
    if (status != EVENT_STATUS_FILLED) {
        return EVENT_STORE_UNDEFINED_ERROR;
    }
    if (ostream << code) {
        return type->store(ostream, this, hist);
    }
    return EVENT_STORE_ERROR;
}   
}
