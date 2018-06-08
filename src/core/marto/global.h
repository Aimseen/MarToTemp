/* -*-c++-*- C++ mode for emacs */
/* Global variables (configuration) */
#ifndef MARTO_GLOBAL_H
#define MARTO_GLOBAL_H

#ifdef __cplusplus

#include <map>
#include <marto/forwardDecl.h>
#include <string>
#include <unistd.h>
#include <vector>

namespace marto {

class Configuration {
    /** \brief forbid copy of this kind of objects */
    Configuration(const Configuration &) = delete;
    /** \brief forbid assignment of this kind of objects */
    Configuration &operator=(const Configuration &) = delete;

  private:
    typedef std::map<std::string, Transition *> transitionMap_t;
    typedef std::map<std::string, EventType *> eventTypeMap_t;
    transitionMap_t transitionsMap;            //< Transition by name
    eventTypeMap_t eventTypesMap;              //< EventType by name
    std::vector<EventType *> eventTypesVector; //< EventType by code

    /** \brief private template to factorize the two 'register' methods
     */
    template <typename T, typename Func,
              typename TM = std::map<std::string, T *>,
              typename TMV = typename std::map<std::string, T *>::value_type>
    T *_register(TM &map, std::string name, T *value, Func lambdaIfRegister);

  public:
    Configuration() : transitionsMap(), eventTypesMap(), eventTypesVector(){};
    EventType *getEventType(unsigned num);
    /** \brief retrieve the transition by its name
     *
     * \return the transition
     * generate an UnknownTransition exception if the transition is not
     * registered
     */
    Transition *getTransition(std::string name);
    /** \brief register the provided transition
     *
     * \return the transition itself unless the name is already
     * registered with another transition. In the later case, a
     * ExistingName exception is thrown.
     */
    Transition *registerTransition(std::string name, Transition *trans);
    /** \brief register the provided EventType
     *
     * \return the provided eventType unless the name is already
     * registered with another eventType. In the later case, a
     * ExistingName exception is thrown.
     */
    EventType *registerEventType(EventType *eventType);
    typedef void transitionInitCallback_t(Configuration*);
    /** \brief load a user defined library of transitions
     *
     * libname must be given without the specific platform extension
     * (.so, .dyndl, ...)
     * initCallback must be defined in the library, and of type
     * transitionInitCallback_t
     */
    void loadTransitionLibrary(std::string libname, std::string initCallback);
    void loadTransitionLibrary(std::string libname) {
        loadTransitionLibrary(libname, "initTransitionLibrary");
    };
    /** \brief load the default transitions library
     */
    void loadTransitionLibrary() {
        loadTransitionLibrary("libmarto-transition");
    };
};
}
#endif
#endif
