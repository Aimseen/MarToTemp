/* -*-c++-*- C++ mode for emacs */
/***********************************************************************\
 *
 * File:           RandomDeterministic.h
 *
 * Fake random number generation library for MarTo
 *
\***********************************************************************/
#ifndef MARTO_RANDOM_DETERMINISTIC_H
#define MARTO_RANDOM_DETERMINISTIC_H

#include <marto/random.h>
#include <vector>

namespace marto {

/* forward declaration for friend */
class RandomDeterministicStreamGenerator;
/** \brief test class that whose streams will return sequences provided at
 * initialisation time
 */
class RandomDeterministic : public RandomFabric {
    friend RandomDeterministicStreamGenerator;

  private:
    std::vector<std::vector<double> *> *streams;

  public:
    ~RandomDeterministic(){};
    RandomDeterministic(std::vector<std::vector<double> *> *s) : streams(s){};
    virtual RandomHistoryStreamGenerator *newRandomStreamGenerator();
    virtual RandomHistoryStreamGenerator *
    newRandomStreamGenerator(HistoryIStream &istream);
};

} // namespace marto

#endif
