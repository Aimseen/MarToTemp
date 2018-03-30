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

enum ParamType {
    QueueList,
    IntList,
    DoubleList,
    // Martostring,
    InvalidType
};

/** Parameters coming from the model description
 *
 * type is used to compact the data if needed
 * l is the maximum number of actual parameters that can be extracted. 0 means no limit. 
 */
class FormalParameterValues {
public:
    FormalParameterValues(ParamType type, size_t l) : paramType(type), length(l) {};
    virtual void generateParameterValues(ParameterValues *actualValues) = 0;
    virtual void store(EventsOStream &os, ParameterValues*actualValues) = 0;
    virtual void load(EventsIStream &is, ParameterValues*actualValues) = 0;
private:
    ParamType paramType;
    size_t length;
};

class FormalConstantList:public FormalParameterValues {
public:
    template<typename T>
    FormalConstantList(ParamType type, size_t s, std::vector<T> *values);
    virtual void generateParameterValues(ParameterValues *actualValues __attribute__((unused))) {};
    /* store nothing, all is constant */
    virtual void store(EventsOStream &os __attribute__((unused)),
                       ParameterValues*actualValues __attribute__((unused))) {};
    /* nothing to load, all is constant */
    virtual void load(EventsIStream &is __attribute__((unused)),
                      ParameterValues*actualValues __attribute__((unused))) {};
private:
    ParameterValues *values;
};

class FormalDistribution : public FormalParameterValues {
public:
    FormalDistribution(ParamType type, size_t l, Random *rand);
private:
    Random *rand;
};
/* when the event is created the values are generated and stored for future direct use at replay */
class FormalDistributionFixedList:public FormalDistribution {
public:
    FormalDistributionFixedList(ParamType type, size_t l, string idRandom);
};

/* the event only stores the generator : values are re-generated at every replay to save buffer space 
    some long fixed lists could be more optimally stored using this generator.
*/ 
class FormalDistributionVariadicList:public FormalDistribution {
public:
    FormalDistributionVariadicList(ParamType type, size_t l, string idRandom);
};

/* list of values for this parameter (for instance : input queues) */
class ParameterValues {
public:
    // This is not an abstract class because we do not want to allocate
    // a new object for each parameter list of each event generated.
    // Instead, the parameter class is a placeholder wich can store either
    // an array of values or a sequence generated from a given generator seed.
    //
    // In the case of an array, this assumes that the user always asks for the
    // same value type and does not exceed the size she has declared in
    // the configuration.
    template < typename T > T get(unsigned int index);
    size_t size();
private:
    friend class FormalConstantList;
    friend class ParametersBaseTest;
    ParameterValues();
    template < typename T > ParameterValues(T *vals, size_t nb);
    enum { ARRAY, GENERATOR, REFERENCE, UNDEFINED } kind; // reference= formalconstantlist ; array can be either a list of constants or a fixed list
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
