#include <marto.h>

namespace marto {

class RandomTestStreamGenerator;

class RandomTestStream : public RandomStream {
    friend RandomTestStreamGenerator;

  private:
    int base;
    int cur;
    int initCur;

  protected:
    RandomTestStream(int base) : base(base), cur(0), initCur(0){};

  public:
    virtual event_access_t load(EventsIStream &istream,
                                EventsHistory *__marto_unused(hist)) {
        if (!(bool)(istream >> base)) {
            return EVENT_LOAD_ERROR;
        }
        if (!(bool)(istream >> initCur)) {
            return EVENT_LOAD_ERROR;
        }
        cur = initCur;
        return EVENT_LOADED;
    };
    virtual event_access_t store(EventsOStream &ostream,
                                 EventsHistory *__marto_unused(hist)) {
        if (!(bool)(ostream << base)) {
            return EVENT_STORE_ERROR;
        }
        if (!(bool)(ostream << initCur)) {
            return EVENT_STORE_ERROR;
        }
        return EVENT_STORED;
    };
    virtual void setInitialStateFromCurrentState() { initCur = cur; };
    virtual double U01() { return (double)1 / (double)(base + cur++); };
    virtual double Uab(double inf, double sup) {
        return (inf + (sup - inf) * U01());
    };
    virtual long Iab(long min, long max) {
        return (long)(min + (base + cur) % (max + 1 - min));
    };
};

class RandomTestStreamGenerator : public RandomStreamGenerator {
    friend RandomTest;

  private:
    int base;
    int cur;

  protected:
    RandomTestStreamGenerator(int base) : base(base), cur(0){};

  public:
    virtual RandomStream *newRandomStream() {
        return new RandomTestStream(base + 1000 * cur++);
    };
    virtual void deleteRandomStream(RandomStream *rs) { delete rs; };
};

RandomTest::RandomTest() : RandomFabric() {}

RandomStreamGenerator *RandomTest::newRandomStreamGenerator() {
    return new RandomTestStreamGenerator(100000 * cur);
}

void RandomTest::deleteRandomStreamGenerator(RandomStreamGenerator *rsg) {
    delete rsg;
};
}
