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

template <typename T> ParameterValues::ParameterValues(T *vals, size_t nb) {
    kind = ARRAY;
    bufferSize = nb * sizeof(T);
    // TODO : decide if we want to copy vals or just use it
    buffer = vals;
    assert(buffer != nullptr);
    nbValues = nb;
}

template <typename T> T ParameterValues::get(unsigned int index) {
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

template <typename T>
// TODO To discuss : 'type' versus 'T'
FormalConstantList::FormalConstantList(ParamType type, size_t s,
                                       std::vector<T> *v)
    : FormalParameterValues(type, s) {
    switch (type) {
    case IntList:
        // TODO : temporary, for testing only
        // TODO : copy v into values
        values = new ParameterValues();
        // TODO
        break;
    default:
        marto_BUG();
    }
}
}

#endif
#endif
