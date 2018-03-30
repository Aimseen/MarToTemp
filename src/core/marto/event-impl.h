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

inline bool Event::valid() { return status == EVENT_STATUS_FILLED; }

inline void Event::clear() {
    parameters.clear();
    status = EVENT_STATUS_INVALID;
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
