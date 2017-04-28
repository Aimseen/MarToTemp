/* State representation */
#ifndef PSI4_STATE_H
#define PSI4_STATE_H

#include <psi4/state.h>
#include <vector>

namespace psi4 {

  void Point::accept(Transition *t, Event *ev) {
    t->apply(this, ev);
  }

  void HyperRectangle::accept(Transition *t, Event *ev) {
    t->apply(this, ev);
  }

  void Union::accept(Transition *t, Event *ev) {
    t->apply(this, ev);
  }
  
}

#endif
