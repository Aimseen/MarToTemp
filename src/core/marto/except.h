/* -*-c++-*- C++ mode for emacs */
/* Exceptions in marto */
#ifndef MARTO_EXCEPT_H
#define MARTO_EXCEPT_H

#ifdef __cplusplus

#include <stdexcept>
#include <string>

namespace marto {

/** Exception launched when trying to read or write out of the allocated space
 * of the history */
class HistoryOutOfBound : public std::runtime_error {
  public:
    HistoryOutOfBound(std::string s) : std::runtime_error(s){};
};

/** Exception launched when trying to read a event not yet all written */
class HistoryIncompleteObject : public std::runtime_error {
  public:
    HistoryIncompleteObject(std::string s) : std::runtime_error(s){};
};

/** Exception launched when trying to access an unknown object */
class UnknownName : public std::out_of_range {
  public:
    UnknownName(std::string name) : std::out_of_range(name){};
};

/** Exception launched when trying to register an already registered name */
class ExistingName : public std::runtime_error {
  public:
    ExistingName(std::string name) : std::runtime_error(name){};
};

/** Exception launched a type error is detected at runtime */
class TypeError : public std::runtime_error {
  public:
    TypeError(std::string s) : std::runtime_error(s){};
};

/** Exception launched when a library cannot be loaded */
class DLOpenError : public std::runtime_error {
  public:
    DLOpenError(std::string s) : std::runtime_error(s){};
};
} // namespace marto

#endif

#endif
