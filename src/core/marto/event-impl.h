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

template <typename T> T ParameterValues::get(unsigned int index) {
    switch (kind) {
    case GENERATOR:
        if (u.generator.cache.size() <= index) {
            for (auto i = u.generator.cache.size(); i < index + 1; i++)
                u.generator.cache[i] = u.generator.g.next();
        }
        return (T)u.generator.cache[index];
    case REFERENCE:
        marto_BUG(); // TODO: handle this case
    case ARRAY:
        T *array = (T *)u.array.values;
        return array[index];
    }
}

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
    return type;
}
}

#endif
#endif
