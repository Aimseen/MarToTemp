/* -*-c++-*- C++ mode for emacs */
/* Parameters management templates implementation */
#ifndef MARTO_PARAMETERS_IMPL_H
#define MARTO_PARAMETERS_IMPL_H

#ifdef __cplusplus

#ifndef MARTO_PARAMETERS_H
#error "Do not include this file directly"
#endif

#include <marto/macros.h>
#include <typeindex>

namespace marto {

template <typename T> T ParameterValues::get(size_t index) {
    T *values = (T *)buffer;
    assert(state == FPFILLED);
    if (index < size()) {
        return values[index];
    }
    /* the value needs to be generated if the formal parameter
       support lazzy generation (else, this is a bug) */
    return fp->getEffective<T>(index, this);
    // fp->
}

inline void ParameterValues::setCapacity(size_t nbValues) {
    assert(state == FPLINKED);
    size_t requiredSize = nbValues * fp->sizeofValues();
    if (requiredSize < bufferSize) {
        bufferSize = next_power2(requiredSize);
        buffer = realloc(buffer, bufferSize);
        assert(buffer != nullptr);
    }
}

inline void
ParameterValues::setFormalParameterValues(FormalParameterValues *fp) {
    assert(state == UNUSED);
    assert(fp != nullptr);
    this->fp = fp;
    state = FPLINKED;
}

inline void FormalParameterValues::setup(ParameterValues *ep) {
    ep->fp = this;
    if (length > 0) {
        ep->setCapacity(length);
    }
}

inline void FormalParameterValues::release(ParameterValues *actualValues) {
    actualValues->reset();
}

template <typename T> void FormalParameterValues::checkType() {
    const std::type_info &typeinfoRequested = typeid(T);
    assert(std::type_index(typeinfoValue) ==
           std::type_index(typeinfoRequested));
}

template <typename T>
FormalConstantList<T>::FormalConstantList(
    size_t s, const std::vector<T> &__marto_unused(v))
    : FormalParameterValuesTyped<T>(s) {
    // TODO : temporary, for testing only
    // TODO : copy v into values
    values = new ParameterValues();
    // TODO
}

template <typename T>
void FormalConstantList<T>::getEffective(
    size_t index, ParameterValues *__marto_unused(actualValues), void *pvalue) {
    *(T *)pvalue = values->get<T>(index);
}
}

#endif
#endif
