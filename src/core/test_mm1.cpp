#include "test_transition.h"
#include "gtest/gtest.h"
#include <marto.h>
#include <stdbool.h>

namespace {

using namespace marto;

/** Common base for history tests (in the google framework)
 */
class MM1Test : public ::testing::Test {
  protected:
     MM1Test() {
        c = new Configuration();
        c->loadTransitionLibrary();
        std::cerr << "Transitions library loaded" << std::endl;
        new TransitionTest(c, "TransitionTest");
        // let us first test the MM1
        int lambda = 1;
        int mu = 1;
        eta = new EventType(c, "Arrival MM1", lambda, "ArrivalReject");
        etb = new EventType(c, "Departure MM1", mu, "Departure");
        auto q = new StandardQueue(c, "Q1", 10);
        std::vector<queue_id_t>v; // event parameters ; same for both eventTypes here
        v.push_back(q->id()); // only one queue symbolized with its id
        eta->registerParameter("to", new FormalConstantList<queue_id_t>(1, v));
        etb->registerParameter("from", new FormalConstantList<queue_id_t>(1, v));
        //  for testing larger states
        e = new Event(); // only one that will change all the time
        h = new History(c);
    }

  virtual ~MM1Test() {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    virtual void SetUp() {
        ASSERT_TRUE(c);
        ASSERT_TRUE(eta);
        ASSERT_TRUE(etb);
        ASSERT_TRUE(e);
        ASSERT_TRUE(h);
    }

    virtual void TearDown() {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

  Configuration *c;
  EventType *eta, *etb, *etc;
  Event *e;
  History *h;
};

  TEST_F(MM1Test, MM1) {
    // Creating state for simulation
    Point *state_pt = new Point(c);
    // reading history and updating state
    auto it = h->iterator();
    int i=0;
    while (i<100) {
        it->getNextEvent(e);
        std::cout << state_pt << std::endl;
        e->apply(state_pt);
        ++i;
    }
}
} // namespace
