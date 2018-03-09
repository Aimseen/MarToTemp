/* -*-c++-*- C++ mode for emacs */
/* Event Generator template implementation */
#ifndef MARTO_EVENT_IMPL_H
#define MARTO_EVENT_IMPL_H

#ifdef __cplusplus

#ifndef MARTO_EVENT_H
#  error "Do not include this file directly"
#endif

#include <marto/macros.h>
#include <marto/global.h>

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

inline EventType *Event::setType(EventType *type) {
    this->type=type;
    if (type == nullptr) {
        status = EVENT_STATUS_INVALID;
        return nullptr;
    }
    status = EVENT_STATUS_TYPED;
    return type;
}

inline EventType *Event::setTypeFromCode(Configuration *config) {
    assert(config != nullptr);
    return setType(config->getEventType(code));
}

inline EventType *Event::setTypeAndCode(EventType *type) {
    if (setType(type)) {
        code=type->code();
    }
    return type;
}

}

#endif
#endif
