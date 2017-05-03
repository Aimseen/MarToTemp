#include <psi4/transition.h>
#include <config.h>
#include <stdexcept>

using namespace psi4;

SetImpl *psi4::Transition::apply(SetImpl *s, Event *ev) {
  throw std::domain_error("A psi4::Transition::apply specialized method should have been called" );
}

int psi4::Transition::apply(Set *s, Event *ev) {
  SetImpl *src=s->realset();
  SetImpl *res=apply(src, ev);
  s->realset(res);
  return 0;
}

// Specialisation: a Point always gives a Point
Point *psi4::Transition::apply(Point *s, Event *ev) {
  return s->accept(this, ev);
}

/** Apply transition to a set of states with
 * hyperrectangle structure;
 */
#if 1
SetImpl *psi4::Transition::apply(HyperRectangle *h, Event *ev) {
  h->inf(apply(h->inf(), ev));
  h->sup(apply(h->sup(), ev));
  return h;
}
#endif

SetImpl *psi4::Transition::apply(Union *u, Event *ev) {
  for (auto it = u->begin(); it != u->end(); it++)
    (*it) = apply(*it, ev);
}
