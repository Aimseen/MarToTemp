#include <marto.h>

//#include <iostream>

namespace marto {

template<typename T,
         typename Func,
         typename TM=std::map<std::string, T *>,
         typename TMV=typename std::map<std::string, T *>::value_type>
T* Configuration::_register(TM &map, string name, T* value,
                            Func lambdaIfRegister) {
    assert(value != nullptr);
    auto res = map.insert(TMV(name, value));
    if (res.second) {
        // the insertion occurs
        lambdaIfRegister();
        return value;
    }
    // the insertion did not occurs, the key already exists
    T *previous = (res.first)->second;
    if (previous == value) {
        // std::cerr << "Returnning old value for " << name << std::endl;
        return value;
    }
    throw ExistingName(name);
}

Transition *Configuration::registerTransition(string name, Transition *trans) {
    return _register<Transition>(transitionsMap, name, trans,
                                 [](){});
}

EventType *Configuration::registerEventType(EventType *eventType) {
    return _register<EventType>(eventTypesMap, eventType->name(), eventType,
                                [this, &eventType](){
                                    Event::code_t code = eventTypesVector.size();
                                    eventTypesVector.push_back(eventType);
                                    eventType->setCode(code);
                                });
}

Transition *Configuration::getTransition(string name) {
    try {
        return transitionsMap.at(name);
    } catch (const std::out_of_range &oor) {
        throw UnknownName(name);
    }
}

EventType *Configuration::getEventType(unsigned num) {
    assert(num < eventTypesVector.size());
    return eventTypesVector[num];
}
}
