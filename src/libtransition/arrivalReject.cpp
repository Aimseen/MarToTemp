#include "std_transitions.h"

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
