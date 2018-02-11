/* -*-c++-*- C++ mode for emacs */
/* Event Generator */
#ifndef MARTO_EVENT_H
#define MARTO_EVENT_H

#ifdef __cplusplus

#include <marto/forwardDecl.h>
#include <marto/random.h>
#include <marto/global.h>
#include <marto/types.h>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <map>
#include <list>

// convolution to keep operator<< in global namespace
// See https://stackoverflow.com/questions/38801608/friend-functions-and-namespaces
using std::ostream;
ostream &operator << (ostream &out, const marto::EventType &ev);
ostream &operator << (ostream &out, const marto::FormalParameters &ev);

namespace marto {

using std::string;

typedef uint32_t Queue;

enum ParamType {
    QueueList,
    IntList,
    DoubleList,
    //Martostring,
    InvalidType
};

class FormalParameterValue {
public:
    FormalParameterValue(ParamType type, size_t l);
private:
    ParamType paramType;
    size_t length;
};

// collection of FormalParameter
// Après 6 mois, on penserait qu'il s'agit de:
// string (nom du parametre) -> taille liste, contenu liste
// avec la convention taille==-1 => taille variable (potentiellement infinie)
class FormalParameters:public std::map < string, std::pair < int, FormalParameterValue >> {
    friend ostream & ::operator << (ostream &out, const FormalParameters &ev);
    //void addParam(string name, FormalParameterValue *value);
};

class FormalConstantList:public FormalParameterValue {
    std::list < double >l;
};

class FormalDistribution:public FormalParameterValue {
public:
    FormalDistribution(string idRandom, FormalParameters fp);
};

class FormalDistributionFixedList:public FormalDistribution {
public:
    FormalDistributionFixedList(string idRandom, FormalParameters fp);
};

class FormalDistributionVariadicList:public FormalDistribution {
public:
    FormalDistributionVariadicList(string idRandom, FormalParameters fp);
};

/* list of values for this parameter (for instance : input queues) */
class ParameterValues {
public:
    // This is not an abstract class because we do not want to allocate
    // a new object for each parameter list of each event generated.
    // Instead, the parameter class is a placeholder wich can store either
    // an array of values or a sequence generated from a given generator seed.
    //
    // In the case of an array, this assumes that the user always ask for the
    // same value type and does not exceed the size she has declared in
    // the configuration.
    template < typename T > T get(unsigned int index);
    size_t size();
private:
    enum { ARRAY, GENERATOR, REFERENCE } kind;
    union {
        struct {
            void *values;
            size_t nbValues;
        } array;
        struct {
            // Note: std::vector no allowed in anonymous union
            std::vector < double >cache;
            // TODO: declare/define Generator type => also update event.cpp
            // ParameterValues::get()
            Random g;
        } generator;
        ParameterValues *reference;
    } u;                    //need to choose between one of those 3 fields, depending on "kind"
};

/* each simulation sequence only uses 1 object of type Event */
class Event {
public:
    /** type used to store the event code */
    typedef unsigned code_t;
    /** Create an empty (unusable) event
     */
    Event();
    /** Creates an event from a given type */
    Event(EventType *type);
    /** Load the event data from its serialization
     *
     */
    event_access_t load(EventsIStream &istream, EventsHistory *h);
    /** Stores a compact representation of the event
     *
     *  Note: the store can be aborted (with an exception)
     *  if the current chunk buffer is not bif enough
     *  In this case, storeNextEvent will restart the
     *  call to this function in a new chunk.
     */
    event_access_t store(EventsOStream &ostream, EventsHistory *h);
    /** Creates (generate) a new Event
     * SEEMS WRONG : because of the rate, the generation should
     * randomly decide of which eventType to generate
     * useful if for one eventype there is a random choice to be made e.g. JSQ)
     *  returns 1 */
    static int generate(EventType * type);
    /* FIXME: just here for basic tests */
    void generate() {
        status=EVENT_STATUS_FILLED;
    };

    ParameterValues *getParameter(string name);
    void apply(Point *p);

    /* Parameters accessors */
    int8_t int8Parameter(int index);
    int16_t int16Parameter(int index);
    int32_t int32Parameter(int index);
    int64_t int64Parameter(int index);
    double doubleParameter(int index);
    int8_t int8Parameter(const string & pname);
    int16_t int16Parameter(const string & pname);
    int32_t int32Parameter(const string & pname);
    int64_t int64Parameter(const string & pname);
    double doubleParameter(const string & pname);

    void set(int index, int8_t value);
    void set(int index, int16_t value);
    void set(int index, int32_t value);
    void set(int index, int64_t value);
    void set(int index, double value);

private:
    enum eventStatus { EVENT_STATUS_INVALID, EVENT_STATUS_TYPED, EVENT_STATUS_FILLED };

    std::vector < ParameterValues * >parameters;   /**< actual parameters (not formal), used to apply transition */
    EventType *type;
    enum eventStatus status;
    code_t code;

    /** \brief setup the EventType of the event
     *
     * \return the EventType (or nullptr if not found in config)
     *
     * reinitialize the parameters and other attribute if required
     * status is set to EVENT_STATUS_TYPED (or EVENT_STATUS_INVALID if this function returns nullptr)
     */
    inline EventType * setTypeFromCode(Configuration *c);
    /** \brief setup the EventType of the event
     *
     * \return the parameter type
     *
     * reinitialize the parameters and other attribute if required
     * status is set to EVENT_STATUS_TYPED (or EVENT_STATUS_INVALID if this function returns nullptr)
     * the code of the Event is set from the EventType
     */
    inline EventType * setTypeAndCode(EventType *type);
    /** \brief setup the EventType of the event
     *
     * \return the parameter type
     *
     * reinitialize the parameters and other attribute if required
     * status is set to EVENT_STATUS_TYPED (or EVENT_STATUS_INVALID if this function returns nullptr)
     */
    inline EventType * setType(EventType *type);
};

class EventType {
    friend class Event;
public:
    /** \brief create a new EventType in the configuration
     *
     * \param idTr indicates which transition function will be used.
     * \param eventName is a long, detailed name for the event.
     * \param fp include all parameters needed to generate the event
     *
     * \note the EventType will be registered into the provided configuration
     */
    EventType(Configuration * config, string eventName, double evtRate, string idTr, FormalParameters *fp);
private:
    friend ostream & ::operator << (ostream &out, const EventType &ev);
    string name;
    Transition * transition;
    double rate;
    // TODO: choose if we use a pointer or an object, same for constructor.
    FormalParameters *parameters;
    Event::code_t _code; ///< code of this EventType as assigned by the configuration
    /** used by the Configuration to assign a code */
    void setCode(Event::code_t c) {
        _code=c;
    };
    friend EventType* Configuration::registerEventType(EventType*);
protected:
    /** \brief load actual parameters from history to an event
     *
     * \return true if the load is successful
     */
    virtual event_access_t load(EventsIStream &istream, Event *event, EventsHistory * hist);
    /** \brief store actual parameters of an event into history
     *
     * \return true if the store is successful
     */
    virtual event_access_t store(EventsOStream &ostream, Event *event, EventsHistory * hist);
public:
    /** Code of this kind of event */
    Event::code_t code() {
        return _code;
    };
    int findIndex(string parameterName);
    //GetParameter gp; /* ??? FIXME */
    int nbIntStaticParameters();
    int nbDoubleStaticParameters();
};

}

#ifndef MARTO_H
// In case of direct inclusion (instead of using <marto.h>),
// we try to include the implementation, hoping to avoid include loops
#  include <marto/event-impl.h>
#endif

#endif
#endif
