/* -*-c++-*- C++ mode for emacs */
/* Global basic types */
#ifndef MARTO_TYPES_H
#define MARTO_TYPES_H

#ifdef __cplusplus

#include <map>
#include <marto/forwardDecl.h>
#include <string>
#include <unistd.h>
#include <vector>

namespace marto {

typedef enum {
    HISTORY_DATA_LOADED = 0, ///< the data has been correctly loaded
    HISTORY_DATA_STORED = 0, ///< the data has been correctly stored
    HISTORY_END_DATA,        ///< no more objects into the history for now
    HISTORY_END_HISTORY,     ///< the end of history is reached
    HISTORY_OBJECT_TOO_BIG, ///< the object cannot be stored into an empty chunk
    HISTORY_STORE_INVALID_EVENT, ///< trying to store an invalid event
    HISTORY_DATA_LOAD_ERROR,     ///< generic error while loading data
    HISTORY_DATA_STORE_ERROR,    ///< generic error while storing data
} history_access_t;

/** unique queue identifier */
typedef uint32_t queue_id_t;

/** queue content */
typedef int32_t queue_state_t;
} // namespace marto
#endif
#endif
