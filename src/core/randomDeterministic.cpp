#include <marto.h>
#include <marto/randomDeterministic.h>

namespace marto {
class RandomDeterministicStreamGenerator;

class RandomDeterministicStream : public RandomStream {
    friend RandomDeterministicStreamGenerator;

  private:
    RandomDeterministicStreamGenerator *sgen;
    size_t stream_id;
    size_t next_pos;
    size_t init_pos;

  protected:
    RandomDeterministicStream(RandomDeterministicStreamGenerator *s,
                              size_t sid);

  public:
    virtual event_access_t load(EventsIStream &istream,
                                EventsHistory *__marto_unused(hist)) {
        if (!(bool)(istream >> stream_id)) {
            return EVENT_LOAD_ERROR;
        }
        if (!(bool)(istream >> init_pos)) {
            return EVENT_LOAD_ERROR;
        }
        next_pos = init_pos;
        return EVENT_LOADED;
    };
    virtual event_access_t store(EventsOStream &ostream,
                                 EventsHistory *__marto_unused(hist)) {
        if (!(bool)(ostream << stream_id)) {
            return EVENT_STORE_ERROR;
        }
        if (!(bool)(ostream << init_pos)) {
            return EVENT_STORE_ERROR;
        }
        return EVENT_STORED;
    };
    virtual void setInitialStateFromCurrentState() { init_pos = next_pos; };
    virtual double U01();
    virtual double Uab(double inf, double sup);
    virtual long Iab(long min, long max);
};

class RandomDeterministicStreamGenerator : public RandomStreamGenerator {
    friend RandomDeterministicStream;
    friend RandomDeterministic;

  private:
    std::vector<std::vector<double> *> *_streams;
    size_t next_stream_id;

  protected:
    RandomDeterministicStreamGenerator(std::vector<std::vector<double> *> *s)
        : _streams(s), next_stream_id(0){};
    std::vector<double> *stream(size_t s_id) { return _streams->at(s_id); };

  public:
    virtual RandomStream *newRandomStream() {
        return new RandomDeterministicStream(this, next_stream_id++);
    };
    virtual void deleteRandomStream(RandomStream *rs) { delete rs; };
};

RandomDeterministicStream::RandomDeterministicStream(
    RandomDeterministicStreamGenerator *s, size_t sid)
    : sgen(s), stream_id(sid), next_pos(0), init_pos(0) {
    sgen->stream(stream_id);
};

double RandomDeterministicStream::U01() {
    double value = sgen->stream(stream_id)->at(next_pos++);
    if (!(value < 1 && value >= 0)) {
        throw std::out_of_range(((std::string) __FUNCTION__) +
                                ": invalid programmed random value: " +
                                std::to_string(value) + " is not in [0;1)");
    }
    return value;
};

double RandomDeterministicStream::Uab(double inf, double sup) {
    double value = sgen->stream(stream_id)->at(next_pos++);
    if (!(value < sup && value >= inf)) {
        throw std::out_of_range(
            ((std::string) __FUNCTION__) +
            ": invalid programmed random value: " + std::to_string(value) +
            " is not in [" + std::to_string(inf) + ";" + std::to_string(sup) +
            ")");
    }
    return value;
};

long RandomDeterministicStream::Iab(long min, long max) {
    long value = sgen->stream(stream_id)->at(next_pos++);
    if (!(value <= max && value >= min)) {
        throw std::out_of_range(((std::string) __FUNCTION__) +
                                ": invalid programmed random value " +
                                std::to_string(value) + " is not in [" +
                                std::to_string(min) + ";" +
                                std::to_string(max) + "]");
    }
    return value;
};

RandomStreamGenerator *RandomDeterministic::newRandomStreamGenerator() {
    return new RandomDeterministicStreamGenerator(streams);
}

void RandomDeterministic::deleteRandomStreamGenerator(
    RandomStreamGenerator *rsg) {
    delete rsg;
};
}; // namespace marto
