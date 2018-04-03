/* -*-c++-*- C++ mode for emacs */
/* Parameters management */
#ifndef MARTO_PARAMETERS_H
#define MARTO_PARAMETERS_H

#ifdef __cplusplus

#include <marto/forwardDecl.h>
#include <marto/macros.h>
#include <marto/random.h>
#include <ostream>
#include <string>
#include <typeinfo>

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
    template <typename T>
    FormalParameterValues(size_t l, T *__marto_unused(unused))
        : length(l), sizeofValue(sizeof(T)), typeinfoValue(typeid(T)) {}

    template <typename T>
    T getEffective(size_t index, ParameterValues *actualValues) {
        T value;
        checkType<T>();
        getEffective(index, actualValues, &value);
        return value;
    }

    /** \brief called when a new event is generated */
    virtual void generate(ParameterValues *actualValues,
                          Random *__marto_unused(g)) {
        setup(actualValues);
    };
    /** \brief called when an event must be stored into history */
    virtual void store(EventsOStream &__marto_unused(os),
                       ParameterValues *__marto_unused(actualValues)){};
    /** \brief called when an event must be loaded from history */
    virtual void load(EventsIStream &__marto_unused(is),
                      ParameterValues *actualValues) {
        setup(actualValues);
    };
    /** \brief called when an event(-shell) is reseted */
    virtual void release(ParameterValues *actualValues);
    /** \brief return the size (in byte) of parameter values */
    size_t sizeofValues() { return sizeofValue; };

  protected:
    /** \brief called by generate() and load() */
    virtual void setup(ParameterValues *actualValues);

    /** \brief computes and returns an effective value
     *
     * \param pvalue is a pointer to a buffer used to store the value
     *
     * Note: no genericity as we want a virtual method
     */
    virtual void getEffective(size_t index, ParameterValues *actualValues,
                              void *pvalue) = 0;

    template <typename T> void checkType();

  private:
    const size_t length;      ///< length of the parameters (0 means no limit)
    const size_t sizeofValue; ///< size in byte of a value
    const std::type_info &typeinfoValue; ///< typeid of the effective values
};

template <typename T>
class FormalParameterValuesTyped : public FormalParameterValues {
  protected:
    FormalParameterValuesTyped(size_t l)
        : FormalParameterValues(l, (T *)nullptr){};

  public:
    typedef T Type;
    virtual size_t sizeofValues() { return sizeof(T); }
};

template <typename T>
class FormalConstantList : public FormalParameterValuesTyped<T> {
  public:
    FormalConstantList(size_t s, const std::vector<T> &values);

    virtual void getEffective(size_t index, ParameterValues *actualValues,
                              void *pvalue);

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
     * Note: these objects are reused for different kind of event without
     * reallocation
     */
    ParameterValues();

    /** \brief get an indexed element of type T
     *
     * The value come from the buffer if present, else fp is used
     *
     * Notes:
     * - only numeric types are supported (for now)
     * - the state must be set to FPFILLED
     * - it is an error to use 'get' with different type without calling 'reset'
     * in between
     * - the index must be lesser than size() (but size() is 0)
     */
    template <typename T> T get(size_t index);

    /** \brief return the number of values that can be got
     *
     * the special 0 value means as many as wanted
     */
    size_t size();

    /** \brief cleanup all fields so that this object can be reused without
     * reallocation
     *
     * Only the malloced buffer (and its size) are not resetted.
     */
    void reset() {
        state = UNUSED;
        nbValues = 0;
        pinfo = nullptr;
        fp = nullptr;
    };

  private:
    template <typename T> friend class FormalConstantList;
    friend class ParametersBaseTest;
    friend class FormalParameterValues;
    /** \brief state of the current object
     *
     * the state can be modified by the following transitions:
     * - UNUSED -> FPLINKED when setFormalParameterValues() is called
     * - FPLINKED -> FPFILLED when fp->load() or fp->generate() is called
     * - FPFILLED -> UNUSED when fp->release() is called
     */
    enum {
        UNUSED,
        FPLINKED,
        FPFILLED,
    } state;
    /** \brief ensure that the buffer can contain at lease nbValues elements
     *
     * The size of an element comes from the current fp.
     * The state must be set to FPLINKED
     */
    void setCapacity(size_t nbValues);
    void *buffer; ///< adress of a malloced memory buffer. This object can
                  ///realloc the buffer if required. This field is never null
    size_t bufferSize; ///< size of the malloced buffer
    /** \brief register the FormalParameterValues linked to this ParameterValues
     *
     * state must be set to UNUSED before and is set to FPLINKED after
     */
    void setFormalParameterValues(FormalParameterValues *fp);
    FormalParameterValues *fp; ///< related FormalParameterValue
    size_t nbValues;           ///< # values currently stored in the buffer
    void *pinfo; ///< generic pointer to be used by the FormalParameterValue
};
}

#ifndef MARTO_H
// In case of direct inclusion (instead of using <marto.h>),
// we try to include the implementation, hoping to avoid include loops
#include <marto/parameters-impl.h>
#endif

#endif
#endif
