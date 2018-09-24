/* -*-c++-*- C++ mode for emacs */
/***********************************************************************\
 *
 * File:           RandomLecuyer.h
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


*/
#ifndef MARTO_RANDOM_LECUYER_H
#define MARTO_RANDOM_LECUYER_H

#include <marto/random.h>

namespace marto {

class RandomLecuyerStreamGenerator;

/** \brief Random fabric based on Lecuyer random generator
 */
class RandomLecuyer : public RandomFabric {
    friend RandomLecuyerStreamGenerator;

  protected:
    double nextSeed[6];

  protected:
    void LecuyerAdvanceStream();

  public:
    virtual ~RandomLecuyer(){};
    virtual RandomStreamGenerator *newRandomStreamGenerator();
    virtual void deleteRandomStreamGenerator(RandomStreamGenerator *rsg);
};
} // namespace marto

#endif
