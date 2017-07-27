/* Event Generator */
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#ifdef __cplusplus

using std::stack;

template <typename T>
class Allocator: public stack<T *> {
      T *new();
      void free(T *o);
}

#endif
#endif