#include <cstdint>
#include <iostream>
#include <marto.h>
#include <marto/event.h>

#include <cassert>

using std::ostream;

ostream &operator<<(ostream &out, const marto::FormalParameters &fp) {
    // FIXME:
    for (auto it = fp;;) {
        out << it << std::endl;
    }
    return out;
}

ostream &operator<<(ostream &out, const marto::EventType &ev) {
    out << "EventType : " << ev.name << std::endl;
    out << "-> " << ev.transition << std::endl;
    out << "-> parameters : " << std::endl;
    out << ev.parameters << std::endl;
    return out;
}

namespace marto {

EventType::EventType(Configuration *config, string idEvT, double evtRate,
                     string idTr, FormalParameters *fp)
    : name(idEvT), transition(config->getTransition(idTr)), rate(evtRate),
      parameters(fp) {
    config->registerEventType(this);
}

int EventType::findIndex(string parameterName) {
    auto couple = parameters->find(parameterName);
    assert(couple != parameters->end());
    return couple->second.first; // couple is a pair, whose first element is the
                                 // index in the parameters table
}

event_access_t EventType::load(EventsIStream &istream, Event *ev,
                               EventsHistory *hist) {
    // FIXME: avoir unused parameter;
    if (!istream) {
        istream >> ev >> hist;
    }
    for (auto pair : *parameters) {
        /* pair iterates on all elements in fp (list of pairs) */
        // parameters.insert(pair.first, pair.second.load(intBuffer+1);/*
        // inserts actual parameters computed using the load method of
        // formalParameterValue class */
    }
    return EVENT_LOADED;
}

event_access_t EventType::store(EventsOStream &ostream, Event *ev,
                                EventsHistory *hist) {
    // FIXME: avoir unused parameter;
    if (!ostream) {
        ostream << ev << hist;
    }
    for (auto pair : *parameters) {
        /* pair iterates on all elements in fp (list of pairs) */
        // parameters.insert(pair.first, pair.second.load(intBuffer+1);/*
        // inserts actual parameters computed using the load method of
        // formalParameterValue class */
    }
    return EVENT_STORED;
}

ParameterValues *Event::getParameter(string name) {
    if (int index = type()->findIndex(name)) {
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
    return 0;    // for compiler
}

Event::Event() : parameters(), status(EVENT_STATUS_INVALID) {}

Event::Event(EventType *t) : Event() { setType(t); }

void Event::apply(Point *p) { type()->transition->apply(p, this); }
}
