/* -*-c++-*- C++ mode for emacs */
/***********************************************************************\
 *
 * File:           Random.h
 *
 * Random number generation library for MarTo
 *
\***********************************************************************/
/*
Objective :
- Each chunk is associated with a different stream
- Each variadic parameter is associated with a different stream
*/
#ifndef MARTO_RANDOM_H
#define MARTO_RANDOM_H

#include <marto/types.h>
#include <stddef.h>

namespace marto {

/** \brief abstract class to obtain a serie of random numbers
 *
 * The initial state can be saved/restored into/from an history
 *
 * Default methods are provided to transform the (0,1) interval
 * into other kind of intervals. They can be used or reimplemented.
 */
class RandomStream {
    /** \brief forbid copy of this kind of objects */
    RandomStream(const RandomStream &) = delete;
    /** \brief forbid assignment of this kind of objects */
    RandomStream &operator=(const RandomStream &) = delete;

  protected:
    RandomStream(){};

  public:
    virtual ~RandomStream(){};
    // method to load/save the initial internal state into an history
    // in order to replay the whole stream
    virtual event_access_t load(EventsIStream &istream,
                                EventsHistory *hist) = 0;
    virtual event_access_t store(EventsOStream &ostream,
                                 EventsHistory *hist) = 0;
    /** \brief define the current state as the initial one
     *
     * If the store() method is latter called, the current (at the
     * time of the setInitialStateFromCurrentState call) state will be
     * saved, not the future (at the time of store call) state.
     */
    virtual void setInitialStateFromCurrentState() = 0;
    /** \brief uniform random value in [0,1) */
    virtual double U01() = 0;
    /** \brief uniform random value in [inf,sup) */
    virtual double Uab(double inf, double sup) {
        return (inf + (sup - inf) * U01());
    };
    /** \brief uniform random integer value in [min,max] */
    virtual long Iab(long min, long max) { return (long)Uab(min, max + 1); };
};

/** \brief abstract class to obtain different (independant) random streams
 *
 * \note: there will be one RandomStreamGenerator per history chunk
 * The first stream will be used for classical event in the chunk
 * Next streams will be used for non-bounded parameters
 */
class RandomStreamGenerator {
    /** \brief forbid copy of this kind of objects */
    RandomStreamGenerator(const RandomStreamGenerator &) = delete;
    /** \brief forbid assignment of this kind of objects */
    RandomStreamGenerator &operator=(const RandomStreamGenerator &) = delete;

  protected:
    RandomStreamGenerator(){};

  public:
    virtual ~RandomStreamGenerator(){};
    virtual RandomStream *newRandomStream() = 0;
    virtual void deleteRandomStream(RandomStream *rs) = 0;
#if 0
    // method to load/save the internal state into an history
    virtual event_access_t load(EventsIStream &istream, EventsHistory *hist) = 0;
    virtual event_access_t store(EventsOStream &ostream, EventsHistory *hist) = 0;
#endif
};

/** \brief abstract class that must be provided to the config
 *
 * \note: this object can used in parallel by different threads
 */
class RandomFabric {
    /** \brief forbid copy of this kind of objects */
    RandomFabric(const RandomFabric &) = delete;
    /** \brief forbid assignment of this kind of objects */
    RandomFabric &operator=(const RandomFabric &) = delete;

  protected:
    RandomFabric(){};

  public:
    virtual ~RandomFabric(){};
    virtual RandomStreamGenerator *newRandomStreamGenerator() = 0;
    virtual void deleteRandomStreamGenerator(RandomStreamGenerator *rsg) = 0;
};

// The comment below seems wrong : we decided to give ParameterValues as the
// only interface to
// access a random stream. The user should have access to any random at all.
// We do this to avoid misuse of the generators : the ParameterValues we provide
// are saved into the
// history and reconstructed correctly when replaying.
//
// must be used only by one thread at any time
// will be provided when generating events
// it should be the only object that the user will deal with
class Random : public RandomStream, RandomStreamGenerator {
    /** \brief forbid copy of this kind of objects */
    Random(const Random &) = delete;
    /** \brief forbid assignment of this kind of objects */
    Random &operator=(const Random &) = delete;
    /** \brief constructor */
    Random(RandomStreamGenerator *v_rsg)
        : RandomStream(), RandomStreamGenerator(), rsg(v_rsg) {
        crs = newRandomStream();
    };
    ~Random() { rsg->deleteRandomStream(crs); }

  public:
    RandomStream *currentRandomStream() { return crs; };
    virtual RandomStream *newRandomStream() { return rsg->newRandomStream(); };
    virtual void deleteRandomStream(RandomStream *rs) {
        rsg->deleteRandomStream(rs);
    };
    virtual event_access_t load(EventsIStream &istream, EventsHistory *hist) {
        return crs->load(istream, hist);
    };
    virtual event_access_t store(EventsOStream &ostream, EventsHistory *hist) {
        return crs->store(ostream, hist);
    };
    virtual double U01() { return crs->U01(); };
    virtual double Uab(double inf, double sup) { return crs->Uab(inf, sup); };
    virtual long Iab(long min, long max) { return crs->Iab(min, max); };

  private:
    RandomStream *crs;
    RandomStreamGenerator *rsg;
};
} // namespace marto

#endif
