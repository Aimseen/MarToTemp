#include <cstdint>
#include <iostream>
#include <marto.h>
#include <marto/event.h>

#include <cassert>

using std::ostream;

ostream &operator<<(ostream &out, const marto::EventType &ev) {
    out << "EventType : " << ev.name() << std::endl;
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
                     string trName)
    : WithConfiguration(config), _name(idEvT),
      transition(config->getTransition(trName)), rate(evtRate) {
    config->registerEventType(this);
    // FIXME: handle an internal state so that parameters cannot be modified
    // once an EventType was used by an Event
}

bool EventType::registerParameter(string name, FormalParameterValues *fp) {
    // FIXME: see previous comment
    auto res = formalParametersNames.insert(
        std::make_pair(name, formalParameters.size()));
    if (res.second) {
        formalParameters.push_back(fp);
    } else {
        throw ExistingName(name);
    }
    return res.second;
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

history_access_t EventType::load(HistoryIStream &istream, Event *ev,
                                 EventsHistory *hist) {
    assert(ev->status == Event::EVENT_STATUS_TYPED);
    assert(ev->_type == this);
    auto fpit = formalParameters.begin();
    auto pit = ev->parameters.begin();
    for (; fpit != formalParameters.end(); fpit++, pit++) {
        auto res = (*fpit)->load(istream, *pit);
        if (res != HISTORY_DATA_LOADED) {
            return res;
        }
    }
    ev->status = Event::EVENT_STATUS_FILLED;
    return HISTORY_DATA_LOADED;
}

history_access_t EventType::store(HistoryOStream &ostream, Event *ev,
                                  EventsHistory *hist) {
    auto fpit = formalParameters.begin();
    auto pit = ev->parameters.begin();
    for (; fpit != formalParameters.end(); fpit++, pit++) {
        (*fpit)->store(ostream, *pit);
    }
    return HISTORY_DATA_STORED;
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
} // namespace marto
