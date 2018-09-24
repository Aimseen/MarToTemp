/* -*-c++-*- C++ mode for emacs */
#ifndef MARTO_ALLOCATOR_H
#define MARTO_ALLOCATOR_H

#ifdef __cplusplus

#include <stack>

namespace marto {

using std::stack;

template <typename T> class Allocator : public stack<T *> {
    T *allocate();
    void free(T *o);
};
} // namespace marto

#ifndef MARTO_H
// In case of direct inclusion (instead of using <marto.h>),
// we try to include the implementation, hoping to avoid include loops
#include <marto/allocator-impl.h>
#endif

#endif
#endif
