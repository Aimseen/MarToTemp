#include <config.h>
#include <marto/transition.h>
#include <stdexcept>

/** doc from transition.cpp */
namespace marto {

SetImpl *Transition::apply(SetImpl *s, Event *ev) {
    return s->accept(this, ev);
}

int Transition::apply(Set *s, Event *ev) {
    SetImpl *src = s->realset();
    SetImpl *res =
        src->accept(this, ev); // specialized apply may have side effects
    s->realset(res);
    return 0;
}

// The following method is virtual pure :
// It has to be given explicitely for each concrete transition type
// Point *marto::Transition::apply(Point *s, Event *ev) = 0;

/** Apply transition to a set of states with
 * hyperrectangle structure;
 */
SetImpl *Transition::apply(HyperRectangle *h, Event *ev) {
    h->inf(apply(h->inf(), ev));
    h->sup(apply(h->sup(), ev));
    return h;
}

SetImpl *Transition::apply(Union *u, Event *ev) {
    for (auto it = u->begin(); it != u->end(); it++)
        (*it) = apply(*it, ev);
    return u;
}
}
