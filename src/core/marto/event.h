/* Event Generator */
#ifndef MARTO_EVENT_H
#define MARTO_EVENT_H

#ifdef __cplusplus

#include <marto/transition.h>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <map>


namespace marto {

class EventsChunk;
class EventsIterator;
class EventsHistory;

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

class EventType {
public:
    /* idTr indicates which transition function will be used.
       idEvt indicates the detailed event.
       fp include all parameters needed to generate the event */
    EventType(string idEvT, double rate, string idTr, FormalParameters fp);
private:
    marto::Transition * transition;
    double rate;
public:
    int findIndex(string name);
    FormalParameters fp;
    //GetParameter gp; /* ??? FIXME */
    int nbIntStaticParameters();
    int nbDoubleStaticParameters();
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
    template < typename T > T get(int index);
    size_t size();
private:
    enum { ARRAY, GENERATOR, REFERENCE } kind;
    union {
        struct {
            void *values;
            size_t nbValues;
        };
        struct {
            // Note: std::vector no allowed in anonymous union
            std::vector < double >cache;
            // TODO: declare/define Generator type => also update event.cpp
            // ParameterValues::get()
            //Generator g;
        } s;
        ParameterValues *reference;
    } u;                    //need to choose between one of those 3 fields, depending on "kind"
};

/* each simulation sequence only uses 1 object of type Event */
class Event {
public:
    /* Create an empty (unusable) event */
    Event();
    /* Load the event data from its serialization
     *  returns the number of byte read upon success,
     *  0 if the compact representation
     *  does not match a known event type */
    size_t load(marto::EventsIterator * h);
    /* Stores a compact representation of the event
     *  returns the size of stored object or 0 upon failure
     */
    size_t store(void *buffer, size_t buf_size);
    /* Creates (generate) a new Event
     *  returns 1 */
    static int generate(EventType * type);

    ParameterValues *getParameter(string name);

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
    std::vector < ParameterValues * >parameters;   /* actual parameters (not formal), used to apply transition */
    EventType *type;
};

}
#endif
#endif
