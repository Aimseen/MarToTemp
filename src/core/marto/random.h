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
- Get/WriteState ne travaille que sur Cg (WriteStateFull, verbeux travaille sur les 3)
- AdvanceState : semble être un déplacement dans le stream, non utilisé par PSI3, utile ?
- ResetStartStream utilisé par psi. Restaure Cg et Bg à partir de Cg, MAIS Get/WriteState ne travaille que sur Cg
- SetSeed : restaure la graine donnée dans Cg, Bg et Ig -> plutôt utiliser ça, non ?
- ResetNextSubStream : apparemment pour passer au substream suivant mais non utilisé dans psi
- Le constructeur semble créer un générateur sur le stream suivant

Objectif :
[ chunk d'evènements contenant dans evènements à nombre variable de paramètres (substreams) ]



*/

/**  internal random number generation 
*  Random is the core brick providing a uniform random number in (0,1).
*/   
class Random {
  public:
    static Random nextStream(); // creates a new independent RNG 
    /* Generic random is on (0,1)
       - one should specialize when forking is required
    */
    double next();
    size_t load(void *buffer);
    size_t store(void *buffer);
    template <typename T : 
        
  protected:
  /* Lecuyer nous fournit le uniforme sur (0,1), même si on aimerait [0,1),
      il faut penser à le rendre accessible jusqu'ici */
    RngStream *intGen;
}

/** Internal Random generator with uniform distribution; type double */
template<typename T>
class RandomUniformInterval : public Random<T> {
  private:
    double a,b;
  public:
    RandomUniformInterval(double a, double b);
}


/*  internal generic-type uniform generator */
template<typename T>
RandomUniformInterval<T>::RandomUniformInterval(double inf, double sup) {
    this.inf=inf;
    this.sup=sup;
}

template<typename T>
RandomUniformInterval<T>::nextValue() {
    return (inf+(sup-inf)*intGen->nextU01());
}

class InternalGeneratorFabric {
  // Should be a singleton because Lecuyer RngStream has to be initialized
}

    