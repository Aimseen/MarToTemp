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
class EventType;
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

/* parameters.h */
class FormalParameterValues;
template <typename T> class FormalParameterValuesTyped;
template <typename T> class FormalConstantList;
template <typename T> class FormalDistribution;
template <typename T> class FormalDistributionFixedList;
template <typename T> class FormalDistributionVariadicList;
class ParameterValues;

/* queue.h */
class QueueConfig;
class Queue;
class StandardQueue;

/* random.h */
class RandomStream;
class RandomStreamGenerator;
class RandomFabric;
class RandomTest;
class Random;

/* randomLecuyer.h */
class RandomLecuyer;

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
