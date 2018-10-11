#include <marto.h>
#include <marto/randomDeterministic.h>

namespace marto {
class RandomDeterministicStreamGenerator;

class RandomDeterministicStream : public RandomHistoryStream {
    friend RandomDeterministicStreamGenerator;

  private:
    RandomDeterministicStreamGenerator *sgen;
    size_t stream_id;
    size_t next_pos;
    size_t marked_pos;

  protected:
    RandomDeterministicStream(RandomDeterministicStreamGenerator *s,
                              size_t sid);
    RandomDeterministicStream(RandomDeterministicStreamGenerator *s,
                              HistoryIStream &istream);

  public:
    virtual history_access_t load(HistoryIStream &istream);
    virtual history_access_t storeMarkedState(HistoryOStream &ostream);
    virtual void markCurrentState() { marked_pos = next_pos; };
    virtual double U01();
    virtual double Uab(double inf, double sup);
    virtual long Iab(long min, long max);
};

class RandomDeterministicStreamGenerator : public RandomHistoryStreamGenerator {
    friend RandomDeterministicStream;
    friend RandomDeterministic;

  private:
    std::vector<std::vector<double> *> *_streams;
    size_t next_stream_id;
    size_t marked_next_stream_id;

  protected:
    RandomDeterministicStreamGenerator(std::vector<std::vector<double> *> *s)
        : _streams(s), next_stream_id(0) {
        markCurrentState();
    };
    RandomDeterministicStreamGenerator(std::vector<std::vector<double> *> *s,
                                       HistoryIStream &istream)
        : _streams(s) {
        load(istream);
    };
    std::vector<double> *stream(size_t s_id) { return _streams->at(s_id); };

  public:
    virtual RandomHistoryStream *newRandomHistoryStream() {
        return new RandomDeterministicStream(this, next_stream_id++);
    };
    virtual RandomHistoryStream *
    newRandomHistoryStream(HistoryIStream &istream) {
        return new RandomDeterministicStream(this, istream);
    };
    virtual history_access_t load(HistoryIStream &istream) {
        if (!(bool)(istream >> next_stream_id)) {
            return HISTORY_DATA_LOAD_ERROR;
        }
        markCurrentState();
        return HISTORY_DATA_LOADED;
    };
    virtual history_access_t storeMarkedState(HistoryOStream &ostream) {
        if (!(bool)(ostream << marked_next_stream_id)) {
            return HISTORY_DATA_STORE_ERROR;
        }
        return HISTORY_DATA_STORED;
    };
    virtual void markCurrentState() { marked_next_stream_id = next_stream_id; };
};

RandomDeterministicStream::RandomDeterministicStream(
    RandomDeterministicStreamGenerator *s, size_t sid)
    : sgen(s), stream_id(sid), next_pos(0) {
    sgen->stream(stream_id);
    markCurrentState();
};

RandomDeterministicStream::RandomDeterministicStream(
    RandomDeterministicStreamGenerator *s, HistoryIStream &istream)
    : sgen(s), stream_id(0), next_pos(0) {
    load(istream);
    markCurrentState();
};

history_access_t RandomDeterministicStream::load(HistoryIStream &istream) {
    if (!(bool)(istream >> stream_id)) {
        return HISTORY_DATA_LOAD_ERROR;
    }
    sgen->stream(stream_id);
    if (!(bool)(istream >> marked_pos)) {
        return HISTORY_DATA_LOAD_ERROR;
    }
    markCurrentState();
    return HISTORY_DATA_LOADED;
};

history_access_t
RandomDeterministicStream::storeMarkedState(HistoryOStream &ostream) {
    if (!(bool)(ostream << stream_id)) {
        return HISTORY_DATA_STORE_ERROR;
    }
    if (!(bool)(ostream << marked_pos)) {
        return HISTORY_DATA_STORE_ERROR;
    }
    return HISTORY_DATA_STORED;
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

RandomHistoryStreamGenerator *RandomDeterministic::newRandomStreamGenerator() {
    return new RandomDeterministicStreamGenerator(streams);
}

RandomHistoryStreamGenerator *
RandomDeterministic::newRandomStreamGenerator(HistoryIStream &istream) {
    return new RandomDeterministicStreamGenerator(streams, istream);
}

}; // namespace marto
