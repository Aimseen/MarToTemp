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

/** For creating independent RNGs */
class InternalGenerator {
    friend std::ostream & operator <<(std::ostream & o, InternalGenerator & g);
    friend std::istream & operator >>(std::istream & i, InternalGenerator & g);
     template < typename T > Random < T > newUserGenerator();
    double nextU01();
  private: generateurLEcuyer;
  public:
    static InternalGenerator newStream();
}
/**  random number generation for the user 
* 
*/ template < typename T >
 class Random {
  public:
    T nextValue() = 0;

  protected:
    /* Lecuyer nous fournit le uniforme sur (0,1), même si on aimerait [0,1),
       il faut penser à le rendre accessible jusqu'ici */
    InternalGenerator * intGen;
}

/* Code To be moved to another internal header file : */
template < typename T > class RandomUniformInterval:public Random < T > {
  private:
    double a, b;
  public:
    RandomUniformInterval(double a, double b);
}


/*  Code To move to C++ file */
template < typename T > RandomUniformInterval < T >::RandomUniformInterval(double inf, double sup) {
    this.inf = inf;
    this.sup = sup;
}

template < typename T > RandomUniformInterval < T >::nextValue() {
    return (inf + (sup - inf) * intGen->nextU01());
}

class InternalGeneratorFabric {
    // Should be a singleton because Lecuyer RngStream has to be initialized
}
