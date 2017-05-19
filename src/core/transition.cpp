#include <marto/transition.h>
#include <config.h>
#include <stdexcept>

using namespace marto;

SetImpl *marto::Transition::apply(SetImpl *s, Event *ev) {
  throw std::domain_error("A marto::Transition::apply specialized method should have been called" );
}

int marto::Transition::apply(Set *s, Event *ev) {
  SetImpl *src=s->realset();
  SetImpl *res=apply(src, ev);
  s->realset(res);
  return 0;
}

// Specialisation: a Point always gives a Point
Point *marto::Transition::apply(Point *s, Event *ev) {
  return s->accept(this, ev);
}

/** Apply transition to a set of states with
 * hyperrectangle structure;
 */
SetImpl *marto::Transition::apply(HyperRectangle *h, Event *ev) {
  h->inf(apply(h->inf(), ev));
  h->sup(apply(h->sup(), ev));
  return h;
}

SetImpl *marto::Transition::apply(Union *u, Event *ev) {
  for (auto it = u->begin(); it != u->end(); it++)
    (*it) = apply(*it, ev);
  return u;
}
