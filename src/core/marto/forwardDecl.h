/* -*-c++-*- C++ mode for emacs */
/* Forward class definition */
#ifndef MARTO_FORWARD_DECL_H
#define MARTO_FORWARD_DECL_H

#ifdef __cplusplus

// generated with:
// for f in $(ls src/core/marto/*.h | sort ) ; do echo "/* $(basename $f) */";
// grep '^class.*{' $f | sed -e 's,^\(class [^ :{]*\)[ :{].*,\1;,' ; done

namespace marto {

/* allocator.h */
template <typename T> class Allocator;

/* event.h */
class FormalParameterValues;
class FormalConstantList;
class FormalDistribution;
class FormalDistributionFixedList;
class FormalDistributionVariadicList;
class EventType;
class ParameterValues;
class Event;

/* eventsHistory.h */
class EventsChunk;
class EventsIterator;
class EventsStreamBase;
class EventsIStream;
class EventsOStream;
class EventsHistory;

/* except.h */
class HistoryOutOfBound;
class HistoryIncompleteEvent;

/* global.h */
class Configuration;
class Global;

/* random.h */
class Random;
class RandomUniformInterval;
class InternalGeneratorFabric;

/* RngStream.h */
class RngStream;

/* state.h */
class SetImpl;
class Point;
class HyperRectangle;
class Union;
class Set;

/* transition.h */
class Transition;
}
#endif
#endif
