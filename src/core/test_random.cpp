#include "test_transition.h"
#include "gtest/gtest.h"
#include <marto.h>
#include <marto/randomDeterministic.h>
#include <stdbool.h>

namespace {

using namespace marto;

/** Checking that RandomDeterministic gives us back the provided values */
TEST(RandomDeterministic, GetContent) {

    std::vector<double> s0{1, 2, 3, 4};
    std::vector<double> s1{10, 11, 12, 13, 14};
    std::vector<double> s2{0.1, 1.1, 1.2, 1.3, 1.4};
    std::vector<std::vector<double> *> streams{&s0, &s1, &s2};

    RandomFabric *rf = new RandomDeterministic(&streams);
    ASSERT_TRUE(rf);
    Configuration *c = new Configuration(rf);
    ASSERT_TRUE(c);
    Random *r = c->newDebugRandom();
    ASSERT_TRUE(r);
    for (std::vector<double> *stream : streams) {
        RandomStream *rs;
        if (stream == &s0) {
            rs = r;
        } else {
            rs = r->newRandomStream();
        }
        for (double vinit : *stream) {
            double v = rs->Uab(0, 100);
            ASSERT_EQ(vinit, v);
        }
        ASSERT_THROW(rs->Uab(0, 100), std::out_of_range);
    }
    ASSERT_THROW(r->newRandomStream(), std::out_of_range);
}

/** Checking that RandomDeterministic generates an exception if provided values
 * are wrong */
TEST(RandomDeterministic, CheckBounds) {

    std::vector<double> s0{1,  0.5, 0, -0.00001, 0.999999, 1, 1.1, 10.99999999,
                           11, 13,  0, 1,        2,        3, 4,   5};
    std::vector<std::vector<double> *> streams{&s0};

    RandomFabric *rf = new RandomDeterministic(&streams);
    ASSERT_TRUE(rf);
    Configuration *c = new Configuration(rf);
    ASSERT_TRUE(c);
    Random *r = c->newDebugRandom();
    ASSERT_TRUE(r);

    ASSERT_THROW(r->U01(), std::out_of_range);
    ASSERT_EQ(0.5, r->U01());
    ASSERT_EQ(0, r->U01());
    ASSERT_THROW(r->U01(), std::out_of_range);
    ASSERT_THROW(r->Uab(1, 11), std::out_of_range);
    ASSERT_EQ(1, r->Uab(1, 11));
    ASSERT_EQ(1.1, r->Uab(1, 11));
    ASSERT_EQ(10.99999999, r->Uab(1, 11));
    ASSERT_THROW(r->Uab(1, 11), std::out_of_range);
    ASSERT_THROW(r->Uab(1, 11), std::out_of_range);
    ASSERT_THROW(r->Iab(1, 3), std::out_of_range);
    ASSERT_EQ(1, r->Iab(1, 3));
    ASSERT_EQ(2, r->Iab(1, 3));
    ASSERT_EQ(3, r->Iab(1, 3));
    ASSERT_THROW(r->Iab(1, 3), std::out_of_range);
    ASSERT_EQ(5, r->Iab(5, 5));

    ASSERT_THROW(r->U01(), std::out_of_range);
}

/** Checking that getRandomEventType gives us the expected result
 */
TEST(RandomTest, getRandomEventType) {

    std::vector<double> s0{0.2, 0.5, 1.1, 1.4, 1.499999, 0};
    std::vector<std::vector<double> *> streams{&s0};

    RandomFabric *rf = new RandomDeterministic(&streams);
    ASSERT_TRUE(rf);
    Configuration *c = new Configuration(rf);
    ASSERT_TRUE(c);
    Random *r = c->newDebugRandom();
    ASSERT_TRUE(r);

    c->loadTransitionLibrary();
    std::cerr << "Transitions library loaded" << std::endl;
    new TransitionTest(c, "TransitionTest");

    EventType *eta, *etb;
    eta = new EventType(c, "Arrival MM1", 0.5, "ArrivalReject");
    etb = new EventType(c, "Departure MM1", 1, "Departure");

    for (double value : s0) {
        EventType *etexpected = (value <= 0.5) ? eta : etb;
        EventType *etgot = c->getRandomEventType(r);
        ASSERT_EQ(etexpected, etgot);
    }
}

#if 0
class RandomBaseTest : public ::testing::Test {
  protected:
    RandomBaseTest() {
        std::vector<double> s0 { 1, 0.5, 0, -0.00001, 0.999999, 1, 1.1, 10.99999999, 11, 13, 0, 1, 2, 3, 4, 5 };
        std::vector<std::vector<double> *> streams { &s0 };
        c = new Configuration(new RandomDeterministic(&streams));
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
        // std::cout << state_pt << std::endl;
        e->apply(state_pt);
        if (itr->loadNextEvent(e) == UNDEFINED_EVENT)
            break; // end of history
    }
    for (auto s : state_pt->states()) {
        ASSERT_EQ(s, 0);
    }
}
#endif
} // namespace
