/* -*-c++-*- C++ mode for emacs */
/* Event Generator */
#ifndef MARTO_EVENT_H
#define MARTO_EVENT_H

#ifdef __cplusplus

#include <list>
#include <map>
#include <marto/forwardDecl.h>
#include <marto/global.h>
#include <marto/parameters.h>
#include <marto/random.h>
#include <marto/types.h>
#include <stddef.h>
#include <stdint.h>
#include <string>

// convolution to keep operator<< in global namespace
// See
// https://stackoverflow.com/questions/38801608/friend-functions-and-namespaces
using std::ostream;
ostream &operator<<(ostream &out, const marto::EventType &ev);

namespace marto {

using std::string;

/* each simulation sequence only uses 1 object of type Event */
class Event {
    friend EventType;

  public:
    /** type used to store the event code */
    typedef unsigned code_t;
    /** Create an empty (unusable) event
     */
    Event();
    /** Creates (generate) a new Event
     * SEEMS WRONG : because of the rate, the generation should
     * randomly decide of which eventType to generate
     * useful if for one eventype there is a random choice to be made e.g. JSQ)
     */
    void generate(EventType *type, Random *g);

    /** An event is valid when all its parameters are available */
    inline bool valid();

    /** \brief clear an event
     *
     * All parameters are emptied
     * The EventType is forgotten
     */
    inline void clear();

    inline EventType *type();

    ParameterValues *getParameter(string name);
    void apply(Point *p);

  private:
    enum eventStatus {
        EVENT_STATUS_INVALID,
        EVENT_STATUS_TYPED,
        EVENT_STATUS_FILLED
    };

    std::vector<ParameterValues *> parameters; /**< actual parameters (not
                                                  formal), used to apply
                                                  transition */
    EventType *_type;
    enum eventStatus status;

    friend EventsIterator;
    void loaded() { status = EVENT_STATUS_FILLED; }

    /** \brief setup the EventType of the event
     *
     * \return the parameter type
     *
     * reinitialize the parameters and other attribute if required
     * status is set to EVENT_STATUS_TYPED (or EVENT_STATUS_INVALID if this
     * function returns nullptr)
     */
    inline EventType *setType(EventType *type);
};

class EventType {
    friend class Event;
    friend class EventsIterator;

  public:
    /** \brief create a new EventType in the configuration
     *
     * \param trName indicates which transition function will be used.
     * \param eventName is a long, detailed name for the event.
     * \param fp include all parameters needed to generate the event
     *
     * \note the EventType will be registered into the provided configuration
     */
    EventType(Configuration *config, string eventName, double evtRate,
              string trName);
    /** \brief define a new FormalParameterValues for this EventType
     *
     * \param name: name of the formal parameter. Must be unique per EventType.
     * \param fp: pointer to a formal parameter.
     *
     * \return true if 'name' is not already used in this EventType
     * \return false if 'name' already exists (and then, fp is ignored)
     */
    bool registerParameter(string name, FormalParameterValues *fp);

  private:
    friend ostream & ::operator<<(ostream &out, const EventType &ev);
    // Name for this event type
    string name;
    Transition *transition;
    double rate;
    // Numbered formal parameters
    std::vector<FormalParameterValues *> formalParameters;
    // Association from names to formal parameter number
    std::map<string, int> formalParametersNames;
    Event::code_t
        _code; ///< code of this EventType as assigned by the configuration

    /** used by the Configuration to assign a code */
    void setCode(Event::code_t c) { _code = c; };
    friend EventType *Configuration::registerEventType(EventType *);

  protected:
    /** \brief load actual parameters from history to an event
     *
     * \return EVENT_LOADED if the load is successful
     */
    virtual event_access_t load(EventsIStream &istream, Event *event,
                                EventsHistory *hist);
    /** \brief store actual parameters of an event into history
     *
     * \return EVENT_STORED if the store is successful
     */
    virtual event_access_t store(EventsOStream &ostream, Event *event,
                                 EventsHistory *hist);

  public:
    /** Code of this kind of event */
    Event::code_t code() { return _code; };
    /** \brief returns the index of the named parameter (internal use)
     *
     * \return a positive index or -1 if the name does not exist
     */
    int findIndex(string parameterName);
};
}

#ifndef MARTO_H
// In case of direct inclusion (instead of using <marto.h>),
// we try to include the implementation, hoping to avoid include loops
#include <marto/event-impl.h>
#endif

#endif
#endif
