#include "gtest/gtest.h"
#include <marto.h>

namespace {

using namespace marto;

class TransitionTest : public Transition {
    Point *apply(Point *p, __attribute__((unused)) Event *ev) {
        for (int i = 0; i < 3; i++)
            p->at(i)->addClient(1);
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

TEST(Event, RegisterConstantParameters) {
    auto c = new Configuration();
    c->registerTransition("TransitionTest", new TransitionTest());
    EventType *et1 = new EventType(c, "Ev type", 42.0, "TransitionTest");
    std::vector<int> v1 = {5, 6};
    std::vector<int> v2 = {15, 16, 17};
    ASSERT_EQ(et1->registerParameter("to", new FormalConstantList<int>(2, v1)),
              true);
    ASSERT_EQ(
        et1->registerParameter("from", new FormalConstantList<int>(2, v2)),
        true);
    ASSERT_THROW(
        et1->registerParameter("to", new FormalConstantList<int>(2, v2)),
        ExistingName);
    Event *e = new Event();
    // Fixme: add a randomGenerator
    e->generate(et1, nullptr);
    ASSERT_TRUE(e->getParameter("to") != nullptr);
    ASSERT_TRUE(e->getParameter("from") != nullptr);
    ASSERT_EQ(e->getParameter("other"), nullptr);
    auto p = e->getParameter("to");
    ASSERT_EQ(p->get<int>(0), 5);
    ASSERT_EQ(p->get<int>(1), 6);
    ASSERT_THROW(p->get<int>(2), std::out_of_range)
        << "Accessing a non-existant parameter value!";
    p = e->getParameter("from");
    ASSERT_EQ(p->get<int>(0), 15);
    ASSERT_EQ(p->get<int>(1), 16);
    ASSERT_THROW(p->get<int>(2), std::out_of_range)
        << "Accessing a non-existant parameter value!";
    ASSERT_EQ(p->get<int>(0), 15);
    ASSERT_THROW(p->get<unsigned long>(0), TypeError);
}

TEST(Event, GenerateEventsWithConstantParameters) {
    auto c = new Configuration();
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

TEST(Event, StoreAndLoadEventsWithConstantParameters) {
    auto c = new Configuration();
    c->registerTransition("TransitionTest", new TransitionTest());
    EventType *et1 = new EventType(c, "Event Type 1", 42.0, "TransitionTest");
    std::vector<int> v1 = {5, 6};
    et1->registerParameter("to", new FormalConstantList<int>(2, v1));
    EventType *et2 = new EventType(c, "Event type 2", 42.0, "TransitionTest");
    std::vector<int> v2 = {15, 16, 17};
    et2->registerParameter("from", new FormalConstantList<int>(3, v2));
    EventsHistory *h = new EventsHistory(c);
    auto it = h->iterator();
    ASSERT_TRUE(it);

    Event *e = new Event();
    // Fixme: add a randomGenerator
    e->generate(et1, nullptr);
    ASSERT_EQ(it->storeNextEvent(e), EVENT_STORED);
    e->generate(et1, nullptr);
    ASSERT_EQ(it->storeNextEvent(e), EVENT_STORED);
    e->generate(et2, nullptr);
    ASSERT_EQ(it->storeNextEvent(e), EVENT_STORED);
    e->generate(et1, nullptr);
    ASSERT_EQ(it->storeNextEvent(e), EVENT_STORED);

    it = h->iterator();
    e->generate(et1, nullptr);
    ASSERT_EQ(it->storeNextEvent(e), EVENT_STORE_ERROR);

    it->loadNextEvent(e);
    ASSERT_TRUE(e->getParameter("to") != nullptr);
    ASSERT_EQ(e->getParameter("from"), nullptr);
    auto p = e->getParameter("to");
    ASSERT_EQ(p->get<int>(0), 5);
    ASSERT_EQ(p->get<int>(1), 6);
    ASSERT_THROW(p->get<int>(2), std::out_of_range)
        << "Accessing a non-existant parameter value!";

    ASSERT_EQ(it->loadNextEvent(e), EVENT_LOADED);
    ASSERT_TRUE(e->getParameter("to") != nullptr);
    ASSERT_EQ(e->getParameter("from"), nullptr);
    p = e->getParameter("to");
    ASSERT_EQ(p->get<int>(0), 5);
    ASSERT_EQ(p->get<int>(1), 6);
    ASSERT_THROW(p->get<int>(2), std::out_of_range)
        << "Accessing a non-existant parameter value!";

    ASSERT_EQ(it->loadNextEvent(e), EVENT_LOADED);
    ASSERT_EQ(e->getParameter("to"), nullptr);
    ASSERT_TRUE(e->getParameter("from") != nullptr);
    p = e->getParameter("from");
    ASSERT_EQ(p->get<int>(0), 15);
    ASSERT_EQ(p->get<int>(1), 16);
    ASSERT_EQ(p->get<int>(2), 17);
    ASSERT_THROW(p->get<int>(3), std::out_of_range)
        << "Accessing a non-existant parameter value!";

    it->loadNextEvent(e);
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
