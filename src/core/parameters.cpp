#include <marto.h>
#include <marto/parameters.h>

#include <cassert>

namespace marto {

ParameterValues::ParameterValues() {
    kind = UNDEFINED;
    bufferSize = 0;
    buffer = nullptr;
    nbValues = 0;
    reference = nullptr;
}

size_t ParameterValues::size() {
    // TODO : keep nbValues coherent for all kinds
    return nbValues;
}
}
