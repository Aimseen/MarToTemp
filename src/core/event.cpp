#include <marto/event.h>
#include <marto/global.h>
#include <marto/eventsHistory.h>
#include <cstdint>
#include <iostream>

#include <assert.h>

using std::ostream;

ostream &operator << (ostream &out, const marto::FormalParameters &fp) {
    // FIXME:
    for (auto it=fp;;) {
        out << it << std::endl;
    }
}

ostream &operator << (ostream &out, const marto::EventType &ev) {
    out << "EventType : " << ev.id << std::endl;
    out << "-> " << ev.transition << std::endl;
    out << "-> parameters : " << std::endl;
    out << ev.fp << std::endl;
}

namespace marto {

int EventType::findIndex(string name) {
    auto couple = fp.find(name);
    assert(couple != fp.end());
    return couple->second.first;   //couple is a pair, whose first element is the index in the parameters table
}

ParameterValues *Event::getParameter(string name) {
    if (int index = type->findIndex(name)) {
        return parameters[index];
    }
}

template <typename T>
T ParameterValues::get(int index) {
    switch (kind) {
    case ARRAY:
        T *array = (T *) u.array.values;
        return array[index];
    case GENERATOR:
        if (u.generator.cache.size() <= index) {
            for (int i=u.generator.cache.size(); i<index+1; i++)
                u.generator.cache[i] = u.generator.g.next();
        }
        return (T) u.generator.cache[index];
    }
}

size_t ParameterValues::size() {
    switch (kind) {
    case ARRAY:
        return u.array.nbValues;
    case GENERATOR:
        return SIZE_MAX;
    }
}

/*  an eventType is stored compactly as an integer
   (example of event type : arrival in queue 1)
   it is read from a table built from user config file
 */
int Event::load(EventsHistory * hist, EventsIStream &istream) {
    parameters.clear();     /* clears previously stored event information */
    int code;
    // FIXME: next line is just to show
    istream >> code >> code;   //eventype is encoded in the first integer of the event buffer.
    type = hist->getConfig()->getEventType(code);
    for (auto pair:type->fp) {
        /* pair iterates on all elements in fp (list of pairs) */
        //parameters.insert(pair.first, pair.second.load(intBuffer+1);/* inserts actual parameters computed using the load method of formalParameterValue class */
    }
}

int Event::store(EventsHistory * hist, EventsOStream &ostream) {
    int code=12;
    // FIXME: next line is just to show
    ostream << code << code;  //eventype is encoded in the first integer of the event buffer.
    type = hist->getConfig()->getEventType(code);
    for (auto pair:type->fp) {
        /* pair iterates on all elements in fp (list of pairs) */
        //parameters.insert(pair.first, pair.second.load(intBuffer+1);/* inserts actual parameters computed using the load method of formalParameterValue class */
    }
}

}
