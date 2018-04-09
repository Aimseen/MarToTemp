#include <cstdint>
#include <iostream>
#include <marto.h>
#include <marto/event.h>

#include <cassert>

using std::ostream;

ostream &operator<<(ostream &out, const marto::EventType &ev) {
    out << "EventType : " << ev.name << std::endl;
    out << "-> " << ev.transition << std::endl;
    out << "-> parameters : " << std::endl;
    for (auto it = ev.formalParameters.begin(); it != ev.formalParameters.end();
         it++) {
        // TODO : missing << for FormalParameterValue
        out << "Un parametre trouvé " << std::endl;
    }
    return out;
}

namespace marto {

EventType::EventType(Configuration *config, string idEvT, double evtRate,
                     string idTr)
    : name(idEvT), transition(config->getTransition(idTr)), rate(evtRate) {
    // FIXME : complete (formal parameters for instance)
    config->registerEventType(this);
}

void EventType::registerParameter(string name, FormalParameterValues *fp) {
    // TODO : check for duplicate name, ...
    formalParametersNames.insert(std::make_pair(name, formalParameters.size()));
    formalParameters.push_back(fp);
}

int EventType::findIndex(string parameterName) {
    auto couple = formalParametersNames.find(parameterName);
    if (couple == formalParametersNames.end()) {
        return -1;
    }
    // couple is a pair, whose second element is the index in the parameters
    // table
    return (couple->second);
}

event_access_t EventType::load(EventsIStream &istream, Event *ev,
                               EventsHistory *hist) {
    auto fpit = formalParameters.begin();
    auto pit = ev->parameters.begin();
    for (; fpit != formalParameters.end(); fpit++, pit++) {
        (*fpit)->load(istream, *pit);
    }
    return EVENT_LOADED;
}

event_access_t EventType::store(EventsOStream &ostream, Event *ev,
                                EventsHistory *hist) {
    auto fpit = formalParameters.begin();
    auto pit = ev->parameters.begin();
    for (; fpit != formalParameters.end(); fpit++, pit++) {
        (*fpit)->store(ostream, *pit);
    }
    return EVENT_STORED;
}

Event::Event() : parameters(), status(EVENT_STATUS_INVALID) {}

void Event::generate(EventType *t, Random *g) {
    setType(t);
    size_t i = 0;
    for (auto fp : t->formalParameters) {
        fp->generate(parameters[i], g);
        i++;
    }
    status = EVENT_STATUS_FILLED;
}

ParameterValues *Event::getParameter(string name) {
    int index = type()->findIndex(name);
    if (index >= 0) {
        return parameters[index];
    }
    return nullptr;
}

void Event::apply(Point *p) { type()->transition->apply(p, this); }
}
