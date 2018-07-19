#include "std_transitions.h"

class_std_transition(JSQ2) {
    default_transition_constructors;
    Point *apply(
        Point * p,
        Event * ev) { // Event ev contains transition specification (parameters)
        // fromList is a random sequence of Queues (specified in ev) to prevent
        // access to random generation and protect monotonicity
        // marto::ParameterValues<marto::Queue>
        auto *fromList =
            ev->getParameter("from"); // We only get vectors of values
        // as defined in the
        // configuration file
        auto *toList = ev->getParameter("to");

        auto from = fromList->get<queue_id_t>(0); // the only source queue
        auto to0 =
            toList->get<queue_id_t>(0); // the first random destination queue
        auto to1 =
            toList->get<queue_id_t>(1); // second random destination queue

        if (!p->at(from)->isEmpty()) {
            if (p->at(to0)->compareTo(p->at(to1)) > 0)
                p->at(to0)->addClient();
            else
                p->at(to1)->addClient();
            p->at(from)->removeClient();
        }
        return p;
    }
};
