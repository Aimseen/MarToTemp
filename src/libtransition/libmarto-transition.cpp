#include <iostream>
#include <marto.h>
#include <string>
#include <list>

namespace marto {

namespace autoregister {
namespace transitions {
class TransitionFactory {
public:
    virtual marto::Transition* CreateTransition() = 0;
};

template <class TransitionClass>
class TransitionFactoryImpl : public TransitionFactory {
public:
    virtual marto::Transition* CreateTransition() { return new TransitionClass ; }
};
class Registering {
public:
    static std::vector<std::pair<std::string, marto::Transition*>> transitions;
    static int autoregister(std::string name, TransitionFactory* factory) {
        auto transitionInfo=std::make_pair(name, factory->CreateTransition());
        transitions.push_back(transitionInfo);
        //std::cout << "Registering " << name << " at addresse " << factory << std::endl;
        return 0;
    }
    static void initTransitionLibrary(Configuration *config) {
        for (auto & transitionInfo : transitions) {
            std::string &name = transitionInfo.first;
            marto::Transition *transition = transitionInfo.second;
            //std::cout << "Really registering " << name << " at addresse " << transition << std::endl;
            config->registerTransition(name, transition);
        }
    }
};
std::vector<std::pair<std::string, Transition*>> Registering::transitions;
class Registered {};
};
extern "C" {
    void initTransitionLibrary(Configuration *config) {
        marto::autoregister::transitions::Registering::initTransitionLibrary(config);
}
}
};
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

class_std_transition(ArrivalReject) {
    Point *apply(Point *p, Event *ev) {
        auto *fromList =
            ev->getParameter("from"); // We only get vectors of values
        // as defined in the
        // configuration file
        auto *toList = ev->getParameter("to");

        auto from = fromList->get<Queue>(0); // the only source queue
        auto to = toList->get<Queue>(0); // the only random destination queue

        if (p->at(from) > 0) {
            if (p->at(to) < config->getCapacity(to)) {
                p->at(to)++;
            }
            p->at(from)--;
        }
        return p;
    }
};

class_std_transition(JSQ2) {
    Point *apply(
        Point *p,
        Event *ev) { // Event ev contains transition specification (parameters)
        // fromList is a random sequence of Queues (specified in ev) to prevent
        // access to random generation and protect monotonicity
        // marto::ParameterValues<marto::Queue>
        auto *fromList =
            ev->getParameter("from"); // We only get vectors of values
        // as defined in the
        // configuration file
        auto *toList = ev->getParameter("to");

        auto from = fromList->get<Queue>(0); // the only source queue
        auto to0 = toList->get<Queue>(0); // the first random destination queue
        auto to1 = toList->get<Queue>(1); // second random destination queue

        if (p->at(from) > 0) {
            if (p->at(to0) < p->at(to1))
                p->at(to0)++;
            else
                p->at(to1)++;
            p->at(from)--;
        }
        return p;
    }
};
