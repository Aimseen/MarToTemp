/* -*-c++-*- C++ mode for emacs */
/* Event Generator */
#ifndef MARTO_EVENT_H
#define MARTO_EVENT_H

#ifdef __cplusplus

#include <list>
#include <map>
#include <marto/forwardDecl.h>
#include <marto/global.h>
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
ostream &operator<<(ostream &out, const marto::FormalParameterValues &ev);

namespace marto {

using std::string;

typedef uint32_t Queue;

/* list of values for this parameter (for instance : input queues) */
class ParameterValues {
public:
    // This is not an abstract class because we do not want to allocate
    // a new object for each parameter list of each event generated.
    // Instead, the parameter class is a placeholder wich can store either
    // an array of values or a sequence generated from a given generator seed.
    //
    // In the case of an array, this assumes that the user always asks for the
    // same value type and does not exceed the size she has declared in
    // the configuration.
    template < typename T > T get(unsigned int index);
    size_t size();
private:
    friend class FormalConstantList;
    friend class ParametersBaseTest;
    ParameterValues();
    template < typename T > ParameterValues(T *vals, size_t nb);
    enum { ARRAY, GENERATOR, REFERENCE, UNDEFINED } kind; // reference= formalconstantlist ; array can be either a list of constants or a fixed list
    void *buffer;
    size_t bufferSize;
    size_t nbValues;
    Random g;
    ParameterValues *reference;
};

enum ParamType {
    QueueList,
    IntList,
    DoubleList,
    // Martostring,
    InvalidType
};

/** Parameters coming from the model description
 *
 * type is used to compact the data if needed
 * l is the maximum number of actual parameters that can be extracted. 0 means no limit. 
 */
class FormalParameterValues {
public:
    FormalParameterValues(ParamType type, size_t l) : paramType(type), length(l) {};
    virtual void generateParameterValues(ParameterValues *actualValues) = 0;
    virtual void store(EventsOStream &os, ParameterValues*actualValues) = 0;
    virtual void load(EventsIStream &is, ParameterValues*actualValues) = 0;
private:
    ParamType paramType;
    size_t length;
};

class FormalConstantList:public FormalParameterValues {
public:
    template<typename T>
    FormalConstantList(ParamType type, size_t s, std::vector<T> *values);
    virtual void generateParameterValues(ParameterValues *actualValues __attribute__((unused))) {};
    /* store nothing, all is constant */
    virtual void store(EventsOStream &os __attribute__((unused)),
                       ParameterValues*actualValues __attribute__((unused))) {};
    /* nothing to load, all is constant */
    virtual void load(EventsIStream &is __attribute__((unused)),
                      ParameterValues*actualValues __attribute__((unused))) {};
private:
    ParameterValues *values;
};

class FormalDistribution : public FormalParameterValues {
public:
    FormalDistribution(ParamType type, size_t l, Random *rand);
private:
    Random *rand;
};
/* when the event is created the values are generated and stored for future direct use at replay */
class FormalDistributionFixedList:public FormalDistribution {
public:
    FormalDistributionFixedList(ParamType type, size_t l, string idRandom);
};

/* the event only stores the generator : values are re-generated at every replay to save buffer space 
    some long fixed lists could be more optimally stored using this generator.
*/ 
class FormalDistributionVariadicList:public FormalDistribution {
public:
    FormalDistributionVariadicList(ParamType type, size_t l, string idRandom);
};

/* each simulation sequence only uses 1 object of type Event */
class Event {
    friend EventType;
  public:
    /** type used to store the event code */
    typedef unsigned code_t;
    /** Create an empty (unusable) event
     */
    Event();
    /** Creates an event from a given type */
    Event(EventType *type);
    /** Creates (generate) a new Event
     * SEEMS WRONG : because of the rate, the generation should
     * randomly decide of which eventType to generate
     * useful if for one eventype there is a random choice to be made e.g. JSQ)
     *  returns 1 */
    static int generate(EventType *type);
    /* FIXME: just here for basic tests */
    void generate() { status = EVENT_STATUS_FILLED; };

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
    enum eventStatus { EVENT_STATUS_INVALID, EVENT_STATUS_TYPED, EVENT_STATUS_FILLED };

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
     * \param idTr indicates which transition function will be used.
     * \param eventName is a long, detailed name for the event.
     * \param fp include all parameters needed to generate the event
     *
     * \note the EventType will be registered into the provided configuration
     */
    EventType(Configuration * config, string eventName, double evtRate, string idTr);
    void registerParameter(string name, FormalParameterValues *fp) {
        // TODO : check for duplicate name, ...
        formalParametersNames.insert(std::make_pair(name, formalParameters.size()));
        formalParameters.push_back(fp);
    };
private:
    friend ostream & ::operator << (ostream &out, const EventType &ev);
    // Name for this event type
    string name;
    Transition *transition;
    double rate;
    // Numbered formal parameters
    std::vector < FormalParameterValues* > formalParameters;
    // Association from names to formal parameter number
    std::map < string, int > formalParametersNames;
    Event::code_t _code; ///< code of this EventType as assigned by the configuration
    
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
    Event::code_t code() {
        return _code;
    };
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
