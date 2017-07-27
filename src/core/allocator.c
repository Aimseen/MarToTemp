#include <marto/allocator.h>

template <typename T>
T *Allocator::new() {
      if (empty()) {
            return new T;
      } else {
            T *result = top();
            pop();
            return result;
      }
}
      
void Allocator::free(T *o) {
      push(o);
}
}