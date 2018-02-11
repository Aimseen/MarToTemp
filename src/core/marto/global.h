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
private:
    std::vector<EventType *> eventTypesVector;
    std::map<std::string, Transition *> transitionsVector;
    //* \brief private constructor to avoid duplicate instanciation
    Configuration() {};
    friend class Global;
public :
    EventType *getEventType(unsigned num);
    Transition *getTransition(std::string name);
    void setTransition(std::string name, Transition *trans);
    EventType* registerEventType(EventType* eventType);
};

class Global {
    static Configuration *config;
public :
    static Configuration *getConfig();
};

}
#endif
#endif
