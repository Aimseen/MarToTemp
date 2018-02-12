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
    std::map<std::string, Transition *> transitionsMap;
    //* \brief private constructor to avoid duplicate instanciation
    Configuration() {};
    friend class Global;
public :
    EventType *getEventType(unsigned num);
    /** \brief retrieve the transition by its name
     *
     * \return the transition
     * generate an out_of_range exception if the transition is not registered
     */
    Transition *getTransition(std::string name);
    /** \brief retrieve the transition by its name
     *
     * \return NULL if the transition is not registered
     */
    Transition *getTransition0(std::string name);
    /** \brief register the provided transition
     *
     * \return NULL if the name is already registered with another transition else the transition itself
     */
    Transition *registerTransition(std::string name, Transition *trans);
    /** \brief register the provided EventType
     *
     * \return the provided eventType
     */
    EventType *registerEventType(EventType* eventType);
};

class Global {
    static Configuration *config;
public :
    static Configuration *getConfig();
};

}
#endif
#endif
