/* -*-c++-*- C++ mode for emacs */
/* Transition representation */
#ifndef MARTO_TRANSITION_H
#define MARTO_TRANSITION_H

#ifdef __cplusplus

#include <marto/state.h>

namespace marto {

class Transition {

public:
    /** in: s, ev
    *  out: ? (FIXME)
    	 * Apply transition to state in Set s
    	 * May change the type of the state contained in Set s, from HyperRectangle to Union for instance
    	 */
    int apply(Set * s, Event * ev);

    /** Technical apply method
    * Does nothing except calling apply for the proper SetImpl subtype
    	 */
    SetImpl *apply(SetImpl * s, Event * ev);

    /** Apply transition to a point
    Should be implemented in each concrete Transition class
    // Specialisation: a Point always gives a Point
    */
    virtual Point *apply(Point * p, Event * ev) = 0;

    /** Apply transition to a set of states with
    * hyperrectangle structure
    	 * default : applies transition to each point of the hyperrectangle
    	 */
    virtual SetImpl *apply(HyperRectangle * h, Event * ev) = 0;

    /** Apply transition to a set of states with
    * union structure;
    	 * default : applies transition to each point of the union
    	 */
    virtual SetImpl *apply(Union * u, Event * ev) = 0;

};

}
/* Pseudo code d'explication :
   - ma poltique :
   class MaPolitique : Transition {
   SetImpl apply(Point *p, Event *ev) {
   calcul(p);
   return p;
   }
   }

   - enveloppe :
   class MesEnveloppes : MaPolitique {
   SetImpl apply(HyperRectangle *h, Event *ev) {
   calcul(h);
   return h;
   }
   }

   ou

   class MesEnveloppes : Transition {
   SetImpl apply(Point *p, Event *ev) {
   calcul(p);
   return p;
   }

   SetImpl apply(HyperRectangle *h, Event *ev) {
   calcul(h);
   return h;
   }
   }
 */
#endif
#endif
