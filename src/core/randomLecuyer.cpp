#include <marto/eventsHistory.h>
#include <marto/macros.h>
#include <marto/randomLecuyer.h>
/* We include the .c because we need the static constants it contains */
extern "C" {
#include <RngStream.c>
}
#include <string.h>

namespace marto {

class RandomLecuyerStream;
class RandomLecuyerStreamGenerator;
class RandomLecuyer;

class LecuyerState {
  private:
    struct RngStream_InfoState _state;

  protected:
    const RngStream g = &_state;

  private:
    void defaultSetup() { RngStream_IncreasedPrecis(g, 1); }

  public:
    LecuyerState(const unsigned long seed[6]) {
        // No using "RngStream_SetSeed(g, seed);"
        // as const is discarded...
        for (unsigned i = 0; i < 6; ++i) {
            g->Cg[i] = g->Bg[i] = /* g->Ig[i] =*/seed[i];
        }
        defaultSetup();
    };

    LecuyerState(const double seed[6]) {
        for (unsigned i = 0; i < 6; ++i) {
            /* No need to handle the start of Lecuyer Stream */
            g->Cg[i] = g->Bg[i] = /*g->Ig[i] =*/seed[i];
        }
        defaultSetup();
    };

    LecuyerState(const LecuyerState &ls) : _state(ls._state) {
        defaultSetup();
    };

    LecuyerState(const LecuyerState *const ls) : _state(ls->_state) {
        defaultSetup();
    };
};

/* We use some knowledge about the implementation from Lecuyer :
   - the begining of the current stream is in state.Ig
   - the begining of the current substream is in state.Bg
   - the current position is in state.Cg
   - a seed is an array of 6 double
*/

class RandomLecuyerStream : public RandomStream, protected LecuyerState {
    friend RandomLecuyerStreamGenerator;

  protected:
    RandomLecuyerStream(const RandomLecuyerStreamGenerator &rsg)
        : LecuyerState((LecuyerState *)&rsg) {
        for (unsigned i = 0; i < 6; ++i) {
            assert(g->Bg[i] == g->Cg[i]);
        }
    };

  public:
    virtual event_access_t load(EventsIStream &istream,
                                EventsHistory *__marto_unused(hist)) {
        for (unsigned i = 0; i < 6; ++i) {
            if (!(bool)(istream >> g->Bg[i])) {
                return EVENT_LOAD_ERROR;
            }
        }
        RngStream_ResetStartSubstream(g);
        return EVENT_LOADED;
    };
    virtual event_access_t store(EventsOStream &ostream,
                                 EventsHistory *__marto_unused(hist)) {
        for (unsigned i = 0; i < 6; ++i) {
            if (!(bool)(ostream << g->Bg[i])) {
                return EVENT_STORE_ERROR;
            }
        }
        return EVENT_STORED;
    };
    virtual void setInitialStateFromCurrentState() {
        for (unsigned i = 0; i < 6; ++i) {
            g->Bg[i] = g->Cg[i];
        }
    };
    virtual double U01() { return RngStream_RandU01(g); };
    virtual long Iab(long min, long max) {
        return RngStream_RandInt(g, min, max);
    };
};

class RandomLecuyerStreamGenerator : public RandomStreamGenerator,
                                     protected LecuyerState {
    friend RandomLecuyer;

  protected:
    RandomLecuyerStreamGenerator(const RandomLecuyer &rf)
        : LecuyerState(rf.nextSeed){};

  public:
    virtual RandomLecuyerStream *newRandomStream() {
        RandomLecuyerStream *rs = new RandomLecuyerStream(*this);
        RngStream_ResetNextSubstream(g);
        return rs;
    };
    virtual void deleteRandomStream(RandomStream *rs) { delete rs; };
    virtual event_access_t load(EventsIStream &istream,
                                EventsHistory *__marto_unused(hist)) {
        for (unsigned i = 0; i < 6; ++i) {
            if (!(bool)(istream >> g->Bg[i])) {
                return EVENT_LOAD_ERROR;
            }
        }
        return EVENT_LOADED;
    };
    virtual event_access_t store(EventsOStream &ostream,
                                 EventsHistory *__marto_unused(hist)) {
        for (unsigned i = 0; i < 6; ++i) {
            if (!(bool)(ostream << g->Bg[i])) {
                return EVENT_STORE_ERROR;
            }
        }
        return EVENT_STORED;
    };
};

void RandomLecuyer::LecuyerAdvanceStream() {
    /* Taken from Lecuyer implementation (see RngStream_CreateStream in
       RngStream.c).
       Extracted because it is not separated from memory allocation */
    MatVecModM(A1p127, nextSeed, nextSeed, m1);
    MatVecModM(A2p127, &nextSeed[3], &nextSeed[3], m2);
}

RandomStreamGenerator *RandomLecuyer::newRandomStreamGenerator() {
    RandomLecuyerStreamGenerator *rsg = new RandomLecuyerStreamGenerator(*this);
    LecuyerAdvanceStream();
    return rsg;
}

void RandomLecuyer::deleteRandomStreamGenerator(RandomStreamGenerator *rsg) {
    delete rsg;
}
};
