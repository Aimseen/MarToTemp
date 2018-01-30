/* Event Generator */
#ifndef MARTO_GLOBAL_H
#define MARTO_GLOBAL_H

#ifdef __cplusplus

#include <unistd.h>
#include <vector>
#include <marto/event.h>

namespace marto {
class Configuration {
    std::vector<EventType *> eventTypesVector;
public :
    EventType *getEventType(int num);
};

class Global {
    constexpr static Configuration *config = nullptr;
public :
    static Configuration *getConfig();
};
}
#endif
#endif
