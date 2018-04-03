/* -*-c++-*- C++ mode for emacs */
/* Macros for marto */
#ifndef MARTO_MACROS_H
#define MARTO_MACROS_H

#ifdef __cplusplus

#include <cassert>

namespace marto {

#define marto_likely(x) __builtin_expect(!!(x), 1)
#define marto_unlikely(x) __builtin_expect(!!(x), 0)

#define marto_BUG()                                                            \
    do {                                                                       \
        assert(0);                                                             \
        abort();                                                               \
    } while (0);
}

#include <climits>
template <typename T> T next_power2(T value) {
    return 1 << ((sizeof(T) * CHAR_BIT) - __builtin_clz(value - 1));
}

#define __marto_unused(x) x __attribute__((unused))

#endif

#endif
