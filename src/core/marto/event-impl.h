/* -*-c++-*- C++ mode for emacs */
/* Event Generator template implementation */
#ifndef MARTO_EVENT_IMPL_H
#define MARTO_EVENT_IMPL_H

#ifdef __cplusplus

#ifndef MARTO_EVENT_H
#error "Do not include this file directly"
#endif

#include <marto/global.h>
#include <marto/macros.h>

namespace marto {

template <typename T>
ParameterValues::ParameterValues(T *vals, size_t nb) {
    kind = ARRAY;
    bufferSize = nb*sizeof(T);
    // TODO : decide if we want to copy vals or just use it
    buffer = vals;
    assert(buffer != nullptr);
    nbValues = nb;
}

template <typename T>
T ParameterValues::get(unsigned int index) {
    T *values = (T *) buffer;
    switch (kind) {
    case GENERATOR:
        // TODO : resize buffer
        if (nbValues <= index) {
            for (auto i=nbValues; i<index+1; i++)
                values[i] = g.next();
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

inline bool Event::valid() { return status == EVENT_STATUS_FILLED; }

inline void Event::clear() {
    parameters.clear();
    status = EVENT_STATUS_INVALID;
}

template<typename T>
// TODO To discuss : 'type' versus 'T'
FormalConstantList::FormalConstantList(ParamType type, size_t s, std::vector<T> *v) : FormalParameterValues(type, s) {
    switch (type) {
        case IntList:
            // TODO : temporary, for testing only
            // TODO : copy v into values
            values = new ParameterValues();
            break;
        default:
            marto_BUG();
    }
}

inline EventType *Event::type() {
    assert(status != EVENT_STATUS_INVALID);
    return _type;
}

inline EventType *Event::setType(EventType *type) {
    this->_type = type;
    if (type == nullptr) {
        status = EVENT_STATUS_INVALID;
        return nullptr;
    }
    status = EVENT_STATUS_TYPED;
    parameters.reserve(type->formalParameters.size());
    return type;
}
}

#endif
#endif
