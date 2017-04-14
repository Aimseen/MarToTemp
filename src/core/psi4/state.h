/* State representation */
#ifndef PSI4_STATE_H
#define PSI4_STATE_H

#include <vector>

namespace psi4 {
	/** Set implementation ; internal structure
	 */
	abstract public class SetImpl {

	}

	/** One point in the state space 
	 */ 
	public class Point : vector<int>, SetImpl {
		
	}

	/** Subset of the state space with a shape of hyperrectangle
	 */ 
	public class HyperRectangle : SetImpl {
		private Point inf, sup;
		
	}

	/** Union of subsets of the state space 
	 */ 
	public class Union : SetImpl {
		private List<SetImpl> list;
	}

	
	
	/** Class that is a bridge to the actual set implementation
	 */
	public class Set {
		private SetImpl *realset;
	}
}

#endif
