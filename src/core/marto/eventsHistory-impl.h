/* -*-c++-*- C++ mode for emacs */
/* Events History template implementation */
#ifndef MARTO_EVENTS_HISTORY_IMPL_H
#define MARTO_EVENTS_HISTORY_IMPL_H

#ifdef __cplusplus

#ifndef MARTO_EVENTS_HISTORY_H
#error "Do not include this file directly"
#endif

#include <marto/except.h>

namespace marto {

template <typename T> void EventsIStream::read(T &var) {
    if (marto_unlikely(bufsize == 0)) {
        eofbit = 1;
        return;
    }
    void *ptr = (void *)buf;
    if (!std::align(alignof(T), sizeof(T), ptr, bufsize)) {
        /* no enough place in the buffer to read the requested data */
        throw HistoryOutOfBound("No enough place in the current buffer to read "
                                "the requested data!");
    }
    T *newbuf = ((T *)ptr) + 1;
    size_t size = sizeof(T);
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

template <typename T> T *EventsOStream::write(const T &value) {
    if (marto_unlikely(eof())) {
        return nullptr;
    }
    void *ptr = (void *)buf;
    if (!std::align(alignof(T), sizeof(T), ptr, bufsize)) {
        abort();
        throw HistoryOutOfBound("Not enough place for the current event");
    }
    T *newbuf = ((T *)ptr) + 1;
    size_t size = sizeof(T);
    if (marto_unlikely(size > bufsize)) {
        // bufsize += ((char*)ptr-buf); // no need: ostream wont be used anymore
        abort();
        throw HistoryOutOfBound("Not enough place for the current event");
    }
    bufsize -= size;
    eventsize += size;
    buf = (char *)newbuf;
    *(T *)ptr = value;
    return (T *)ptr;
}
}

#endif
#endif
