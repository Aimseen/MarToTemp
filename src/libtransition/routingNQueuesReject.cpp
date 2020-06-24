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

    int count = 0;
    for(unsigned int i=0; i<fromList->size(); ++i){
      auto index = fromList->get<queue_id_t>(i);
      if (!p->at(index)->isEmpty()) {
        p->at(index)->removeClient();
        count++;
      }
    }

    int countdown;
    for(unsigned int i=0; i<toList->size(); ++i){
      auto index = toList->get<queue_id_t>(i);
      countdown = count;
      while(!p->at(index)->isFull() && countdown>0){
        p->at(index)->addClient();
        countdown--;
      }
    }

    return p;
  }
};
