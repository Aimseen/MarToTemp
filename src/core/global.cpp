#include <marto.h>

//#include <iostream>

namespace marto {

Configuration *Global::config = nullptr;

Transition *Configuration::registerTransition(string name, Transition *trans) {
    auto it = transitionsMap.find(name);
    if (marto_likely(it == transitionsMap.end())) {
        auto element = new std::pair<string, Transition *>(name, trans);
        transitionsMap.insert(*element);
        //std::cerr << "Returnning value for " << name << std::endl;
        return trans;
    }
    Transition *previous=it->second;
    if (previous == trans) {
        //std::cerr << "Returnning old value for " << name << std::endl;
        return trans;
    }
    //std::cerr << "Returnning NULL for " << name << std::endl;
    return nullptr;
}

EventType* Configuration::registerEventType(EventType* eventType) {
    Event::code_t code=eventTypesVector.size();
    eventTypesVector.push_back(eventType);
    eventType->setCode(code);
    return eventType;
}

Transition *Configuration::getTransition(string name) {
    try {
        return transitionsMap.at(name);
    } catch (const std::out_of_range& oor) {
        throw UnknownTransition(name);
    }
}

EventType *Configuration::getEventType(unsigned num) {
    assert(num < eventTypesVector.size());
    return eventTypesVector[num];
}

Configuration *Global::getConfig() {
    if (config == nullptr) {
        config = new Configuration();

        // TODO but rather on the libtransition side
        // Fill the hardcoded transition names
        // setTransition("JSQ2", new JSQ2());
    }
    return config;
}

}
