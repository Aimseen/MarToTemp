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
    if (status == EVENT_STATUS_FILLED) {
        size_t fpsize = this->type()->formalParameters.size();
        size_t i = 0;
        for (auto p : parameters) {
            if (i == fpsize) {
                break;
            }
            p->reset();
        }
    }
    status = EVENT_STATUS_INVALID;
}

inline EventType *Event::type() {
    assert(status != EVENT_STATUS_INVALID);
    return _type;
}

inline EventType *Event::setType(EventType *type) {
    clear();
    this->_type = type;
    if (type == nullptr) {
        status = EVENT_STATUS_INVALID;
        return nullptr;
    }
    status = EVENT_STATUS_TYPED;
    size_t psize = parameters.size();
    size_t fpsize = type->formalParameters.size();
    if (marto_unlikely(psize < fpsize)) {
        parameters.reserve(fpsize);
        for (size_t i = psize; i < fpsize; i++) {
            parameters[i] = new ParameterValues();
        }
    }
    return type;
}
} // namespace marto

#endif
#endif
