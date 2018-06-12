#include "gtest/gtest.h"
#include <marto.h>

namespace {

using namespace marto;

/** Common base for history tests (in the google framework)
 */
class SimpleForwardBaseTest : public ::testing::Test {
  protected:
    SimpleForwardBaseTest() {
        c = new Configuration();
        // ensure TransitionTest exists. Can return NULL if already registered.
	c->loadTransitionLibrary();
        et = new EventType(c, "My super event", 2.0, "ArrivalReject");
        std::vector<int> v;
        v.push_back(0);
        et->registerParameter("to", new FormalConstantList<int>(1, v));
        e = new Event();
        h = new TestEventsHistory(c);
    }

    virtual ~SimpleForwardBaseTest() {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    virtual void SetUp() {
        ASSERT_TRUE(c);
        ASSERT_TRUE(et);
        ASSERT_TRUE(e);
        ASSERT_TRUE(h);
    }

    virtual void TearDown() {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    Configuration *c;
    EventType *et;
    Event *e;
    TestEventsHistory *h;
};


TEST_F(SimpleForwardBaseTest, SimpleForward) {
    auto itw = h->iterator();
    ASSERT_TRUE(itw);
    // Fixme : generator
    e->generate(et, nullptr);
    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(EVENT_STORED, itw->storeNextEvent(e));
    }
    auto itr = h->iterator();
    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(EVENT_LOADED, itr->loadNextEvent(e));
        ASSERT_EQ(e->type(), et);
    }
    // No more events
    // Not END_OF_HISTORY as it is possible to generate new events here
    ASSERT_EQ(UNDEFINED_EVENT, itr->loadNextEvent(e));
    // and we will test it
    ASSERT_EQ(EVENT_STORED, itw->storeNextEvent(e));
    ASSERT_EQ(EVENT_LOADED, itr->loadNextEvent(e));
    ASSERT_EQ(UNDEFINED_EVENT, itr->loadNextEvent(e));

    // Now, we generate new events until a new chunk is required
    unsigned nbGeneratedEvents = 11;
    while (h->nbAllocatedChunk == 1 && nbGeneratedEvents < 1000000) {
        ASSERT_EQ(EVENT_STORED, itw->storeNextEvent(e));
        nbGeneratedEvents++;
    }
    ASSERT_NE(h->nbAllocatedChunk, 1);
    unsigned nbReadEvents = 11;
    while (nbReadEvents < nbGeneratedEvents) {
        ASSERT_EQ(EVENT_LOADED, itr->loadNextEvent(e));
        ASSERT_EQ(e->type(), et);
        nbReadEvents++;
    }
    ASSERT_EQ(UNDEFINED_EVENT, itr->loadNextEvent(e));
    // And we try to load again ALL events
    itr = h->iterator();
    nbReadEvents = 0;
    while (nbReadEvents < nbGeneratedEvents) {
        ASSERT_EQ(EVENT_LOADED, itr->loadNextEvent(e));
        ASSERT_EQ(e->type(), et);
        nbReadEvents++;
    }
    ASSERT_EQ(UNDEFINED_EVENT, itr->loadNextEvent(e));
    std::cout << "Nb events: " << nbReadEvents << std::endl;
}

} // namespace
