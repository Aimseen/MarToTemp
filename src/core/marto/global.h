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
    std::map<string, Transition *> transitionsVector;
public :
    EventType *getEventType(unsigned num);
    Transition *getTransition(string name);
    void setTransition(string name, Transition *trans);
};

class Global {
    static Configuration *config;
public :
    static Configuration *getConfig();
};
}
#endif
#endif
