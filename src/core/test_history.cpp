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

// The fixture for testing class Foo.
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
        h = new EventsHistory(c);
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
    EventsHistory *h;
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
        new EventType(c, "My super event", 42.0, "UnknownTransitionForTest");
        ASSERT_THROW(c->getTransition("UnknownTransitionForTest"),
                     UnknownName)
            << "Transition 'UnknownTransitionForTest' should not exist";
        FAIL() << "EventType successfully created with an unknown transition";
    } catch (const UnknownName &e) {
        SUCCEED();
    } catch (...) {
        FAIL() << "EventType creation generated an unknown transition";
        throw;
    }
}

TEST(Configuration, RegisterEventTypeTwice) {
    auto c = new Configuration();
    // ensure TransitionTest exists. Can return NULL if already registered.
    c->registerTransition("TransitionTest", new TransitionTest());
    ASSERT_TRUE(new EventType(c, "My super event", 42.0, "TransitionTest"));
    ASSERT_THROW(new EventType(c, "My super event", 42.0, "TransitionTest"),
        ExistingName);
}

// Tests that writing a undefined event correctly fails
TEST_F(EventsHistoryBaseTest, writeUndefinedEvent) {
    auto it = h->iterator();
    ASSERT_TRUE(it);
    ASSERT_EQ(EVENT_STORE_UNDEFINED_ERROR, it->storeNextEvent(e));
}

// Tests that it is possible to write 10 events
TEST_F(EventsHistoryBaseTest, writeEvents) {
    auto it = h->iterator();
    ASSERT_TRUE(it);
    // Fixme : generator
    e->generate(et, nullptr);
    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(EVENT_STORED, it->storeNextEvent(e));
    }
}

} // namespace
