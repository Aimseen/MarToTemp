/* -*-c++-*- C++ mode for emacs */
/* Global variables (configuration) */
#ifndef MARTO_GLOBAL_H
#define MARTO_GLOBAL_H

#ifdef __cplusplus

#include <cassert>
#include <functional>
#include <ltdl.h>
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
    typedef std::map<std::string, QueueConfig *> queueConfigMap_t;
    transitionMap_t transitionsMap;                //< Transition by name
    eventTypeMap_t eventTypesMap;                  //< EventType by name
    std::vector<EventType *> eventTypesVector;     //< EventType by code
    queueConfigMap_t queueConfigsMap;              //< QueueConfig by name
    std::vector<QueueConfig *> queueConfigsVector; // vector of queue capacities
    friend class Point; //< Point constructor iterates over queueConfigsVector.
                        // TODO: to encapsulate ?

    /** \brief private template to factorize the two 'register' methods
     */
    template <typename T, typename Func,
              typename TM = std::map<std::string, T *>,
              typename TMV = typename std::map<std::string, T *>::value_type>
    T *_register(TM &map, std::string name, T *value, Func lambdaIfRegister);

    /** \brief sum of rates of all registered eventTypes
     */
    double ratesSum;

  public:
    Configuration()
        : transitionsMap(), eventTypesMap(), eventTypesVector(),
          queueConfigsMap(), queueConfigsVector(), ratesSum(0.0){};
    /** \brief retrieve the eventType by its number
     */
    EventType *getEventType(unsigned num);
    /** \brief give an eventType randomly chosen
     *
     * rate of eventType are taken into account
     */

    EventType *getRandomEventType(Random *g);
    /** \brief retrieve the transition by its name
     *
     * \return the transition
     * generate an UnknownTransition exception if the transition is not
     * registered
     */
    Transition *getTransition(std::string name);
    /** \brief retrieve the queue configuration by its name
     *
     * \return the queueConfig
     * generate an UnknownQueue exception if the queue name is not
     * registered
     */
    QueueConfig *getQueueConfig(std::string name);

  private:
    /** \brief register the provided Queue
     *
     * \return the provided queue unless the name is already
     * registered with another eventType. In the later case, a
     * ExistingName exception is thrown.
     */
    QueueConfig *registerQueue(std::string name, QueueConfig *queue);
    friend class QueueConfig;
    /** \brief register the provided transition
     *
     * \return the transition itself unless the name is already
     * registered with another transition. In the later case, a
     * ExistingName exception is thrown.
     */
    Transition *registerTransition(std::string name, Transition *trans);
    friend class Transition;
    /** \brief register the provided EventType
     *
     * \return the provided eventType unless the name is already
     * registered with another eventType. In the later case, a
     * ExistingName exception is thrown.
     */
    EventType *registerEventType(EventType *eventType);
    friend class EventType;

  public:
    typedef std::function<void(lt_dlhandle handle)> loadLibraryCallback_t;
    /** \brief load a user defined library of transitions
     *
     * libname must be given without the specific platform extension
     * (.so, .dyndl, ...)
     *
     * callback will be executed with the handle of the loaded library in
     * parameter
     */
    void loadTransitionLibrary(std::string libname,
                               loadLibraryCallback_t callback);
    typedef void transitionInitCallback_t(Configuration *);
    /** \brief load a user defined library of transitions
     *
     * libname must be given without the specific platform extension
     * (.so, .dyndl, ...)
     *
     * The symbol initCallback must be defined in the library, and of type
     * transitionInitCallback_t
     */
    void loadTransitionLibrary(std::string libname, std::string initCallback);
    void loadTransitionLibrary(std::string libname) {
        loadTransitionLibrary(libname, "initTransitionLibrary");
    };
    /** \brief load the default transitions library
     */
    void loadTransitionLibrary() { loadTransitionLibrary("std_transitions"); };
};

/** Partial class to inherit that offerts a link to a Configuration object
 */
class WithConfiguration {
  private:
    Configuration *_config;

  public:
    WithConfiguration(Configuration *c) : _config(c) { assert(c != nullptr); }
    Configuration *config() const { return _config; }
};
} // namespace marto
#endif
#endif
