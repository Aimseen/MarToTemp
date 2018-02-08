/* -*-c++-*- C++ mode for emacs */
/* Exceptions in marto */
#ifndef MARTO_EXCEPT_H
#define MARTO_EXCEPT_H

#ifdef __cplusplus

#include <stdexcept>
#include <string>

namespace marto {

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
