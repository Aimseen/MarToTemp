/* -*-c++-*- C++ mode for emacs */
/* Global variables (configuration) */
#ifndef MARTO_GLOBAL_H
#define MARTO_GLOBAL_H

#ifdef __cplusplus

#include <unistd.h>
#include <vector>
#include <marto/forwardDecl.h>
#include <string>
#include <map>

namespace marto {

class Configuration {
    std::vector<EventType *> eventTypesVector;
    std::map<std::string, Transition *> transitionsVector;
public :
    EventType *getEventType(unsigned num);
    Transition *getTransition(std::string name);
    void setTransition(std::string name, Transition *trans);
};

class Global {
    static Configuration *config;
public :
    static Configuration *getConfig();
};

}
#endif
#endif
