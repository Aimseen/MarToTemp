/***********************************************************************\
 *
 * File:           Random.h
 * 
 * Random number generation library for MarTo
 * 
\***********************************************************************/

/** For creating independent RNGs */ 
class InternalGenerator {
    friend std::ostream &operator <<(std::ostream &o, InternalGenerator &g);
    friend std::istream &operator >>(std::istream &i, InternalGenerator &g);
    template<typename T> Random<T> newUserGenerator(); 
    double nextU01();
  private: generateurLEcuyer;
  public:
    static InternalGenerator newStream();
}

/**  random number generation for the user 
* 
*/   
template<typename T>
class Random {
  public:
    T nextValue() = 0;
        
  protected:
  /* Lecuyer nous fournit le uniforme sur (0,1), même si on aimerait [0,1),
      il faut penser à le rendre accessible jusqu'ici */
    InternalGenerator *intGen;
}

/* Code To be moved to another internal header file : */
template<typename T>
class RandomUniformInterval : public Random<T> {
  private:
    double a,b;
  public:
    RandomUniformInterval(double a, double b);
}


/*  Code To move to C++ file */
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

    