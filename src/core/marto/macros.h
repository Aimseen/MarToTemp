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

#endif

#endif
