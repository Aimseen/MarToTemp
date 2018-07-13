#include "std_transitions.h"

class_std_transition(ArrivalReject) {
    Point *apply(Point *p, Event *ev) {
        auto *fromList =
            ev->getParameter("from"); // We only get vectors of values
        // as defined in the
        // configuration file
        auto *toList = ev->getParameter("to");

        auto from = fromList->get<queue_id_t>(0); // the only source queue
        auto to = toList->get<queue_id_t>(0); // the only random destination queue

        if (! p->at(from)->isEmpty()) {
            if (! p->at(to)->isFull()) {
                p->at(to)->addClient();
            }
            p->at(from)->removeClient();
        }
        return p;
    }
};
