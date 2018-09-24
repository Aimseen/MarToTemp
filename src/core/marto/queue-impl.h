/* -*-c++-*- C++ mode for emacs */
/* Queues management implementation */
#ifndef MARTO_QUEUE_IMPL_H
#define MARTO_QUEUE_IMPL_H

#ifdef __cplusplus

#ifndef MARTO_QUEUE_H
#error "Do not include this file directly"
#endif

#include <marto/global.h>
#include <marto/macros.h>

namespace marto {

inline QueueConfig::QueueConfig(Configuration *c, const std::string &name)
    : WithConfiguration(c) {
    config()->registerQueue(name, this);
}

inline StateLessQueueConfig::~StateLessQueueConfig() { delete queueState; }
} // namespace marto

#endif
#endif
