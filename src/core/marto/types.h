/* -*-c++-*- C++ mode for emacs */
/* Global types */
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
    EVENT_LOADED = 0,      ///< the event has been correctly loaded
    EVENT_STORED = 0,      ///< the event has been correctly stored
    END_OF_HISTORY,        ///< the end of history is reached
    UNDEFINED_EVENT,       ///< trying to load an event not yet generated
    EVENT_LOAD_CODE_ERROR, ///< error with the code of the event
    EVENT_STORE_UNDEFINED_ERROR =
        EVENT_LOAD_CODE_ERROR, ///< trying to store an invalid/undefined event
    EVENT_LOAD_ERROR,          ///< generic error while loading data
    EVENT_STORE_ERROR = EVENT_LOAD_ERROR, ///< generic error while storing data
} event_access_t;
}
#endif
#endif
