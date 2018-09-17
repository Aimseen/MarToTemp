#include "std_transitions.h"

/** Departure of a single client
 *  from a single queue
 *  Client can be dropped or equivalently sent to "outside queue"
 */
class_std_transition(Departure) {
    default_transition_constructors;
    Point *apply(Point * p, Event * ev) {
        auto *fromList = ev->getParameter("from");
        auto from = fromList->get<queue_id_t>(0); // the only (?) source queue

        if (!p->at(from)->isEmpty()) {
            p->at(from)->removeClient();
        }
        return p;
    }
};
