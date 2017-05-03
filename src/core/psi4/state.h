/* State representation */
#ifndef PSI4_STATE_H
#define PSI4_STATE_H

#ifdef __cplusplus

#include <vector>
#include <list>

namespace psi4 {
	/* Forward declarations */
	class Transition;
	class Event;
	
	/** Set implementation ; internal structure
	 */
	class SetImpl {
		/** used to call the transition:apply function
		 * corresponding to the SetImpl type
		 */
	public:
		virtual SetImpl* accept(Transition *t, Event *ev) = 0;
	};

	/** One point in the state space
	 */
	class Point : public std::vector<int>, public SetImpl {
	public:
		// Specialisation: a Point always gives a Point
		virtual Point* accept(Transition *t, Event *ev);
	};

	/** Subset of the state space with a shape of hyperrectangle
	 */
	class HyperRectangle : public SetImpl {
	public:
		virtual SetImpl* accept(Transition *t, Event *ev);
		psi4::Point* inf() const {
			return inf_;
		}
		psi4::Point* inf(psi4::Point *p) {
			inf_ = p;
			return inf_;
		}
		psi4::Point* sup() const {
			return sup_;
		}
		psi4::Point* sup(psi4::Point *p) {
			sup_ = p;
			return sup_;
		}
	private:
		psi4::Point *inf_, *sup_;
	};

	/** Union of subsets of the state space
	 */
	class Union : public SetImpl {
	public:
		virtual SetImpl* accept(Transition *t, Event *ev);
	private:
		typedef std::list<SetImpl *> unionList;
		unionList list;

	public:
		unionList::iterator begin() {
			return list.begin();
		}
		unionList::iterator end() {
			return list.end();
		}
		unionList::const_iterator begin() const {
			return list.begin();
		}
		unionList::const_iterator end() const {
			return list.end();
		}
	};

	
	
	/** Class that is a bridge to the actual set implementation
	 */
	class Set {
	public:
		SetImpl* realset(void) { return _realset; };
		void realset(SetImpl*s) { _realset = s; };
	private:
		SetImpl *_realset;
	};
}

#endif

#endif
