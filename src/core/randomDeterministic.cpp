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
    RandomDeterministicStream(RandomDeterministicStreamGenerator *s, size_t sid)
        : sgen(s), stream_id(sid), next_pos(0), init_pos(0){};

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

double RandomDeterministicStream::U01() {
    return sgen->stream(stream_id)->at(next_pos++);
};

RandomStreamGenerator *RandomDeterministic::newRandomStreamGenerator() {
    return new RandomDeterministicStreamGenerator(streams);
}

void RandomDeterministic::deleteRandomStreamGenerator(
    RandomStreamGenerator *rsg) {
    delete rsg;
};
}; // namespace marto
