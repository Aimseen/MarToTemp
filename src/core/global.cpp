#include <marto/global.h>

namespace marto {

Configuration *Global::config = nullptr;

void Configuration::setTransition(string name, Transition *trans) {
    auto element = new std::pair<string, Transition *>(name, trans);
    transitionsVector.insert(*element);
}

Transition *Configuration::getTransition(string name) {
    return transitionsVector.at(name);
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

EventType *Configuration::getEventType(unsigned num) {
    assert(num < eventTypesVector.size());
    return eventTypesVector[num];
}

}
