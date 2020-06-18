#include "std_transitions.h"

/**
 * \brief      Routing n queues from origin queue to destination queues
 * \details    Remove 1 packet from each origin queue and send to destination
 *             queues. Reject if destination queues are full.
 */

class_std_transition(routingNQueuesReject) {
  default_transition_constructors;
  Point *apply(Point * p, Event * ev) {
    auto *fromList = ev->getParameter("from");
    auto *toList = ev->getParameter("to");

    auto itf = fromList->begin();
    auto itt = toList->begin();

    int count = 0;
    while (itf != fromList->end()){
      if (!p->at(itf)->isEmpty()) {
        p->at(itf)->removeClient();
        count++;
      }
      itf++;
    }

    while (itt != fromList->end()){
      while(!p->at(itt)->isFull()){
        p->at(itt)->addClient();
      }
      itt++;
    }

    return p;
  }
};
