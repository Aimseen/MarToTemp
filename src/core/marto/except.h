/* -*-c++-*- C++ mode for emacs */
/* Exceptions in marto */
#ifndef MARTO_EXCEPT_H
#define MARTO_EXCEPT_H

#ifdef __cplusplus

#include <stdexcept>
#include <string>

namespace marto {

// FIXME: define this kind of macros in a better place
#define marto_likely(x)       __builtin_expect((x),1)
#define marto_unlikely(x)     __builtin_expect((x),0)

#define marto_BUG() do {                        \
    assert(0);                                  \
    abort();                                    \
    } while(0);

/** Exception launched when trying to read or write out of the allocated space of the history */
class HistoryOutOfBound : public std::runtime_error {
public:
    HistoryOutOfBound(std::string s): std::runtime_error(s) {};
};

/** Exception launched when trying to read a event not yet all written */
class HistoryIncompleteEvent : public std::runtime_error {
public:
    HistoryIncompleteEvent(std::string s): std::runtime_error(s) {};
};

}

#endif

#endif
