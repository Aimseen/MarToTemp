/* -*-c++-*- C++ mode for emacs */
/***********************************************************************\
 *
 * File:           Random.h
 *
 * Random number generation library for MarTo
 *
\***********************************************************************/
/* Notes relatives au générateur de Lecuyer

Etat courant de PSI3 :
- pas de IncreasedPrec
- pas de SetAntithetic
- pas de AdvanceState

Questions :
- Get/WriteState ne travaille que sur Cg (WriteStateFull, verbeux travaille sur
les 3)
- AdvanceState : semble être un déplacement dans le stream, non utilisé par
PSI3, utile ?
- ResetStartStream utilisé par psi. Restaure Cg et Bg à partir de Ig, MAIS
Get/WriteState ne travaille que sur Cg
- SetSeed : restaure la graine donnée dans Cg, Bg et Ig -> plutôt utiliser ça,
non ?
- ResetNextSubStream : apparemment pour passer au substream suivant mais non
utilisé dans psi
- Le constructeur semble créer un générateur sur le stream suivant

Objective :
- Each chunk is associated with a different stream
- Each variadic parameter is associated with a different substream



*/
#ifndef MARTO_RANDOM_H
#define MARTO_RANDOM_H

#include <marto/RngStream.h>
#include <stddef.h>

namespace marto {

/**  internal random number generation
*  Random is the core brick providing a uniform random number in (0,1).
*
* Intended usage :
* - use all variants of next to advance in the current stream
* - to spawn a generator to the next substream from myGenerator :
*   Random newGenerator = new Random(myGenerator);
*   newGenerator.nextSubStream();
* - it's similar to spawn a generator to the next stream
*/
class Random {
    friend class Configuration;
    friend class eventsHistory;

  public:
    /** \brief creates a copy of a given generator */
    Random(const Random &original);
    /* Generic random is on (0,1)
       - one should specialize when forking is required
    */
    /** \brief advances this generator to the next substream */
    void nextSubStream();
    /** \brief resets to the begining of the current substream */
    void resetSubStream();
    /** \brief returns the next double in (0,1) */
    double next();
    /** \brief returns the next int in [i,j) */
    int next(int i, int j);
    /** \brief loads a full generator state */
    size_t load(void *buffer);
    /** \brief stores a full generator state */
    size_t store(void *buffer);
    /** \brief loads a stream and also sets it as current substream and current
     * position  */
    size_t loadStream(void *buffer);
    /** \brief stores the begining of the current stream */
    size_t storeStream(void *buffer);
    /** \brief loads a substream and also sets it as current position
        Warning : this assumes that the loaded substream is a substream of the
       current stream */
    size_t loadSubStream(void *buffer);
    /** \brief stores the begining of the current substream */
    size_t storeSubStream(void *buffer);

  protected:
    /* TODO : complete and give access to all this stuff from Config :
       access to distinct streams should be centralized because in the
       multithreaded versions, each new thread should be given a stream
       not already in use by another thread
    */
    /** \brief creates a new Random using a predefined seed */
    Random();
    /** \brief creates a new Random at the given seed */
    Random(unsigned long seed[]);
    /** \brief advances to the next stream */
    void nextStream();
    /** \brief resets to the begining of the current stream */
    void resetStream();
    /* Lecuyer nous fournit le uniforme sur (0,1), même si on aimerait [0,1),
       il faut penser à le rendre accessible jusqu'ici */
    struct RngStream_InfoState state;
};

/** Internal Random generator with uniform distribution; type double */
class RandomUniformInterval : public Random {
  private:
    double inf, sup;

  public:
    virtual double next();
    RandomUniformInterval(double inf, double sub);
};

class InternalGeneratorFabric {
    // Should be a singleton because Lecuyer RngStream has to be initialized
};
}

#endif
