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
T ParameterValues::get(unsigned int index) {
    switch (kind) {
    case GENERATOR:
        if (u.generator.cache.size() <= index) {
            for (auto i=u.generator.cache.size(); i<index+1; i++)
                u.generator.cache[i] = u.generator.g.next();
        }
        return (T) u.generator.cache[index];
    case REFERENCE:
        marto_BUG(); // TODO: handle this case
    case ARRAY:
        T *array = (T *) u.array.values;
        return array[index];
    }
}

}

#endif
#endif
