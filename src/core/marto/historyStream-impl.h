/* -*-c++-*- C++ mode for emacs */
/* Events History template implementation */
#ifndef MARTO_HISTORY_STREAM_IMPL_H
#define MARTO_HISTORY_STREAM_IMPL_H

#ifdef __cplusplus

#ifndef MARTO_HISTORY_STREAM_H
#error "Do not include this file directly"
#endif

#include <type_traits>

#include <marto/except.h>

#define DEBUG_COMPACTINT

#ifdef DEBUG_COMPACTINT
#include <iostream>
#endif

namespace marto {

template <typename T>
void HistoryStreamBase::CompactInt<T>::readFromHIS(HistoryIStream &istream) {
    uint8_t sval;
    typename std::make_unsigned<T>::type uval;
    int neg = 0;
    size_t bits;
    istream >> sval;
    if (std::is_signed<T>::value) {
        uval = sval & 63;
        if (sval & 64) {
            neg = 1;
        }
        bits = 6;
    } else {
        uval = sval & 127;
        bits = 7;
    }
    while (sval & 128) {
        istream >> sval;
        uval |= ((__typeof__(uval))(sval & 127)) << bits;
        bits += 7;
    }
    if (neg) {
        val = ~uval;
    } else {
        val = uval;
    }
#ifdef DEBUG_COMPACTINT
    std::cerr << "decompacting " << (long long)(T)val << std::endl;
#endif
}

template <typename T>
void HistoryStreamBase::CompactInt<T>::writeToHOS(HistoryOStream &ostream) const {
#ifdef DEBUG_COMPACTINT
    std::cerr << "compacting " << (long long)(T)val << std::endl;
#endif
    typename std::make_unsigned<T>::type uval;
    uint8_t sval;
    if (std::is_signed<T>::value) {
        if (val < 0) {
            uval = ~val;
            sval = 64 | (uval & 63);
        } else {
            uval = val;
            sval = uval & 63;
        }
        uval >>= 6;
    } else {
        sval = val & 127;
        uval = val >> 7;
    }
    do {
        if (uval) {
            sval |= 128;
        }
        ostream << sval;
        sval = uval & 127;
        uval >>= 7;
    } while (sval || uval);
}

template <typename T, size_t sizeof_T = sizeof(T), size_t alignof_T = alignof(T)>
typename std::enable_if<alignof_T != 1>::type
HistoryIStream::read(T &var) {
    if (marto_unlikely(bufsize == 0)) {
        eofbit = 1;
        return;
    }
    void *ptr = (void *)buf;
    if (!std::align(alignof_T, sizeof_T, ptr, bufsize)) {
        /* no enough place in the buffer to read the requested data */
        throw HistoryOutOfBound("No enough place in the current buffer to read "
                                "the requested data!");
    }
    T *newbuf = ((T *)ptr) + 1;
    const size_t size = sizeof_T;
    if (marto_unlikely(size > bufsize)) {
        /* no enough place in the buffer to read the requested data */
        bufsize += ((char *)ptr - buf);
        throw HistoryOutOfBound("No enough place in the current buffer to read "
                                "the requested data!");
    }
    bufsize -= size;
    buf = (char *)newbuf;
    var = *(T *)ptr;
}

template <typename T, size_t sizeof_T = sizeof(T), size_t alignof_T = alignof(T)>
typename std::enable_if<alignof_T == 1>::type
HistoryIStream::read(T &var) {
    if (marto_unlikely(bufsize == 0)) {
        eofbit = 1;
        return;
    }
    void *ptr = (void *)buf;
    T *newbuf = ((T *)ptr) + 1;
    const size_t size = sizeof_T;
    if (marto_unlikely(size > bufsize)) {
        /* no enough place in the buffer to read the requested data */
        throw HistoryOutOfBound("No enough place in the current buffer to read "
                                "the requested data!");
    }
    bufsize -= size;
    buf = (char *)newbuf;
    var = *(T *)ptr;
}

template <typename T> inline HistoryIStream &HistoryIStream::operator>>(T &var) {
    static_assert(std::is_arithmetic<T>::value, "T must be numeric");
    readvar<std::is_integral<T>::value>(var);
    return *this;
}

template <> inline HistoryIStream &HistoryIStream::operator>><uint8_t>(uint8_t &var) {
    read(var);
    return *this;
}

template <> inline HistoryIStream &HistoryIStream::operator>><int8_t>(int8_t &var) {
    read(var);
    return *this;
}

template <> inline HistoryIStream &HistoryIStream::operator>><>(IsReadable &var) {
    var.readFromHIS(*this);
    return *this;
}

template <typename T> inline HistoryIStream &HistoryIStream::operator>>(CompactInt<T> &var) {
    var.readFromHIS(*this);
    return *this;
}

template <typename T> T *HistoryOStream::write(const T &value) {
    if (marto_unlikely(eof())) {
        return nullptr;
    }
    void *ptr = (void *)buf;
    if (!std::align(alignof(T), sizeof(T), ptr, bufsize)) {
        throw HistoryOutOfBound("Not enough place for the current object");
    }
    T *newbuf = ((T *)ptr) + 1;
    bufsize -= sizeof(T);
    objectsize += (char *)newbuf - (char *)buf;
    buf = (char *)newbuf;
    *(T *)ptr = value;
    return (T *)ptr;
}

template <> inline HistoryOStream &HistoryOStream::operator<< <uint8_t>(const uint8_t &var) {
    write(var);
    return *this;
}

template <> inline HistoryOStream &HistoryOStream::operator<< <int8_t>(const int8_t &var) {
    write(var);
    return *this;
}

inline history_access_t HistoryOStream::finalize() {
    if (eof()) {
        return HISTORY_DATA_STORE_ERROR;
    }
    assert(objectsize <= (objectsize_t)(-1));
    objectsize_t size = objectSize();
    assert(*objectSizePtr == 0 || *objectSizePtr == size);
    *objectSizePtr = size;
    return HISTORY_DATA_STORED;
}
} // namespace marto

#endif
#endif
