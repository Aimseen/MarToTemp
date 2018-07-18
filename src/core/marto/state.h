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
    virtual ~SetImpl() {}
    virtual SetImpl *accept(Transition *t, Event *ev) = 0;
};

/** One point in the state space
*/
class Point : public std::vector<Queue*>, public SetImpl {
  public:
    /** Create a point (queue states) from registered queue
     *
     * The queue states will have a default value
     */
    Point(Configuration *config);
    /** Create a point (queue states) from registered queue
     *
     * The queue states will be initialized from the value
     */
    Point(Configuration *config, queue_state_t value);
    // Specialisation: a Point always gives a Point
    virtual Point *accept(Transition *t, Event *ev);
};

/** Subset of the state space with a shape of hyperrectangle
*/
class HyperRectangle : public SetImpl {
  private:
    marto::Point *inf_, *sup_;
  public:
    /** Create a HypepRectangle from two Points
     *
     * the ownership of the two points are transfered to the HyperRectangle
     */
    HyperRectangle(Point *p1, Point *p2) : inf_(p1), sup_(p2) {}
    virtual ~HyperRectangle() {
        delete(inf_); inf_=nullptr;
        delete(sup_); sup_=nullptr;
    }
    virtual SetImpl *accept(Transition *t, Event *ev);
    marto::Point *inf() { return inf_; };
    marto::Point *sup() { return sup_; };
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
