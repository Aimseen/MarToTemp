#include "std_transitions.h"

//#define DEBUG_REGISTER

#ifdef DEBUG_REGISTER
#include <iostream>
#endif

namespace marto::autoregister::transitions {

/* using a pointer to a vector to avoid to rely on the linker
 * to correctly schedule the various constructors
 */
std::vector<TransitionFactory *> *Registering::transitionFactories = nullptr;

int Registering::autoregister(TransitionFactory *factory) {
    if (transitionFactories == nullptr) {
        transitionFactories = new std::vector<TransitionFactory *>;
    }
    transitionFactories->emplace_back(factory);
#ifdef DEBUG_REGISTER
    std::cerr << "Registering factory at addresse " << factory << " in "
              << (&transitionFactories) << std::endl;
#endif
    return 0;
}

void Registering::initTransitionLibrary(Configuration *config) {
#ifdef DEBUG_REGISTER
    std::cerr << "Delegated registration"
              << " from " << (&transitionFactories) << std::endl;
#endif
    for (auto factory : *transitionFactories) {
        marto::Transition *transition = factory->createTransition(config);
#ifdef DEBUG_REGISTER
        std::cerr << "Really registering " << transition->name()
                  << " at addresse " << transition << std::endl;
#endif
        assert(transition != nullptr);
    }
}

extern "C" {
void initTransitionLibrary(Configuration *config) {
#ifdef DEBUG_REGISTER
    std::cerr << "Main registration" << std::endl;
#endif
    Registering::initTransitionLibrary(config);
}
}
}; // namespace marto::autoregister::transitions
