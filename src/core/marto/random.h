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

/** This class formalize the interraction between Random* object and the history
 */
class RandomHistory {
  public:
    /** \brief replace the current state by the one from the history
     */
    virtual history_access_t load(HistoryIStream &istream) = 0;
    /** \brief store the marked state into the history
     *
     * by default, the initial state is marked
     */
    virtual history_access_t storeMarkedState(HistoryOStream &ostream) = 0;
    /** \brief mark the current state
     *
     * If the storeMarkedState() method is latter called, the current
     * (at the time of the markCurrentState() call) state will be
     * saved, not the future (at the time of storeMarkedState call)
     * state.
     */
    virtual void markCurrentState() = 0;
};

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
    /** \brief uniform random value in [0,1) */
    virtual double U01() = 0;
    /** \brief uniform random value in [inf,sup) */
    virtual double Uab(double inf, double sup) {
        return (inf + (sup - inf) * U01());
    };
    /** \brief uniform random integer value in [min,max] */
    virtual long Iab(long min, long max) { return (long)Uab(min, max + 1); };
};

/** \brief abstract class whose state can be load/stored from/to an history
 */
class RandomHistoryStream : public virtual RandomStream,
                            public virtual RandomHistory {};

/** \brief abstract class to obtain different (independant) random streams
 *
 * \note: there will be one RandomStreamGenerator per history chunk
 * The first stream will be used for classical event in the chunk
 * Next streams will be used for non-bounded parameters
 */
class RandomStreamGenerator : public virtual RandomHistory {
    /** \brief forbid copy of this kind of objects */
    RandomStreamGenerator(const RandomStreamGenerator &) = delete;
    /** \brief forbid assignment of this kind of objects */
    RandomStreamGenerator &operator=(const RandomStreamGenerator &) = delete;

  protected:
    RandomStreamGenerator(){};

  public:
    virtual ~RandomStreamGenerator(){};
    virtual RandomStream *newRandomStream() = 0;
    virtual RandomStream *newRandomStream(HistoryIStream &istream) = 0;
    virtual void deleteRandomStream(RandomStream *rs) { delete (rs); };
};

/** \brief same as RandomStreamGenerator but with streams history-aware
 *
 */
class RandomHistoryStreamGenerator : public RandomStreamGenerator {
    /** \brief forbid copy of this kind of objects */
    RandomHistoryStreamGenerator(const RandomHistoryStreamGenerator &) = delete;
    /** \brief forbid assignment of this kind of objects */
    RandomHistoryStreamGenerator &
    operator=(const RandomHistoryStreamGenerator &) = delete;

  protected:
    RandomHistoryStreamGenerator() : RandomStreamGenerator(){};

  public:
    virtual ~RandomHistoryStreamGenerator(){};
    virtual RandomHistoryStream *newRandomHistoryStream() = 0;
    virtual RandomHistoryStream *
    newRandomHistoryStream(HistoryIStream &istream) = 0;
    virtual void deleteRandomHistoryStream(RandomStream *rs) { delete (rs); };
    virtual RandomStream *newRandomStream() {
        return newRandomHistoryStream();
    };
    virtual RandomStream *newRandomStream(HistoryIStream &istream) {
        return newRandomHistoryStream(istream);
    };
    virtual void deleteRandomStream(RandomStream *rs) {
        deleteRandomHistoryStream(rs);
    };
};

/** \brief abstract class that must be provided to the config
 *
 * \note: this object can used in parallel by different threads
 * \note: this object should be implemented as a singleton (service common to
 * all threads to get their local RandomStreamGenerator
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
    virtual RandomHistoryStreamGenerator *newRandomStreamGenerator() = 0;
    virtual RandomHistoryStreamGenerator *
    newRandomStreamGenerator(HistoryIStream &istream) = 0;
    virtual void deleteRandomStreamGenerator(RandomStreamGenerator *rsg) {
        delete rsg;
    };
};

/** \brief multi-purpose object used for the generation of random values in the
 * current substream and the spawn of new generators on a new substream
 *
 * This class is for use inside MarTo itself, not for users in their
 * simulations. Random values are made available to users through the use of
 * variadic (or not) lists of values implemented by ParameterValues. This way,
 * random values provided to the users are properly saved and regenerated when
 * replaying.
 *
 * Objects of this class are not thread safe : they will be instantiated into a
 * local objet by each thread when simulating replaying. Other threads working
 * on the same chunk should have their own local copies based on the state saved
 * at the beginning of the chunk.
 */
class Random : public RandomHistoryStream, RandomStreamGenerator {
    /** \brief forbid copy of this kind of objects */
    Random(const Random &) = delete;
    /** \brief forbid assignment of this kind of objects */
    Random &operator=(const Random &) = delete;
    friend marto::Configuration;
    /** \brief build a Random object from a RandomFabric
     *
     * A RandomStreamGenerator is allocated and used internally.
     *
     * \param RandomFabric the fabric where to get a RandomStreamGenerator
     * object
     */
    Random(RandomFabric *fabric)
        : RandomHistoryStream(), RandomStreamGenerator() {
        rsg = fabric->newRandomStreamGenerator();
        crs = rsg->newRandomHistoryStream();
        markCurrentState();
    };
    Random(RandomFabric *fabric, HistoryIStream &istream)
        : RandomHistoryStream(), RandomStreamGenerator() {
        rsg = fabric->newRandomStreamGenerator(istream);
        crs = rsg->newRandomHistoryStream(istream);
    };
    ~Random() {
        delete crs;
        delete rsg;
    }

  public:
    RandomStream *currentRandomStream() { return crs; };
    virtual RandomStream *newRandomStream() { return rsg->newRandomStream(); };
    virtual RandomStream *newRandomStream(HistoryIStream &istream) {
        return rsg->newRandomStream(istream);
    };
    virtual void deleteRandomStream(RandomStream *rs) {
        rsg->deleteRandomStream(rs);
    };
    virtual history_access_t load(HistoryIStream &istream) {
        auto res = rsg->load(istream);
        if (res != HISTORY_DATA_LOADED) {
            return res;
        }
        return crs->load(istream);
    };
    virtual history_access_t storeMarkedState(HistoryOStream &ostream) {
        auto res = rsg->storeMarkedState(ostream);
        if (res != HISTORY_DATA_STORED) {
            return res;
        }
        return crs->storeMarkedState(ostream);
    };
    virtual void markCurrentState() {
        rsg->markCurrentState();
        crs->markCurrentState();
    };
    virtual double U01() { return crs->U01(); };
    virtual double Uab(double inf, double sup) { return crs->Uab(inf, sup); };
    virtual long Iab(long min, long max) { return crs->Iab(min, max); };

  private:
    RandomHistoryStream *crs;
    RandomHistoryStreamGenerator *rsg;
};
} // namespace marto

#endif
