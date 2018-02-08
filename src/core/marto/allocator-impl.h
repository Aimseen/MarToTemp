/* -*-c++-*- C++ mode for emacs */
#ifndef MARTO_ALLOCATOR_IMPL_H
#define MARTO_ALLOCATOR_IMPL_H

#ifdef __cplusplus

#ifndef MARTO_ALLOCATOR_H
#  error "Do not include this file directly"
#endif

namespace marto {

template<typename T>
T *Allocator<T>::allocate() {
      if (this->empty()) {
            return new T;
      } else {
            T *result = this->top();
            this->pop();
            return result;
      }
}
      
template<typename T>
void Allocator<T>::free(T *o) {
      push(o);
}

}

#endif
#endif
