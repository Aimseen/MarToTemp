#include "gtest/gtest.h"
#include <marto.h>

namespace {

using namespace marto;

class TransitionTest : public Transition {
    Point *apply(Point *p, __attribute__((unused)) Event *ev) {
        for (int i = 0; i < 3; i++)
            p->at(i)++;
        return p;
    }
};

class TestEventsHistory : public EventsHistory {
  public:
    size_t chunkSize=100;
    size_t nbAllocatedChunk;
    TestEventsHistory(Configuration *c)
        : EventsHistory(c), nbAllocatedChunk(0) {};
    virtual char *allocChunkBuffer(size_t *size) {
        char *buffer = (char *)malloc(chunkSize);
        if (size) {
            *size = buffer ? chunkSize : 0;
        }
        nbAllocatedChunk++;
        return buffer;
    }
};

/** Common base for history tests (in the google framework)
 */
class EventsHistoryBaseTest : public ::testing::Test {
  protected:
    EventsHistoryBaseTest() {
        c = new Configuration();
        // ensure TransitionTest exists. Can return NULL if already registered.
        c->registerTransition("TransitionTest", new TransitionTest());
        et = new EventType(c, "My super event", 42.0, "TransitionTest");
        std::vector<int> v;
        v.push_back(5);
        v.push_back(6);
        et->registerParameter("to", new FormalConstantList<int>(2, v));
        e = new Event();
        h = new TestEventsHistory(c);
    }

    virtual ~EventsHistoryBaseTest() {
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

TEST(Configuration, RegisterTransitionTwice) {
    auto c = new Configuration();
    Transition *tr = new TransitionTest();
    ASSERT_EQ(tr, c->registerTransition("TransitionTestDupName", tr));
    ASSERT_EQ(tr, c->registerTransition("TransitionTestDupName", tr));
}

TEST(Configuration, RegisterEventTypeWithUnknownTransition) {
    auto c = new Configuration();
    // ensure TransitionTest exists. Can return NULL if already registered.
    c->registerTransition("TransitionTest", new TransitionTest());
    try {
        // we should generate a UnknownName exception
        new EventType(c, "My super event", 42.0, "UnknownTransitionForTest");
        // if not, we check for the behavior of getTransition...
        ASSERT_THROW(c->getTransition("UnknownTransitionForTest"), UnknownName)
            << "Transition 'UnknownTransitionForTest' should not exist";
        // ..before reporting the failure
        FAIL() << "EventType successfully created with an unknown transition";
    } catch (const UnknownName &e) {
        // expected path
        SUCCEED();
    } catch (...) {
        // unexpected exception type
        FAIL() << "EventType creation generated an unknown exception";
        throw;
    }
}

TEST(Configuration, RegisterEventTypeTwice) {
    auto c = new Configuration();
    // ensure TransitionTest exists. Can return NULL if already registered.
    c->registerTransition("TransitionTest", new TransitionTest());
    ASSERT_TRUE(new EventType(c, "My super event", 42.0, "TransitionTest"));
    // we cannot create a new EventType with the same name
    ASSERT_THROW(new EventType(c, "My super event", 42.0, "TransitionTest"),
                 ExistingName);
    // but we can create an EventType with an other name and the same transition
    ASSERT_TRUE(new EventType(c, "My new super event", 42.0, "TransitionTest"));
}

// Tests that writing a undefined event correctly fails
TEST_F(EventsHistoryBaseTest, writeUndefinedEvent) {
    auto it = h->iterator();
    ASSERT_TRUE(it);
    ASSERT_EQ(EVENT_STORE_UNDEFINED_ERROR, it->storeNextEvent(e));
}

// Tests that it is possible to read/write events in a history
TEST_F(EventsHistoryBaseTest, readWriteEvents) {
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
    unsigned nbGeneratedEvents=11;
    while (h->nbAllocatedChunk == 1 && nbGeneratedEvents < 1000000) {
        ASSERT_EQ(EVENT_STORED, itw->storeNextEvent(e));
        nbGeneratedEvents++;
    }
    ASSERT_NE(h->nbAllocatedChunk, 1);
    unsigned nbReadEvents=11;
    while (nbReadEvents < nbGeneratedEvents) {
        ASSERT_EQ(EVENT_LOADED, itr->loadNextEvent(e));
        ASSERT_EQ(e->type(), et);
        nbReadEvents++;
    }
    ASSERT_EQ(UNDEFINED_EVENT, itr->loadNextEvent(e));
    // And we try to load again ALL events
    itr = h->iterator();
    nbReadEvents=0;
    while (nbReadEvents < nbGeneratedEvents) {
        ASSERT_EQ(EVENT_LOADED, itr->loadNextEvent(e));
        ASSERT_EQ(e->type(), et);
        nbReadEvents++;
    }
    ASSERT_EQ(UNDEFINED_EVENT, itr->loadNextEvent(e));
    std::cout << "Nb events: " << nbReadEvents << std::endl;

}

// Tests with various chunkSizes to check limit conditions.
TEST_F(EventsHistoryBaseTest, chunkSize) {
    auto itw = h->iterator();
    ASSERT_TRUE(itw);
    // Fixme : generator
    e->generate(et, nullptr);

    unsigned nbGeneratedEvents=0;
    unsigned requestedChunks=2;

    for (size_t size=100; size<150; size++) {
        h->chunkSize=size;
        while (h->nbAllocatedChunk < requestedChunks && nbGeneratedEvents < 1000000) {
            ASSERT_EQ(EVENT_STORED, itw->storeNextEvent(e))
                                     << "current chunk size " << size;
            nbGeneratedEvents++;
        }
        ASSERT_EQ(h->nbAllocatedChunk, requestedChunks);
        requestedChunks++;
    }
    unsigned nbReadEvents=0;
    auto itr = h->iterator();
    while (nbReadEvents < nbGeneratedEvents) {
        ASSERT_EQ(EVENT_LOADED, itr->loadNextEvent(e));
        ASSERT_EQ(e->type(), et);
        nbReadEvents++;
    }
    ASSERT_EQ(UNDEFINED_EVENT, itr->loadNextEvent(e));
    std::cout << "Nb events: " << nbReadEvents << std::endl;

}

} // namespace
