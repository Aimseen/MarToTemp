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
};

class TestEventsHistory : public EventsHistory {
  public:
    size_t chunkSize = 100;
    size_t nbAllocatedChunk;
    TestEventsHistory(Configuration *c)
        : EventsHistory(c), nbAllocatedChunk(0){};
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
        new TransitionTest(c, "TransitionTest");
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

// Tests with various chunkSizes to check limit conditions.
TEST_F(EventsHistoryBaseTest, chunkSize) {
    auto itw = h->iterator();
    ASSERT_TRUE(itw);
    // Fixme : generator
    e->generate(et, nullptr);

    unsigned nbGeneratedEvents = 0;
    unsigned requestedChunks = 2;

    for (size_t size = 100; size < 150; size++) {
        h->chunkSize = size;
        while (h->nbAllocatedChunk < requestedChunks &&
               nbGeneratedEvents < 1000000) {
            ASSERT_EQ(EVENT_STORED, itw->storeNextEvent(e))
                << "current chunk size " << size
                << " nbGeneratedEvents=" << nbGeneratedEvents;
            nbGeneratedEvents++;
        }
        ASSERT_EQ(h->nbAllocatedChunk, requestedChunks);
        requestedChunks++;
    }
    unsigned nbReadEvents = 0;
    auto itr = h->iterator();
    while (nbReadEvents < nbGeneratedEvents) {
        ASSERT_EQ(EVENT_LOADED, itr->loadNextEvent(e));
        ASSERT_EQ(e->type(), et);
        nbReadEvents++;
    }
    ASSERT_EQ(UNDEFINED_EVENT, itr->loadNextEvent(e));
    std::cout << "Nb events: " << nbReadEvents << std::endl;
}

class TestEventType : public EventType {
  public:
    TestEventType(Configuration *config, string eventName, double evtRate,
                  string trName)
        : EventType(config, eventName, evtRate, trName){};

  protected:
    template <typename T> void _load(EventsIStream &istream) {
        T val;
        size_t nbits = sizeof(T) * 8 - (std::is_signed<T>::value ? 1 : 0);
        if (std::is_signed<T>::value) {
            for (size_t i = 0; i <= nbits; i++) {
                istream >> val;
                ASSERT_EQ(val, ((T)-1) * ((T)1 << i));
            }
        }
        for (size_t i = 0; i < nbits; i++) {
            istream >> val;
            ASSERT_EQ(val, (T)1 << i);
        }
    }
    template <typename T> void _store(EventsOStream &ostream) {
        T val;
        size_t nbits = sizeof(T) * 8 - (std::is_signed<T>::value ? 1 : 0);
        if (std::is_signed<T>::value) {
            for (size_t i = 0; i <= nbits; i++) {
                val = ((T)-1) * ((T)1 << i);
                ostream << val;
            }
        }
        for (size_t i = 0; i < nbits; i++) {
            val = (T)1 << i;
            ASSERT_TRUE(ostream << val);
        }
    }
    virtual event_access_t load(EventsIStream &istream, Event *event,
                                EventsHistory *hist) {
        _load<int8_t>(istream);
        _load<uint8_t>(istream);
        _load<int16_t>(istream);
        _load<uint16_t>(istream);
        _load<int32_t>(istream);
        _load<uint32_t>(istream);
        _load<int64_t>(istream);
        _load<uint64_t>(istream);
        return EventType::load(istream, event, hist);
    }
    virtual event_access_t store(EventsOStream &ostream, Event *event,
                                 EventsHistory *hist) {
        _store<int8_t>(ostream);
        _store<uint8_t>(ostream);
        _store<int16_t>(ostream);
        _store<uint16_t>(ostream);
        _store<int32_t>(ostream);
        _store<uint32_t>(ostream);
        _store<int64_t>(ostream);
        _store<uint64_t>(ostream);
        return EventType::store(ostream, event, hist);
    }
};

// Tests when storing different values
TEST_F(EventsHistoryBaseTest, values) {
    EventType *testet =
        new TestEventType(c, "My test event", 42.0, "TransitionTest");
    std::vector<int> v;
    v.push_back(5);
    v.push_back(6);
    testet->registerParameter("to", new FormalConstantList<int>(2, v));

    // Fixme : generator
    e->generate(testet, nullptr);

    auto itw = h->iterator();
    h->chunkSize = 1024 + 256 + 64 + 8 + 2; // 1024+512+256+128+64+32+16+8;
    ASSERT_TRUE(itw);
    ASSERT_EQ(EVENT_STORED, itw->storeNextEvent(e));
    ASSERT_EQ(EVENT_STORED, itw->storeNextEvent(e));

    auto itr = h->iterator();
    ASSERT_EQ(EVENT_LOADED, itr->loadNextEvent(e));
    ASSERT_EQ(e->type(), testet);
    ASSERT_EQ(EVENT_LOADED, itr->loadNextEvent(e));
    ASSERT_EQ(e->type(), testet);

    ASSERT_EQ(UNDEFINED_EVENT, itr->loadNextEvent(e));
}

} // namespace
