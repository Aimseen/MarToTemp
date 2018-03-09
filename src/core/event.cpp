#include <cstdint>
#include <iostream>
#include <marto.h>
#include <marto/event.h>

#include <cassert>

using std::ostream;

ostream &operator << (ostream &out, const marto::EventType &ev) {
    out << "EventType : " << ev.name << std::endl;
    out << "-> " << ev.transition << std::endl;
    out << "-> parameters : " << std::endl;
    for (auto it=ev.formalParameters.begin(); it != ev.formalParameters.end(); it++) {
        // TODO : missing << for FormalParameterValue
        out << "Un parametre trouvé " << std::endl;
    }
    return out;
}

namespace marto {

EventType::EventType(Configuration *config, string idEvT, double evtRate, string idTr):
    name(idEvT), transition(config->getTransition(idTr)), rate(evtRate)
{
    // FIXME : complete (formal parameters for instance)
    config->registerEventType(this);
}

int EventType::findIndex(string parameterName) {
    auto couple = formalParametersNames.find(parameterName);
    assert(couple != formalParametersNames.end());
    return couple->second;   //couple is a pair, whose first element is the index in the parameters table
}

event_access_t EventType::load(EventsIStream &istream, Event *ev,
                               EventsHistory *hist) {
    // FIXME: avoir unused parameter;
    if (!istream) {
        istream >> ev >> hist;
    }
    for (auto pair:formalParameters) {
        /* pair iterates on all elements in fp (list of pairs) */
        //parameters.insert(pair.first, pair.second.load(intBuffer+1);/* inserts actual parameters computed using the load method of formalParameterValue class */
        (void) pair;
    }
    return EVENT_LOADED;
}

event_access_t EventType::store(EventsOStream &ostream, Event *ev,
                                EventsHistory *hist) {
    // FIXME: avoir unused parameter;
    if (!ostream) {
        ostream << ev << hist;
    }
    for (auto pair:formalParameters) {
        /* pair iterates on all elements in fp (list of pairs) */
        //parameters.insert(pair.first, pair.second.load(intBuffer+1);/* inserts actual parameters computed using the load method of formalParameterValue class */
        (void) pair;
    }
    return EVENT_STORED;
}

ParameterValues *Event::getParameter(string name) {
    if (int index = type()->findIndex(name)) {
        return parameters[index];
    }
    return nullptr;
}

ParameterValues::ParameterValues() {
    kind = UNDEFINED;
    bufferSize = 0;
    buffer = nullptr;
    nbValues = 0;
    reference = nullptr;
}

size_t ParameterValues::size() {
    // TODO : keep nbValues coherent for all kinds
    return nbValues;
}

FormalConstantList::FormalConstantList(ParamType type, size_t s) : FormalParameterValue(type, s) {
    switch (type) {
        case IntList:
            // TODO : temporary, for testing only
            values = new ParameterValues();
        default:
            marto_BUG();
    }
};

Event::Event():
    parameters(), status(EVENT_STATUS_INVALID)
{
}

Event::Event(EventType *t) : Event() { setType(t); }

void Event::apply(Point *p) { type()->transition->apply(p, this); }
}
