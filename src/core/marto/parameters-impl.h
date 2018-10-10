/* -*-c++-*- C++ mode for emacs */
/* Parameters management templates implementation */
#ifndef MARTO_PARAMETERS_IMPL_H
#define MARTO_PARAMETERS_IMPL_H

#ifdef __cplusplus

#ifndef MARTO_PARAMETERS_H
#error "Do not include this file directly"
#endif

#include <marto/except.h>
#include <marto/macros.h>
#include <string>
#include <typeindex>

namespace marto {

template <typename T> T ParameterValues::get(size_t index) {
    T *values = (T *)buffer;
    assert(state == FPFILLED);
    fp->checkType<T>();
    if (index < nbValues) {
        return values[index];
    }
    if (size() > 0 && index >= size()) {
        throw std::out_of_range("parameterValues");
    }
    /* the value needs to be generated if the formal parameter
       support lazzy generation (else, this is a bug) */
    return fp->getEffective<T>(index, this);
    // fp->
}

template <typename T> void ParameterValues::push(const T &value, size_t index) {
    assert(state == FPLINKED);
    fp->checkType<T>();
    assert(index == nbValues);
    assert(size() == 0 || index < size());
    nbValues++;
    setCapacity(nbValues);
    T *values = (T *)buffer;
    values[index] = value;
}

inline void ParameterValues::setCapacity(size_t nbValues) {
    assert(state == FPLINKED);
    size_t requiredSize = nbValues * fp->sizeofValues();
    if (requiredSize < bufferSize) {
        bufferSize = next_power2(requiredSize);
        buffer = realloc(buffer, bufferSize);
        assert(buffer != nullptr);
    }
}

inline void
ParameterValues::setFormalParameterValues(FormalParameterValues *fp) {
    assert(state == UNUSED);
    assert(this->fp == nullptr);
    this->fp = fp;
    state = FPLINKED;
}

inline size_t ParameterValues::size() const {
    assert(state >= FPLINKED);
    return fp->size();
}

inline void FormalParameterValues::generate(ParameterValues *actualValues,
                                            Random *g) {
    initPV(actualValues);
    doGenerate(actualValues, g);
    actualValues->filled();
}

inline history_access_t
FormalParameterValues::load(HistoryIStream &is, ParameterValues *actualValues) {
    initPV(actualValues);
    auto res = doLoad(is, actualValues);
    actualValues->filled();
    return res;
}

inline void FormalParameterValues::initPV(ParameterValues *ep) {
    ep->setFormalParameterValues(this);
    if (length > 0) {
        ep->setCapacity(length);
    }
    assert(ep->state == ParameterValues::FPLINKED);
}

inline void FormalParameterValues::release(ParameterValues *actualValues) {
    doRelease(actualValues);
    actualValues->reset();
}

template <typename T> void FormalParameterValues::checkType() {
    const std::type_info &typeinfoRequested = typeid(T);
    if (std::type_index(typeinfoValue) != std::type_index(typeinfoRequested)) {
        std::string s;
        s = s + "In parameters: requested: " + typeinfoRequested.name() +
            " / available: " + typeinfoValue.name();
        throw TypeError(s);
    }
}

template <typename T>
FormalConstantList<T>::FormalConstantList(size_t s, const std::vector<T> &v)
    : FormalParameterValuesTyped<T>(s) {
    assert(v.size() >= this->size());
    values = new ParameterValues(s * sizeof(T));
    this->initPV(values);
    for (size_t i = 0; i < this->size(); i++) {
        values->push(v[i], i);
    }
    values->filled();
}

template <typename T>
void FormalConstantList<T>::getEffective(
    size_t index, ParameterValues *__marto_unused(actualValues), void *pvalue) {
    *(T *)pvalue = values->get<T>(index);
}
} // namespace marto

#endif
#endif
