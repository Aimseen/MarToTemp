#include "test_transition.h"
#include "gtest/gtest.h"
#include <marto.h>
#include <marto/randomDeterministic.h>
#include <stdbool.h>

namespace {

using namespace marto;

TEST(RandomDeterministic, CreateGenerator) {

    std::vector<double> s0{1, 2, 3, 4};
    std::vector<double> s1{10, 11, 12, 13, 14};
    std::vector<std::vector<double> *> streams{&s0, &s1};

    RandomFabric *rf = new RandomDeterministic(&streams);
    ASSERT_TRUE(rf);
    Configuration *c = new Configuration(rf);
    ASSERT_TRUE(c);
    Random *r = c->newDebugRandom();
    ASSERT_TRUE(r);
}

/** Common base for tests (in the google framework)
 */
class RandomBaseTest : public ::testing::Test {
  protected:
    RandomBaseTest() {
        c = new Configuration();
        c->loadTransitionLibrary();
        std::cerr << "Transitions library loaded" << std::endl;
        new TransitionTest(c, "TransitionTest");
        // let us first test the MM1
        eta = new EventType(c, "Arrival MM1", 0.5, "ArrivalReject");
        etb = new EventType(c, "Departure MM1", 1, "Departure");
        auto q = new StandardQueue(c, "Q1", 10);
        std::vector<queue_id_t>
            v; // event parameters ; same for both eventTypes here
        v.push_back(q->id()); // only one queue symbolized with its id
        eta->registerParameter("to", new FormalConstantList<queue_id_t>(1, v));
        etb->registerParameter("from",
                               new FormalConstantList<queue_id_t>(1, v));
        //  for testing larger states
        new StandardQueue(c, "Q2", 10); // unused in MM1 test
        e = new Event(); // only one that will change all the time
        h = new EventsHistory(c);
    }

    virtual ~RandomBaseTest() {
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
    EventType *eta, *etb;
    Event *e;
    EventsHistory *h;
};

TEST_F(RandomBaseTest, GetEvents) {

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

TEST_F(RandomBaseTest, SimpleForwardMM1) {
    auto itw = h->iterator();
    ASSERT_TRUE(itw);
    // Fixme : generator
    // we fill the history in advance with arrivals and departures.
    e->generate(eta, nullptr);
    int nba = 10;
    int nbd = nba;
    for (int i = 0; i < nba; i++) {
        ASSERT_EQ(EVENT_STORED, itw->storeNextEvent(e));
    }
    e->generate(etb, nullptr);
    for (int i = 0; i < nbd; i++) {
        ASSERT_EQ(EVENT_STORED, itw->storeNextEvent(e));
    }
    e->generate(eta, nullptr);
    ASSERT_EQ(EVENT_STORED, itw->storeNextEvent(e));
    e->generate(etb, nullptr);
    ASSERT_EQ(EVENT_STORED, itw->storeNextEvent(e));

    // Creating state for simulation
    Point *state_pt = new Point(c);
    // reading history and updating state
    auto itr = h->iterator();
    while (true) {
        std::cout << state_pt << std::endl;
        e->apply(state_pt);
        if (itr->loadNextEvent(e) == UNDEFINED_EVENT)
            break; // end of history
    }
    for (auto s : state_pt->states()) {
        ASSERT_EQ(s, 0);
    }
}
} // namespace
