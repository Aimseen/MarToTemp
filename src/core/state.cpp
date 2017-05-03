#include <psi4/state.h>
#include <psi4/transition.h>
#include <vector>

using namespace psi4;

// Specialisation: a Point always gives a Point
Point* psi4::Point::accept(Transition *t, Event *ev) {
  return t->apply(this, ev);
}

SetImpl* psi4::HyperRectangle::accept(Transition *t, Event *ev) {
  return t->apply(this, ev);
}

SetImpl* psi4::Union::accept(Transition *t, Event *ev) {
  return t->apply(this, ev);
}
  
//}

