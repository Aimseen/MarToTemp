#include "std_transitions.h"

class_std_transition(ArrivalReject) {
    default_transition_constructors;
    Point *apply(Point * p, Event * ev) {
        // We only get vectors of values as defined in the
        // configuration file
        auto *toList = ev->getParameter("to");

        auto to =
            toList->get<queue_id_t>(0); // the only random destination queue

        if (!p->at(to)->isFull()) {
            p->at(to)->addClient();
        }
        return p;
    }
};
