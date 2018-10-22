#include "gtest/gtest.h"
#include <marto.h>

namespace {

using namespace marto;

class TransitionTest : public Transition {
    default_transition_constructors;
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
class HistoryBaseTest : public ::testing::Test {
  protected:
    HistoryBaseTest() {
        c = new Configuration();
        new TransitionTest(c, "TransitionTest");
        et = new EventType(c, "My super event", 42.0, "TransitionTest");
        std::vector<int> v;
        v.push_back(5);
        v.push_back(6);
        et->registerParameter("to", new FormalConstantList<int>(2, v));
        e = new Event();
        h = new History(c);
    }

    virtual ~HistoryBaseTest() {
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
    History *h;
};

TEST(Event, RegisterConstantParameters) {
    auto c = new Configuration();
    new TransitionTest(c, "TransitionTest");
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
    new TransitionTest(c, "TransitionTest");
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
    new TransitionTest(c, "TransitionTest");
    EventType *et1 = new EventType(c, "Event Type 1", 42.0, "TransitionTest");
    std::vector<int> v1 = {5, 6};
    et1->registerParameter("to", new FormalConstantList<int>(2, v1));
    EventType *et2 = new EventType(c, "Event type 2", 42.0, "TransitionTest");
    std::vector<int> v2 = {15, 16, 17};
    et2->registerParameter("from", new FormalConstantList<int>(3, v2));
    History *h = new History(c);
    auto it = h->iterator();
    ASSERT_TRUE(it);

    Event *e = new Event();
    // Fixme: add a randomGenerator
    e->generate(et1, nullptr);
    ASSERT_EQ(it->storeNextEvent(e), HISTORY_DATA_STORED);
    e->generate(et1, nullptr);
    ASSERT_EQ(it->storeNextEvent(e), HISTORY_DATA_STORED);
    e->generate(et2, nullptr);
    ASSERT_EQ(it->storeNextEvent(e), HISTORY_DATA_STORED);
    e->generate(et1, nullptr);
    ASSERT_EQ(it->storeNextEvent(e), HISTORY_DATA_STORED);

    it = h->iterator();
    e->generate(et1, nullptr);
    ASSERT_EQ(it->storeNextEvent(e), HISTORY_DATA_STORE_ERROR);

    it->loadNextEvent(e);
    ASSERT_TRUE(e->getParameter("to") != nullptr);
    ASSERT_EQ(e->getParameter("from"), nullptr);
    auto p = e->getParameter("to");
    ASSERT_EQ(p->get<int>(0), 5);
    ASSERT_EQ(p->get<int>(1), 6);
    ASSERT_THROW(p->get<int>(2), std::out_of_range)
        << "Accessing a non-existant parameter value!";

    ASSERT_EQ(it->loadNextEvent(e), HISTORY_DATA_LOADED);
    ASSERT_TRUE(e->getParameter("to") != nullptr);
    ASSERT_EQ(e->getParameter("from"), nullptr);
    p = e->getParameter("to");
    ASSERT_EQ(p->get<int>(0), 5);
    ASSERT_EQ(p->get<int>(1), 6);
    ASSERT_THROW(p->get<int>(2), std::out_of_range)
        << "Accessing a non-existant parameter value!";

    ASSERT_EQ(it->loadNextEvent(e), HISTORY_DATA_LOADED);
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
