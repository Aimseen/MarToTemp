/* -*-c++-*- C++ mode for emacs */
/* State representation */
#ifndef MARTO_STATE_H
#define MARTO_STATE_H

#ifdef __cplusplus

#include <list>
#include <marto/forwardDecl.h>
#include <vector>

namespace marto {

/** Set implementation ; internal structure
*/
class SetImpl {
    /** used to call the transition:apply function
    * corresponding to the SetImpl type
         */
  public:
    virtual SetImpl *accept(Transition *t, Event *ev) = 0;
};

/** One point in the state space
*/
class Point : public std::vector<int>, public SetImpl {
  public:
    // Specialisation: a Point always gives a Point
    virtual Point *accept(Transition *t, Event *ev);
};

/** Subset of the state space with a shape of hyperrectangle
*/
class HyperRectangle : public SetImpl {
  public:
    virtual SetImpl *accept(Transition *t, Event *ev);
    marto::Point *inf() const { return inf_; };
    marto::Point *inf(marto::Point *p) { // sets inf_ to Point p
        inf_ = p;
        return inf_;
    };
    marto::Point *sup() const { return sup_; };
    marto::Point *sup(marto::Point *p) {
        sup_ = p;
        return sup_;
    };

  private:
    marto::Point *inf_, *sup_;
};

/** Union of subsets of the state space
*/
class Union : public SetImpl {
  public:
    virtual SetImpl *accept(Transition *t, Event *ev);

  private:
    typedef std::list<SetImpl *> unionList;
    unionList list;

  public:
    unionList::iterator begin() { return list.begin(); };
    unionList::iterator end() { return list.end(); };
    unionList::const_iterator begin() const { return list.begin(); };
    unionList::const_iterator end() const { return list.end(); };
};

/** Class that is a bridge to the actual set implementation
*/
class Set {
  public:
    SetImpl *realset(void) { return _realset; }; // read
    void realset(SetImpl *s) { _realset = s; };  // write
  private:
    SetImpl *_realset;
};
}

#endif

#endif
