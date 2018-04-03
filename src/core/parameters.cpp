#include <marto.h>
#include <marto/parameters.h>

#include <cassert>

namespace marto {

ParameterValues::ParameterValues() {
    bufferSize=128;
    buffer=malloc(bufferSize);
    assert(buffer!=nullptr);
    reset();
}

size_t ParameterValues::size() {
    // TODO : keep nbValues coherent for all kinds
    return nbValues;
}
}
