#include "gtest/gtest.h"
#include <marto.h>

namespace {

using namespace marto;
/** dummy transition for testing transition registration */
class TransitionTest : public Transition {
    default_transition_constructors;
    Point *apply(Point *p, __attribute__((unused)) Event *ev) {
        for (int i = 0; i < 3; i++)
            p->at(i)->addClient(1);
        return p;
    }
};

TEST(Transitions, RegisterTransition) {
    auto c = new Configuration();
    Transition *tr = new TransitionTest(c, "TransitionTestDupName");
    ASSERT_NE(tr, nullptr);
    ASSERT_THROW(new TransitionTest(c, "TransitionTestDupName"), ExistingName);
}

TEST(Transitions, LibTransition) {
    auto c1 = new Configuration();
    auto c2 = new Configuration();
    /* loading the default library in c1 */
    c1->loadTransitionLibrary();
    /* Checking a transition is really here */
    Transition *t = c1->getTransition("ArrivalReject");
    ASSERT_NE(t, nullptr);
    ASSERT_TRUE(t->name() == "ArrivalReject");
    /* Checking that we cannot load the same library twice... */
    ASSERT_THROW(c1->loadTransitionLibrary(), ExistingName);
    /* ...unless in another configuration */
    c2->loadTransitionLibrary();
}

TEST(EventTypes, RegisterEventTypeWithUnknownTransition) {
    auto c = new Configuration();
    new TransitionTest(c, "TransitionTest");
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

TEST(EventTypes, RegisterEventTypeTwice) {
    auto c = new Configuration();
    new TransitionTest(c, "TransitionTest");
    ASSERT_TRUE(new EventType(c, "My super event", 42.0, "TransitionTest"));
    // we cannot create a new EventType with the same name
    ASSERT_THROW(new EventType(c, "My super event", 42.0, "TransitionTest"),
                 ExistingName);
    // but we can create an EventType with an other name and the same transition
    ASSERT_TRUE(new EventType(c, "My new super event", 42.0, "TransitionTest"));
}

TEST(Queues, CreateQueueConfigurations) {
    auto c = new Configuration();
    int capacity=10;
    auto q=new StandardQueue(c,"Q1",capacity);
    ASSERT_EQ(q,c->getQueueConfig("Q1"));
    ASSERT_EQ(q->hasNoState(),false);
}

} // namespace
