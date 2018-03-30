/* -*-c++-*- C++ mode for emacs */
/* Parameters management */
#ifndef MARTO_PARAMETERS_H
#define MARTO_PARAMETERS_H

#ifdef __cplusplus

#include <marto/forwardDecl.h>
#include <marto/random.h>
#include <ostream>
#include <string>

// convolution to keep operator<< in global namespace
// See
// https://stackoverflow.com/questions/38801608/friend-functions-and-namespaces
using std::ostream;
ostream &operator<<(ostream &out, const marto::FormalParameterValues &ev);

namespace marto {

using std::string;

typedef uint32_t Queue;

/** Parameters coming from the model description
 *
 * \param l is the maximum number of actual parameters that can be extracted.
 *   0 means no limit.
 *
 * FormalParameterValues is a generic (not templated) type. It is the only
 * info we have at runtime in the core code.
 */
class FormalParameterValues {
  public:
    FormalParameterValues(size_t l) : length(l){};
    virtual void generateParameterValues(ParameterValues *actualValues) = 0;
    virtual void store(EventsOStream &os, ParameterValues *actualValues) = 0;
    virtual void load(EventsIStream &is, ParameterValues *actualValues) = 0;
    virtual size_t sizeofValues() = 0;
  protected:
    size_t length;
};

template <typename T>
class FormalParameterValuesTyped : public FormalParameterValues {
  public:
    typedef T Type;
    virtual size_t sizeofValues() { return sizeof(T); }
};

template <typename T>
class FormalConstantList : public FormalParameterValuesTyped<T> {
  public:
    FormalConstantList(size_t s, std::vector<T> *values);
    virtual void generateParameterValues(ParameterValues *actualValues
                                         __attribute__((unused))){};
    /* store nothing, all is constant */
    virtual void store(EventsOStream &os __attribute__((unused)),
                       ParameterValues *actualValues __attribute__((unused))){};
    /* nothing to load, all is constant */
    virtual void load(EventsIStream &is __attribute__((unused)),
                      ParameterValues *actualValues __attribute__((unused))){};

  private:
    ParameterValues *values;
};

template <typename T>
class FormalDistribution : public FormalParameterValuesTyped<T> {
  public:
    FormalDistribution(size_t l, Random *rand);

  protected:
    Random *rand;
};

/* when the event is created the values are generated and stored for future
 * direct use at replay */
template <typename T>
class FormalDistributionFixedList : public FormalDistribution<T> {
  public:
    FormalDistributionFixedList(size_t l, string idRandom);
};

/* the event only stores the generator : values are re-generated at every replay
   to save buffer space
    some long fixed lists could be more optimally stored using this generator.
*/
template <typename T>
class FormalDistributionVariadicList : public FormalDistribution<T> {
  public:
    FormalDistributionVariadicList(size_t l, string idRandom);
};

/** \brief list of values for a parameter (for instance : input queues)
 *
 * This is not an abstract class because we do not want to allocate
 * a new object for each parameter list of each event generated.
 * Instead, the parameter class is a placeholder wich can store either
 * an array of values or a sequence generated from a given generator seed.
 *
 * In the case of an array, this assumes that the user always asks for the
 * same value type and does not exceed the size she has declared in
 * the configuration.
 *
 * The difficulty is that we loose the type information from the formal
 * parameter and we assume the user won't mix type between the declared
 * formal parameter and the use of this parameter (mainly in the
 * transition code)
 */
class ParameterValues {
  public:
    template <typename T> T get(unsigned int index);
    size_t size();

  private:
    template <typename T> friend class FormalConstantList;
    friend class ParametersBaseTest;
    ParameterValues();
    template <typename T> ParameterValues(T *vals, size_t nb);
    enum {
        ARRAY,
        GENERATOR,
        REFERENCE,
        UNDEFINED
    } kind; // reference= formalconstantlist ; array can be either a list of
            // constants or a fixed list
    void *buffer;
    size_t bufferSize;
    size_t nbValues;
    Random g;
    ParameterValues *reference;
};
}

#ifndef MARTO_H
// In case of direct inclusion (instead of using <marto.h>),
// we try to include the implementation, hoping to avoid include loops
#include <marto/parameters-impl.h>
#endif

#endif
#endif
