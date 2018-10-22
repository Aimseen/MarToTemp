/* -*-c++-*- C++ mode for emacs */
/* Forward class definition */
#ifndef MARTO_FORWARD_DECL_H
#define MARTO_FORWARD_DECL_H

#ifdef __cplusplus

// generated from the main Makefile with the update-forward-decl-header target

namespace marto {

// Start of auto-generated part. DO NOT MODIFY.

/* allocator.h */
template <typename T> class Allocator;

/* event.h */
class Event;
class EventType;

/* except.h */
class HistoryOutOfBound;
class HistoryIncompleteObject;
class UnknownName;
class ExistingName;
class TypeError;
class DLOpenError;

/* global.h */
class Configuration;
class WithConfiguration;

/* history.h */
class HistoryChunk;
class HistoryIterator;
class History;

/* historyStream.h */
class HistoryStreamBase;
class HistoryIStream;
class HistoryOStream;

/* macros.h */

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
class StateLessQueueConfig;
class Queue;
template <class QC> class TypedQueue;
class StandardQueue;
class OutsideQueue;

/* randomDeterministic.h */
class RandomDeterministicStreamGenerator;
class RandomDeterministic;

/* random.h */
class RandomHistory;
class RandomStream;
class RandomHistoryStream;
class RandomStreamGenerator;
class RandomHistoryStreamGenerator;
class RandomFabric;
class Random;

/* randomLecuyer.h */
class RandomLecuyerStreamGenerator;
class RandomLecuyer;

/* state.h */
class SetImpl;
class Point;
class HyperRectangle;
class Union;
class Set;

/* transition.h */
class Transition;

/* types.h */

// End of auto-generated part. DO NOT MODIFY.

} // namespace marto
#endif
#endif
