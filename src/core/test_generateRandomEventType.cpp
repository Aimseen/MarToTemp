#include "test_transition.h"
#include "gtest/gtest.h"
#include <marto.h>
#include <marto/randomDeterministic.h>
#include <stdbool.h>

namespace {

  using namespace marto;

  /** Checking that RandomDeterministic gives us back the provided values */
  TEST(generateRandomEventType, testRandomRate) {

    std::vector<double> s0{9.0, 15.2, 12.0, 15.0};

    std::vector<std::vector<double> *> streams{&s0};

    RandomFabric *rf = new RandomDeterministic(&streams);
    ASSERT_TRUE(rf);
    Configuration *c = new Configuration(rf);
    ASSERT_TRUE(c);


    c->loadTransitionLibrary();
    new TransitionTest(c, "TransitionTest");
    EventType* eta = new EventType(c, "Arrival MM1", 10, "ArrivalReject");
    EventType* etb = new EventType(c, "Departure MM1", 5, "Departure");
    EventType* etc = new EventType(c, "Transition MM1", 0.3, "TransitionTest");

    Random *r = c->newDebugRandom();
    ASSERT_TRUE(r);

    EventType *etValue = c->getRandomEventType(r);
    ASSERT_EQ("Arrival MM1", etValue->name());

    etValue = c->getRandomEventType(r);
    ASSERT_EQ("Transition MM1", etValue->name());

    etValue = c->getRandomEventType(r);
    ASSERT_EQ("Departure MM1", etValue->name());

    etValue = c->getRandomEventType(r);
    ASSERT_EQ("Transition MM1", etValue->name());
  }

} // namespace
