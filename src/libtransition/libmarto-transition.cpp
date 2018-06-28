#include <iostream>
#include <marto.h>
#include <string>

namespace marto {
class ArrivalReject : public Transition {
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

class JSQ2 : public Transition {
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

void initTransitionLibrary(Configuration *config) {
    ArrivalReject *arrivalReject = new ArrivalReject();
    JSQ2 *jsq2 = new JSQ2();
    config->registerTransition("JSQ2", jsq2);
    config->registerTransition("ArrivalReject", arrivalReject);
}
};
