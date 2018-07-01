/* -*-c++-*- C++ mode for emacs */
/* header for adding transitions in the default MarTo transition library */
#ifndef MARTO_STD_TRANSITIONS_H
#define MARTO_STD_TRANSITIONS_H

#include <marto.h>
#include <string>
#include <list>

namespace marto::autoregister::transitions {
class TransitionFactory {
public:
    virtual Transition* CreateTransition() = 0;
};
template <class TransitionClass>
class TransitionFactoryImpl : public TransitionFactory {
public:
    virtual Transition* CreateTransition() { return new TransitionClass ; }
};
class Registering {
private:
public:
    static std::vector<std::pair<std::string, marto::Transition*>> *transitions;
    static int autoregister(std::string name, TransitionFactory* factory);
    static void initTransitionLibrary(Configuration *config);
};
class Registered {};

};

#define class_transition(name, ns)                                      \
    namespace ns {                                                      \
class name;                                                             \
    };                                                                  \
    namespace marto::autoregister::transitions::instances {             \
class name : public ::marto::autoregister::transitions::Registered {    \
    static int dummy;                                                   \
};                                                                      \
    };                                                                  \
    int marto::autoregister::transitions::instances::name::dummy \
    = marto::autoregister::transitions::Registering::autoregister( \
        #name, new   marto::autoregister::transitions::TransitionFactoryImpl<ns::name>() \
        );                                                              \
    class ns::name : public marto::Transition

#define class_std_transition(name)              \
    class_transition(name, marto::stdlib)

#endif
