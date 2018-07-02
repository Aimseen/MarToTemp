#include "std_transitions.h"

//#define DEBUG_REGISTER

#ifdef DEBUG_REGISTER
#include <iostream>
#endif

namespace marto::autoregister::transitions {

/* using a pointer to a vector to avoid to rely on the linker
 * to correctly schedule the various constructors
 */
std::vector<std::pair<std::string, Transition*>> *Registering::transitions = nullptr;

int Registering::autoregister(std::string name, TransitionFactory* factory) {
    if (transitions == nullptr) {
        transitions = new std::vector<std::pair<std::string, Transition*>>;
    }
    transitions->emplace_back(name, factory->CreateTransition());
#ifdef DEBUG_REGISTER
    std::cout << "Registering " << name << " at addresse " << factory << " in " << &transitions << std::endl;
#endif
    return 0;
}

void Registering::initTransitionLibrary(Configuration *config) {
#ifdef DEBUG_REGISTER
    std::cout << "Delegated registration" << " from " << &transitions << std::endl;
#endif
    for (auto & transitionInfo : *transitions) {
        std::string &name = transitionInfo.first;
        marto::Transition *transition = transitionInfo.second;
#ifdef DEBUG_REGISTER
        std::cout << "Really registering " << name << " at addresse " << transition << std::endl;
#endif
        config->registerTransition(name, transition);
    }
    delete transitions;
    transitions=nullptr;
}

extern "C" {
    void initTransitionLibrary(Configuration *config) {
#ifdef DEBUG_REGISTER
        std::cout << "Main registration" << std::endl;
#endif
        Registering::initTransitionLibrary(config);
    }
}
};
