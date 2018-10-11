#include <marto.h>
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

typedef enum {
    LECUYER_STATE_STREAM,
    LECUYER_STATE_STREAM_GENERATOR,
} LecuyerStateKind;

/* We use some knowledge about the implementation from Lecuyer :
   - the begining of the current stream is in state.Ig
   - the begining of the current substream is in state.Bg
   - the current position is in state.Cg
   - a seed is an array of 6 double
*/

/* **************************************************************
 * LecuyerState{,History}
 */

/** class used to manage a state of a Lecuyer random generator
 *
 * For our need, we only use a small part of the feature of the
 * original Lecuyer random implementation. In particular:
 * - Lecuyer streams are only created in a RandomLecuyer object (ie
 *   RandomFabric)
 * - Lecuyer substreams are only created in a RandomLecuyerStreamGenerator
 *   object
 * - Lecuyer random values are only created in a RandomLecuyerStream object
 * - a RandomLecuyerStreamGenerator uses only one Lecuyer stream
 * - a RandomLecuyerStream manages uses only one Lecuyer substream
 *
 * The LecuyerState is used by both RandomLecuyerStreamGenerator and
 * RandomLecuyerStream, but the useful part of the state is not the
 * same in both cases, so this class is parametrized by this information.
 */
template <LecuyerStateKind kind> class LecuyerState {
  private:
    struct RngStream_InfoState _state;

  protected:
    const RngStream g = &_state;

    /** pointer on the useful part of the state
     *
     * A RandomStreamGenerator will only use Bg
     * A RandomStream will only use Cg
     *
     * Note: this variable should be private in LecuyerStateHistory
     * but due to an internal GCC error on gcc 8.2.0, it is moved here
     * as a workaround.
     */
    double *const useful_state =
        (kind == LECUYER_STATE_STREAM)
            ? this->g->Cg
            : (kind == LECUYER_STATE_STREAM_GENERATOR) ? this->g->Bg : nullptr;

    void defaultSetup() { RngStream_IncreasedPrecis(g, 1); }

  public:
    /** Constructor used when creating a RandomLecuyerStreamGenerator
     *
     * It use the RandomFabric 'RandomLecuyer' to get the initial seed
     */
    LecuyerState(const RandomLecuyer &rf) {
        for (unsigned i = 0; i < 6; ++i) {
            /* No need to handle the start of Lecuyer Stream */
            this->g->Cg[i] = this->g->Bg[i] = /*g->Ig[i] =*/rf.nextSeed[i];
        }
        defaultSetup();
    };

    /** Constructor used when creating a RandomLecuyerStream
     *
     * The state is duplicated from the RandomLecuyerStreamGenerator state
     */
    LecuyerState(const RandomLecuyerStreamGenerator &rsg);

  protected:
    /** Used by the LecuyerStateHistory(istream) constructor
     *
     * An default constructor would also solve the issue but it is
     * better to avoid it, to be sure we correctly initialize this
     * state
     */
    LecuyerState(HistoryIStream &__marto_unused(istream)){};
};

template <LecuyerStateKind kind>
class LecuyerStateHistory : public virtual LecuyerState<kind>,
                            public virtual RandomHistory {
  private:
    double marked_state[6];

  public:
    /** Constructor used when creating a RandomLecuyerStreamGenerator
     *
     * It use the RandomFabric 'RandomLecuyer' to get the initial seed
     */
    LecuyerStateHistory(const RandomLecuyer &rf) : LecuyerState<kind>(rf) {
        markCurrentState();
    };

    /** Constructor used when creating a RandomLecuyerHistoryStream
     *
     * The state is duplicated from the RandomLecuyerStreamGenerator state
     */
    LecuyerStateHistory(const RandomLecuyerStreamGenerator &rsg)
        : LecuyerState<kind>(rsg) {
        markCurrentState();
    };

    /** Constructor used when creating a RandomLecuyerHistoryStream(Generator)
     * from an history
     *
     */
    LecuyerStateHistory(HistoryIStream &istream) : LecuyerState<kind>(istream) {
        load(istream);
        this->defaultSetup();
    };

    virtual history_access_t load(HistoryIStream &istream) {
        for (unsigned i = 0; i < 6; ++i) {
            if (!(bool)(istream >> this->useful_state[i])) {
                return HISTORY_DATA_LOAD_ERROR;
            }
        }
        return HISTORY_DATA_LOADED;
    }
    virtual history_access_t storeMarkedState(HistoryOStream &ostream) {
        for (unsigned i = 0; i < 6; ++i) {
            if (!(bool)(ostream << marked_state[i])) {
                return HISTORY_DATA_STORE_ERROR;
            }
        }
        return HISTORY_DATA_STORED;
    };
    virtual void markCurrentState() {
        for (unsigned i = 0; i < 6; ++i) {
            marked_state[i] = this->useful_state[i];
        }
    };
};

/* **************************************************************
 * RandomLecuyer{,History}Stream
 */

class RandomLecuyerStream
    : public virtual RandomStream,
      protected virtual LecuyerState<LECUYER_STATE_STREAM> {
    friend RandomLecuyerStreamGenerator;

  protected:
    RandomLecuyerStream(const RandomLecuyerStreamGenerator &rsg)
        : LecuyerState<LECUYER_STATE_STREAM>(rsg) {
        for (unsigned i = 0; i < 6; ++i) {
            assert(g->Bg[i] == g->Cg[i]);
        }
    };

  public:
    virtual double U01() { return RngStream_RandU01(g); };
    virtual long Iab(long min, long max) {
        return RngStream_RandInt(g, min, max);
    };
};

class RandomLecuyerHistoryStream
    : public RandomHistoryStream,
      public RandomLecuyerStream,
      protected LecuyerStateHistory<LECUYER_STATE_STREAM> {
    friend RandomLecuyerStreamGenerator;

  protected:
    RandomLecuyerHistoryStream(const RandomLecuyerStreamGenerator &rsg)
        : LecuyerState<LECUYER_STATE_STREAM>(rsg), RandomHistoryStream(),
          RandomLecuyerStream(rsg), LecuyerStateHistory<LECUYER_STATE_STREAM>(
                                        rsg){};

  public:
    virtual history_access_t load(HistoryIStream &istream) {
        auto res = LecuyerStateHistory<LECUYER_STATE_STREAM>::load(istream);
        RngStream_ResetStartSubstream(g);
        return res;
    };
};

/* **************************************************************
 * RandomLecuyerStreamGenerator
 */

class RandomLecuyerStreamGenerator
    : public RandomHistoryStreamGenerator,
      protected LecuyerStateHistory<LECUYER_STATE_STREAM_GENERATOR> {
    friend RandomLecuyer; // for constructor access

  protected:
    RandomLecuyerStreamGenerator(const RandomLecuyer &rf)
        : LecuyerState<LECUYER_STATE_STREAM_GENERATOR>(rf),
          LecuyerStateHistory<LECUYER_STATE_STREAM_GENERATOR>(rf){};

    RandomLecuyerStreamGenerator(HistoryIStream &istream)
        : LecuyerState<LECUYER_STATE_STREAM_GENERATOR>(istream),
          LecuyerStateHistory<LECUYER_STATE_STREAM_GENERATOR>(istream){};

  public:
    virtual RandomLecuyerHistoryStream *newRandomHistoryStream() {
        RandomLecuyerHistoryStream *rs = new RandomLecuyerHistoryStream(*this);
        RngStream_ResetNextSubstream(g);
        return rs;
    };
    virtual RandomLecuyerHistoryStream *
    newRandomHistoryStream(HistoryIStream &istream) {
        RandomLecuyerHistoryStream *rs =
            new RandomLecuyerHistoryStream(istream);
        RngStream_ResetNextSubstream(g);
        return rs;
    };
    virtual RandomLecuyerStream *newRandomStream() {
        RandomLecuyerStream *rs = new RandomLecuyerStream(*this);
        RngStream_ResetNextSubstream(g);
        return rs;
    };
    virtual RandomLecuyerStream *newRandomStream(HistoryIStream &istream) {
        RandomLecuyerStream *rs = new RandomLecuyerStream(istream);
        RngStream_ResetNextSubstream(g);
        return rs;
    };
    virtual void deleteRandomStream(RandomStream *rs) { delete rs; };

    const double *Cg() const { return g->Cg; };
};

/* **************************************************************
 * Methods implementation
 */

template <LecuyerStateKind kind>
LecuyerState<kind>::LecuyerState(const RandomLecuyerStreamGenerator &rsg) {
    for (unsigned i = 0; i < 6; ++i) {
        g->Cg[i] = rsg.Cg()[i];
    }
    defaultSetup();
};

void RandomLecuyer::LecuyerAdvanceStream() {
    /* Taken from Lecuyer implementation (see RngStream_CreateStream in
       RngStream.c).
       Extracted because it is not separated from memory allocation */
    MatVecModM(A1p127, nextSeed, nextSeed, m1);
    MatVecModM(A2p127, &nextSeed[3], &nextSeed[3], m2);
}

RandomHistoryStreamGenerator *RandomLecuyer::newRandomStreamGenerator() {
    RandomLecuyerStreamGenerator *rsg = new RandomLecuyerStreamGenerator(*this);
    LecuyerAdvanceStream();
    return rsg;
}

RandomHistoryStreamGenerator *
RandomLecuyer::newRandomStreamGenerator(HistoryIStream &istream) {
    RandomLecuyerStreamGenerator *rsg =
        new RandomLecuyerStreamGenerator(istream);
    return rsg;
}

void RandomLecuyer::deleteRandomStreamGenerator(RandomStreamGenerator *rsg) {
    delete rsg;
}
}; // namespace marto
