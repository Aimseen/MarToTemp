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
    /** \brief forbid copy of this kind of objects */
    FormalParameterValues(const FormalParameterValues &) = delete;
    /** \brief forbid assignment of this kind of objects */
    FormalParameterValues &operator=(const FormalParameterValues &) = delete;

  public:
    FormalParameterValues(size_t l) : length(l){};
    virtual void generateParameterValues(ParameterValues *actualValues) = 0;
    virtual void store(EventsOStream &os, ParameterValues *actualValues) = 0;
    virtual void load(EventsIStream &is, ParameterValues *actualValues) = 0;
    virtual size_t sizeofValues() = 0;
    virtual void initParameterValues(ParameterValues* ep, Random *g);
  protected:
    size_t length;
};

template <typename T>
class FormalParameterValuesTyped : public FormalParameterValues {
  protected:
    FormalParameterValuesTyped(size_t l) : FormalParameterValues(l) {};
  public:
    typedef T Type;
    virtual size_t sizeofValues() { return sizeof(T); }
};

template <typename T>
class FormalConstantList : public FormalParameterValuesTyped<T> {
  public:
    FormalConstantList(size_t s, const std::vector<T> &values);
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
    /** \brief forbid copy of this kind of objects */
    ParameterValues(const ParameterValues &) = delete;
    /** \brief forbid assignment of this kind of objects */
    ParameterValues &operator=(const ParameterValues &) = delete;

  public:
    /** \brief create and initialize a ParameterValues
     *
     * Note: these objects are reused for different kind of event without reallocation
     */
    ParameterValues();
    
    /** \brief get an indexed element of type T from the buffer
     *
     * - only numeric types are supported (for now)
     * - it is an error to use 'get' with different type without calling 'reset' in between
     * - the index must be lesser than size()
     */
    template <typename T> T get(size_t index);

    size_t size();

    /** \brief cleanup all fields so that this object can be reused without reallocation
     *
     * Only the malloced buffer (and its size) are not resetted.
     */
    void reset() {
        kind=UNDEFINED;
        nbValues=0;
        g=nullptr;
        reference=nullptr;
        fp=nullptr;
    };

  private:
    template <typename T> friend class FormalConstantList;
    friend class ParametersBaseTest;
    friend class FormalParameterValues;
    enum {
        ARRAY,
        GENERATOR,
        REFERENCE,
        UNDEFINED
    } kind; // reference= formalconstantlist ; array can be either a list of
            // constants or a fixed list
    void *buffer; ///< adress of a malloced memory buffer. This object can realloc the buffer if required. This field is never null
    size_t bufferSize; ///< size of the malloced buffer
    FormalParameterValues *fp; ///< related FormalParameterValue
    size_t nbValues; ///< # objects cyrrently stored in the buffer
    Random *g; ///< # random object to use for this parameter if required
    ParameterValues *reference; ///< if kind is set to REFERENCE, delegate 'get' to this object
};
}

#ifndef MARTO_H
// In case of direct inclusion (instead of using <marto.h>),
// we try to include the implementation, hoping to avoid include loops
#include <marto/parameters-impl.h>
#endif

#endif
#endif
