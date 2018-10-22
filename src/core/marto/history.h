/* -*-c++-*- C++ mode for emacs */
/* Events History */
#ifndef MARTO_HISTORY_H
#define MARTO_HISTORY_H

#ifdef __cplusplus

#include <cassert>
#include <marto/except.h>
#include <marto/forwardDecl.h>
#include <marto/macros.h>
#include <marto/random.h>
#include <marto/types.h>
#include <memory>
#include <stdint.h>
#include <stdlib.h>

namespace marto {

//////////////////////////////////////////////////////////////////////////////
/** \brief class to manages chunk of events in memory
 *
 * These chunks will be linked in memory to create an history.
 *
 * The chunk notion should be transparent to the user of History.
 *
 * Chunk are also used when some events must be inserted into an existing
 * history (not yet implemented)
 */
class HistoryChunk {
    friend HistoryIterator;
    friend History;

  private:
    HistoryChunk(uint32_t capacity, HistoryChunk *prev, HistoryChunk *next,
                 /*Random *rand, */ History *hist);
    ~HistoryChunk();
    bool allocOwner; ///< true if bufferMemory is malloc'ed
    char *bufferMemory;
    char
        *bufferStart; ///< beginning of history chunk; same as chunkStart ptr in
    /// the plain backward scheme
    char *bufferEnd;         ///< end of history chunk;
    uint32_t eventsCapacity; ///< maximum number of allowed events in this chunk
    /// and possible additional chunks
    uint32_t nbEvents;       ///< current number of events in the chunk
    HistoryChunk *nextChunk; ///< always later in simulated time
    HistoryChunk *prevChunk; ///< always earlier in simulated time
    History *history;        ///< history this chunk belong to

    /** \brief return the next chunk in the history
     *
     * \return NULL at the end of the history.
     *
     * \note this accessor is provided as synchronization will be required
     * when simulating concurrent trajectories (not yet implemented)
     */
    HistoryChunk *getNextChunk();
    /** \brief allocate a new chunk in the history
     *
     * needed when current chunk is full
     * The new chunk is placed just after the current one (in simulated time)
     * Its event capacity is set to the remaining of the current one
     * The capacity of the current chunk is adjusted its current number of
     * events
     */
    HistoryChunk *allocateNextChunk();
};

//////////////////////////////////////////////////////////////////////////////
/** \brief class to allow one to read the history and to write new events
 *
 * \note some parallel version will be to be implemented
 */
class HistoryIterator {
  private:
    HistoryIterator(History *hist);
    // friend Iterator *Historyiterator();
    friend class History;

  public:
    /** \brief Fill ev with the next event
     *
     * The event will come from the history or, if none are available,
     * a new one will be generated.
     */
    history_access_t getNextEvent(Event *ev);

    /** \brief Fill ev with the next event, loading from the history
     */
    history_access_t loadNextEvent(Event *ev);

    /** \brief Generate a new event
     *
     * The event will be written into the history
     */
    history_access_t generateNextEvent(Event *ev);

    /** \brief Write the event in the history.
     *
     * Some place (for events) must be available at the current position
     */
    history_access_t storeNextEvent(Event *ev);
    /* we do not store events reversely. Never. If really required, we
     * should go back for several events and generate and store them
     * forward.

    int storePrevEvent(Event * ev); // for reverse trajectory algorithm
    */

  private:
    HistoryChunk *setNewChunk(HistoryChunk *chunk);

    HistoryChunk *curChunk;
    char *position; ///< current position in the chunk buffer
    uint32_t
        eventNumber; ///< # event in the current chunk to be read or written

    /** Load the event data from its serialization
     *
     */
    history_access_t loadEventContent(HistoryIStream &istream, Event *event);
    /** Stores a compact representation of the event
     *
     *  Note: the store can be aborted (with an exception)
     *  if the current chunk buffer is not bif enough
     *  In this case, storeNextEvent will restart the
     *  call to this function in a new chunk.
     */
    history_access_t storeEventContent(HistoryOStream &ostream, Event *event);

    /** Look if we are ready to write a new event into the history
     *
     * HISTORY_END_DATA is return when this is the case
     */
    history_access_t readyToStore();
};

/** \brief Class to manage an events history
 */
class History : protected WithConfiguration {
  public:
    /** \brief Initialize a new history of events */
    History(Configuration *conf);
    /** \brief Get an iterator positioned at the begining of the history */
    HistoryIterator *iterator();

    /** \brief Add some space in history
     *
     * the space has a capacity of nbEvents and is inserted *before*
     * the previous start of the history
     *
     * iterator() can be called to start at the (new) begining of the
     * history
     */
    void backward(uint32_t nbEvents);

    // DELETED : the next stream is provided by the configuration (globalized
    // service)
    /* \brief generator for the current history
     *
     * This generator is starting at a new available stream
     * associated to the current simulation context One stream per
     * chunk (to be able to regenerate the same events)
     */
    // Random *stream; // FIXME: attribute or method? provides a clone and
    // advances to next stream

  private:
    /// Iterator needs to access to firstChunk
    friend HistoryIterator::HistoryIterator(History *hist);
    /// loadEventContent needs to access to the configuration
    friend history_access_t
    HistoryIterator::loadEventContent(HistoryIStream &istream, Event *ev);
    HistoryChunk *firstChunk; ///< beginning of history
                              // uint32_t _nbEvents; // useful ?
  protected:
    /** \brief Allocate Memory for a chunk
     * \param size must be filled with the size of the allocated buffer if not
     * NULL
     * \return address of a buffer that can be used by an Chunk
     *
     * The default implementation allocates 4096 bytes but any derived
     * class can choose different sizes
     */
    virtual char *allocChunkBuffer(size_t *size) {
        const size_t defaultBufferSize = 4096;
        char *buffer = (char *)malloc(defaultBufferSize);
        if (size) {
            *size = buffer ? defaultBufferSize : 0;
        }
        return buffer;
    }
    friend class HistoryChunk;
};
} // namespace marto

#endif
#endif
