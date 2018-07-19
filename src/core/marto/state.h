/* -*-c++-*- C++ mode for emacs */
/* State representation */
#ifndef MARTO_STATE_H
#define MARTO_STATE_H

#ifdef __cplusplus

#include <functional>
#include <list>
#include <marto/forwardDecl.h>
#include <vector>

// convolution to keep operator<< in global namespace
// See
// https://stackoverflow.com/questions/38801608/friend-functions-and-namespaces
std::ostream &operator<<(std::ostream &out, const marto::Point &p);

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
class Point : protected std::vector<Queue *>,
              public SetImpl,
              protected WithConfiguration {
  public:
    /** No default constructor, a Configuration must always be provided */
    Point() = delete;
    /** Create a point (queue states) from registered queue
     *
     * The queue states will have a default value
     */
    Point(Configuration *config);
    /** Create a point (queue states) from registered queue
     *
     * The queue states will be initialized from the callback return value
     */
    typedef queue_state_t(initCallback_t)(queue_id_t id, QueueConfig *qc,
                                          Point *p, void *arg);
    Point(Configuration *config, initCallback_t *callback, void *arg);
    /** callback type to help to initialize queues when creating a Point
     *
     * The computed queue_state_t will be given to the QueueConfig
     * newQueue (hence also allocate) method.
     */
    typedef std::function<queue_state_t(queue_id_t id, QueueConfig *qc,
                                        Point *p)>
        initLambdaCallback_t;
    /** Create a point (queue states) from registered queue
     *
     * The queue states will be initialized from the lambda callback return
     * value
     */
    Point(Configuration *config, const initLambdaCallback_t &lambdaCallback);
    /** Create a point (queue states) from registered queue
     *
     * The queue states will be initialized from the value
     */
    Point(Configuration *config, queue_state_t value);
    // Specialisation: a Point always gives a Point
    virtual Point *accept(Transition *t, Event *ev);
    /** accessing the various queues */
    Queue *at(queue_id_t id) const { return std::vector<Queue *>::at(id); }
    std::vector<Queue *>::iterator begin() {
        return std::vector<Queue *>::begin();
    }
    std::vector<Queue *>::iterator end() { return std::vector<Queue *>::end(); }
    std::vector<Queue *>::const_iterator begin() const {
        return std::vector<Queue *>::begin();
    }
    std::vector<Queue *>::const_iterator end() const {
        return std::vector<Queue *>::end();
    }
    std::vector<Queue *>::const_iterator cbegin() const noexcept {
        return std::vector<Queue *>::cbegin();
    }
    std::vector<Queue *>::const_iterator cend() const noexcept {
        return std::vector<Queue *>::cend();
    }
    std::vector<queue_state_t> states() const {
        std::vector<queue_state_t> v(0);
        v.reserve(config()->queueConfigsVector.size());
        for (const auto &q : (*this)) {
            v.push_back(q->state());
        }
        return v;
    }
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
        delete (inf_);
        inf_ = nullptr;
        delete (sup_);
        sup_ = nullptr;
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
