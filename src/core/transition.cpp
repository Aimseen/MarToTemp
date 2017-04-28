#include <psi4/transition.h>
#include <config.h>

namespace psi4 {

  int Transition::apply(Set *s, Event *ev) {
    SetImpl *src=s->realset;
    SetImpl *res=apply(src, ev);
    s->realset = res;
    return 0;
  }

  SetImpl *Transition::apply(SetImpl *s, Event *ev) {
    return s->accept(this, ev);
  }
		
  /** Apply transition to a set of states with
   * hyperrectangle structure;
   */
  SetImpl *Transition::apply(HyperRectangle *h, Event *ev) {
    h->inf = apply(h->inf, ev);
    h->sup = apply(h->sup, ev);
    return h;
  }
  
  SetImpl *Transition::apply(Union *u, Event *ev) {
    for (Iterator<SetImpl *> it = u->begin(); it != u->end(); it++)
      (*it) = apply(*it, ev);
  }
			
}

#endif
