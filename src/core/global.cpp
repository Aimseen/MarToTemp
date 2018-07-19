#include <ltdl.h>
#include <marto.h>

//#define DEBUG_REGISTER

#ifdef DEBUG_REGISTER
#include <iostream>
#endif

namespace marto {

template <typename T, typename Func, typename TM, typename TMV>
T *Configuration::_register(TM &map, string name, T *value,
                            Func lambdaIfRegister) {
    assert(value != nullptr);
    auto res = map.insert(TMV(name, value));
    if (res.second) {
// the insertion occurs
#ifdef DEBUG_REGISTER
        std::cerr << "Registering " << name << std::endl;
#endif
        lambdaIfRegister();
        return value;
    }
    // the insertion did not occurs, the key already exists
    T *previous = (res.first)->second;
    if (previous == value) {
#ifdef DEBUG_REGISTER
        std::cerr << "Returnning old value for " << name << std::endl;
#endif
        return value;
    }
#ifdef DEBUG_REGISTER
    std::cerr << "Duplicate for " << name << std::endl;
#endif
    throw ExistingName(name);
}

Transition *Configuration::registerTransition(string name, Transition *trans) {
    return _register<Transition>(transitionsMap, name, trans, []() {});
}

QueueConfig *Configuration::registerQueue(string name, QueueConfig *queue) {
    return _register<QueueConfig>(queueConfigsMap, name, queue,
                                  [this, queue]() {
                                      queue_id_t id = queueConfigsVector.size();
                                      queueConfigsVector.push_back(queue);
                                      queue->setId(id);
                                  });
}

EventType *Configuration::registerEventType(EventType *eventType) {
    return _register<EventType>(
        eventTypesMap, eventType->name(), eventType, [this, &eventType]() {
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

QueueConfig *Configuration::getQueueConfig(std::string name) {
    try {
        return queueConfigsMap.at(name);
    } catch (const std::out_of_range &oor) {
        throw UnknownName(name);
    }
}

EventType *Configuration::getEventType(unsigned num) {
    assert(num < eventTypesVector.size());
    return eventTypesVector[num];
}

void Configuration::loadTransitionLibrary(std::string libname,
                                          std::string initCallback) {
    int err = 0;
    static int initialized = 0;
    static lt_dladvise advise;

    if (!initialized) {
        err = lt_dlinit();
        if (err != 0) {
            throw DLOpenError(std::string("lt_dlinit error: ") +
                              std::to_string(err));
        }
        err = lt_dladdsearchdir(PKGLIBDIR);
        if (err != 0) {
            throw DLOpenError(std::string("lt_dladdsearchdir error: ") +
                              lt_dlerror());
        }
        err = lt_dladvise_init(&advise);
        if (err != 0) {
            throw DLOpenError(std::string("lt_dladvise_init error: ") +
                              lt_dlerror());
        }
        err = lt_dladvise_ext(&advise);
        if (err != 0) {
            throw DLOpenError(std::string("lt_dladvise_ext error: ") +
                              lt_dlerror());
        }
        err = lt_dladvise_local(&advise);
        if (err != 0) {
            throw DLOpenError(std::string("lt_dladvise_local error: ") +
                              lt_dlerror());
        }
        initialized = 1;
    }
    lt_dlhandle handle = 0;
    handle = lt_dlopenadvise(libname.c_str(), advise);
    // lt_dladvise_destroy(&advise);

    if (handle == 0) {
        throw DLOpenError(std::string("Cannot load ") + libname);
    }

    transitionInitCallback_t *initaddr =
        (transitionInitCallback_t *)lt_dlsym(handle, initCallback.c_str());

    if (initaddr == nullptr) {
        throw DLOpenError(std::string("Cannot find ") + initCallback + " in " +
                          libname);
    }
    (*initaddr)(this);
}
}
