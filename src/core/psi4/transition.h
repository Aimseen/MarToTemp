/* Transition representation */
#ifndef PSI4_TRANSITION_H
#define PSI4_TRANSITION_H

#include <psi4/state.h>

namespace psi4 {

	abstract public class Transition {
		
	public:
		int apply(Set s, Event ev)
		{
			SetImpl &src=s.realset;
			SetImpl &res=apply(src, ev)
			s.realset = res;
		}
		/** Apply transition to a point 
		 */
		SetImpl& apply(Point &p, Event)
		{
			foreach (int ind: p) {
				p[ind] = max(0,p[ind] -1);
			}
			return p;
		}
		/** Apply transition to a set of states with
		 * hyperrectangle structure
		 */
		SetImpl& apply(HyperRectangle &h, Event) {
			// Split
			Union myUnion;
			myUnion = new Union();
			// En place
			myTransition(h);
			myUnion.add(h);
			foreach (Point p : findPointsOutside(h)) {
				myUnion.add(p);
			}
			return myUnion;


			// Classique
			return myTransition(h);
		}
			
		SetImpl& apply(Union&, Event);
			
	}


}

#endif
