/* -*-c++-*- C++ mode for emacs */
/* Parameters management templates implementation */
#ifndef MARTO_PARAMETERS_IMPL_H
#define MARTO_PARAMETERS_IMPL_H

#ifdef __cplusplus

#ifndef MARTO_PARAMETERS_H
#error "Do not include this file directly"
#endif

#include <marto/macros.h>

namespace marto {

template <typename T> T ParameterValues::get(size_t index) {
    T *values = (T *)buffer;
    switch (kind) {
    case GENERATOR:
        // TODO : resize buffer
        if (nbValues <= index) {
            for (auto i = nbValues; i < index + 1; i++)
                values[i] = g->next();
        }
        return values[index];
    case REFERENCE:
        marto_BUG(); // TODO: handle this case
    case ARRAY:
        return values[index];
    default:
        marto_BUG();
    }
}

inline void FormalParameterValues::initParameterValues(ParameterValues* ep,
                                                Random *g __attribute__((unused))) {
    ep->fp=this;
}

template <typename T>
FormalConstantList<T>::FormalConstantList(size_t s, const std::vector<T> &v)
    : FormalParameterValuesTyped<T>(s) {
    // TODO : temporary, for testing only
    // TODO : copy v into values
    values = new ParameterValues();
    // TODO
}
}

#endif
#endif
