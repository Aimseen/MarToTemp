/* -*-c++-*- C++ mode for emacs */
/* header for adding transitions in the default MarTo transition library */
#ifndef MARTO_STD_TRANSITIONS_H
#define MARTO_STD_TRANSITIONS_H

#include <marto.h>
#include <string>
#include <list>

namespace marto::autoregister::transitions {
class TransitionFactory {
protected:
    std::string name;
public:
    TransitionFactory(std::string n) : name(n) {}
    virtual Transition* createTransition(Configuration *config) = 0;
};
template <class TransitionClass>
class TransitionFactoryImpl : public TransitionFactory {
public:
    TransitionFactoryImpl(std::string n) : TransitionFactory(n) {}
    virtual Transition* createTransition(Configuration *config) {
        return new TransitionClass(config, name) ;
    }
};
class Registering {
private:
public:
    static std::vector<marto::autoregister::transitions::TransitionFactory*> *transitionFactories;
    static int autoregister(TransitionFactory* factory);
    static void initTransitionLibrary(Configuration *config);
};
class Registered {};

};

#define class_transition(name, ns)                                      \
    namespace ns {                                                      \
    class name;                                                         \
    };                                                                  \
    namespace marto::autoregister::transitions::instances {             \
    class name : public ::marto::autoregister::transitions::Registered {\
        static int dummy;                                               \
    };                                                                  \
    };                                                                  \
    int marto::autoregister::transitions::instances::name::dummy        \
    = marto::autoregister::transitions::Registering::autoregister(      \
        new   marto::autoregister::transitions::TransitionFactoryImpl<ns::name>(#name) \
        );                                                              \
    class ns::name : public marto::Transition

#define class_std_transition(name)              \
    class_transition(name, marto::stdlib)

#endif
