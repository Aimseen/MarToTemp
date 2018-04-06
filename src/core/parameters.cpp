#include <marto.h>
#include <marto/parameters.h>

#include <cassert>

namespace marto {

ParameterValues::ParameterValues() : ParameterValues(128) {}

ParameterValues::ParameterValues(size_t bufSize) : bufferSize(bufSize) {
    buffer = malloc(bufferSize);
    assert(buffer != nullptr);
    reset();
}
}
