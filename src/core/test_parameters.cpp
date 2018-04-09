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
    HyperRectangle *apply(HyperRectangle *h,
                          __attribute__((unused)) Event *ev) {
        return h;
    }
    Union *apply(Union *u, __attribute__((unused)) Event *ev) { return u; }
};

// The fixture for testing class Foo.
class EventsHistoryBaseTest : public ::testing::Test {
  protected:
    EventsHistoryBaseTest() {
        c = Global::getConfig();
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

TEST(Event, GenerateEventsWithConstantParameters) {
    auto c = Global::getConfig();
    Transition *tr = new TransitionTest();
    c->registerTransition("TransitionTest", new TransitionTest());
    EventType *et1 = new EventType(c, "My super event", 42.0, "TransitionTest");
    std::vector<int> v1 = {5, 6};
    et1->registerParameter("to", new FormalConstantList<int>(2, v1));
    Event *e = new Event();
    // Fixme: add a randomGenerator
    e->generate(et1, nullptr);
    ASSERT_TRUE(e->getParameter("to") != nullptr);
    ASSERT_EQ(e->getParameter("from"), nullptr);
    auto p = e->getParameter("to");
    ASSERT_EQ(p->get<int>(0), 5);
    ASSERT_EQ(p->get<int>(1), 6);
    ASSERT_THROW(p->get<int>(2), std::out_of_range)
        << "Accessing a non-existant parameter value!";

    // Fixme: add a randomGenerator
    e->generate(et1, nullptr);
    ASSERT_TRUE(e->getParameter("to") != nullptr);
    ASSERT_EQ(e->getParameter("from"), nullptr);
    p = e->getParameter("to");
    ASSERT_EQ(p->get<int>(0), 5);
    ASSERT_EQ(p->get<int>(1), 6);
    ASSERT_THROW(p->get<int>(2), std::out_of_range)
        << "Accessing a non-existant parameter value!";

    EventType *et2 =
        new EventType(c, "My super event 2", 42.0, "TransitionTest");
    std::vector<int> v2 = {15, 16, 17};
    et2->registerParameter("from", new FormalConstantList<int>(3, v2));

    // Fixme: add a randomGenerator
    e->generate(et2, nullptr);
    ASSERT_EQ(e->getParameter("to"), nullptr);
    ASSERT_TRUE(e->getParameter("from") != nullptr);
    p = e->getParameter("from");
    ASSERT_EQ(p->get<int>(0), 15);
    ASSERT_EQ(p->get<int>(1), 16);
    ASSERT_EQ(p->get<int>(2), 17);
    ASSERT_THROW(p->get<int>(3), std::out_of_range)
        << "Accessing a non-existant parameter value!";

    // Fixme: add a randomGenerator
    e->generate(et1, nullptr);
    ASSERT_TRUE(e->getParameter("to") != nullptr);
    ASSERT_EQ(e->getParameter("from"), nullptr);
    p = e->getParameter("to");
    ASSERT_EQ(p->get<int>(0), 5);
    ASSERT_EQ(p->get<int>(1), 6);
    ASSERT_THROW(p->get<int>(2), std::out_of_range)
        << "Accessing a non-existant parameter value!";
}

// TODO : put these 4 events into an history and reload them

} // namespace
