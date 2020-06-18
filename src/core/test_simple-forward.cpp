#include "test_transition.h"
#include "gtest/gtest.h"
#include <marto.h>
#include <stdbool.h>

namespace {

using namespace marto;

/** Common base for history tests (in the google framework)
 */
class SimpleForwardBaseTest : public ::testing::Test {
  protected:
    SimpleForwardBaseTest() {
        c = new Configuration();
        c->loadTransitionLibrary();
        std::cerr << "Transitions library loaded" << std::endl;
        new TransitionTest(c, "TransitionTest");
        // let us first test the MM1
        eta = new EventType(c, "Arrival MM1", 0.5, "ArrivalReject");
        etb = new EventType(c, "Departure MM1", 1, "Departure");
        etc = new EventType(c, "Transition MM1", 1, "TransitionTest");
        auto q = new StandardQueue(c, "Q1", 10);
        std::vector<queue_id_t>v; // event parameters ; same for both eventTypes here
        v.push_back(q->id()); // only one queue symbolized with its id
        eta->registerParameter("to", new FormalConstantList<queue_id_t>(1, v));
        etb->registerParameter("from", new FormalConstantList<queue_id_t>(1, v));
        //  for testing larger states
        new StandardQueue(c, "Q2", 10); // unused in MM1 test
        e = new Event(); // only one that will change all the time
        h = new History(c);
    }

    virtual ~SimpleForwardBaseTest() {
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

TEST_F(SimpleForwardBaseTest, StateTest) {
    Point p1(c);
    std::cout << "p1=" << p1 << std::endl;
    for (auto s : p1.states()) {
        ASSERT_EQ(s, 0);
    }
    ASSERT_EQ(p1.states().size(), 2); //< 2 queues created
    // p1.at(0)->addClient(1);
    Point p2(c, 8);
    std::cout << "p2=" << p2 << std::endl;
    for (auto s : p2.states()) {
        ASSERT_EQ(s, 8);
    }
    ASSERT_EQ(p2.states().size(), 2); //< 2 queues created
}

/* Testing M/M/1 trajectory with hard-coded history  */
TEST_F(SimpleForwardBaseTest, SimpleForwardMM1) {
    auto itw = h->iterator();
    ASSERT_TRUE(itw);
    // Fixme : generator //TO DO voir avec vincent
    // we fill the history in advance with arrivals and departures.
    e->generate(eta, nullptr);
    int nba = 10;
    int nbd = nba;
    for (int i = 0; i < nba; i++) {
        ASSERT_EQ(HISTORY_DATA_STORED, itw->storeNextEvent(e));
    }
    e->generate(etb, nullptr);
    for (int i = 0; i < nbd; i++) {
        ASSERT_EQ(HISTORY_DATA_STORED, itw->storeNextEvent(e));
    }
    e->generate(eta, nullptr);
    ASSERT_EQ(HISTORY_DATA_STORED, itw->storeNextEvent(e));
    e->generate(etb, nullptr);
    ASSERT_EQ(HISTORY_DATA_STORED, itw->storeNextEvent(e));

    // Creating state for simulation
    Point *state_pt = new Point(c);
    // reading history and updating state
    auto itr = h->iterator();
    while (true) {
        std::cout << state_pt << std::endl;
        e->apply(state_pt);
        if (itr->loadNextEvent(e) == HISTORY_END_DATA)
            break; // end of history
    }
    for (auto s : state_pt->states()) {
        ASSERT_EQ(s, 0);
    }
}
TEST_F(SimpleForwardBaseTest, TransitionSimpleTest){

    // Creating state for simulation
    Point *state_pt = new Point(c);
    //generate etc (adding one token to each queue)
    e->generate(etc, nullptr);
    //apply the event "to" the point
    std::cout << state_pt << std::endl;
    e->apply(state_pt);
    //Check if the transition occured correctly by getting the state of eache queues
    for (auto s : state_pt->states()) {
        ASSERT_EQ(s, 1);
    }
  }

TEST_F(SimpleForwardBaseTest, TransitionArrivalRejectTest){
  // Creating state for simulation
  Point *state_pt = new Point(c);
  //generate eta (arrivalReject)
  e->generate(eta, nullptr);
  //apply the event "to" the point
  std::cout << state_pt << std::endl;
  e->apply(state_pt);
  //Check if the transition occured correctly by getting the state of eache queues
  int sum = 0;
  for (auto s : state_pt->states()) {
    sum += s;
  }
  ASSERT_EQ(sum, 1);
}

  /* TEST_F(SimpleForwardBaseTest, TransitionDepartureTest){
    // Creating state for simulation
    Point *state_pt = new Point(c);
    auto itQueues = state_pt->begin();
    itQueues->addClient(3);
    itQueues++;
    itQueues->addClient(3);

    for (auto s : state_pt->states()) {
      ASSERT_EQ(s, 3);
    }
    //generate eta (arrivalReject)
    e->generate(etb, nullptr);
    //apply the event "to" the point
    std::cout << state_pt << std::endl;
    e->apply(state_pt);
    //Check if the transition occured correctly by getting the state of eache queues
    int sum = 0;
    for (auto s : state_pt->states()) {
      sum += s;
    }
    ASSERT_EQ(sum, 1);
  }*/

TEST_F(SimpleForwardBaseTest, TransitionRNQRTest){
  Configuration* cLocal = new Configuration();
  cLocal->loadTransitionLibrary();
  std::cerr << "Transitions library loaded" << std::endl;
  EventType* etd = new EventType(cLocal, "rnqr", 1, "routingNQueuesReject");
  auto from0 = new StandardQueue(cLocal, "Q1", 10);
  auto from1 = new StandardQueue(cLocal, "Q2", 10);
  auto from2 = new StandardQueue(cLocal, "Q3", 10);

  auto to0 = new StandardQueue(cLocal, "Q4", 10);
  auto to1 = new StandardQueue(cLocal, "Q5", 10);
  auto to2 = new StandardQueue(cLocal, "Q6", 10);

  std::vector<queue_id_t> fromVector;
  fromVector.push_back(from0->id());
  fromVector.push_back(from1->id());
  fromVector.push_back(from2->id());

  std::vector<queue_id_t> toVector;
  toVector.push_back(to0->id());
  toVector.push_back(to1->id());
  toVector.push_back(to2->id());

  etd->registerParameter("to", new FormalConstantList<queue_id_t>(3, toVector));
  etd->registerParameter("from", new FormalConstantList<queue_id_t>(3, fromVector));

  // from0->addClient(3);
  // from1->addClient(3);
  // from2->addClient(3);

  e->generate(etd, nullptr);
  Point *state_pt = new Point(cLocal);
  e->apply(state_pt);

  for (int i=0; i < 3 ; ++i){
    ASSERT_EQ(state_pt->states().at(i), 2);
  }
  for (int i=3; i < 6 ; ++i){
    ASSERT_EQ(state_pt->states().at(i), 3);
  }
}
} // namespace
