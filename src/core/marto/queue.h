/* -*-c++-*- C++ mode for emacs */
/* Queues management */
#ifndef MARTO_QUEUES_H
#define MARTO_QUEUES_H

#include <marto/forwardDecl.h>
#include <marto/global.h>

namespace marto {

/** unique queue identifier */
typedef uint32_t queue_id_t;

/** queue content */
typedef uint32_t queue_state_t;

/** Static parameters for Queues
 *
 * One object of this type will be created by the config
 */
class QueueConfig {
  public:
    QueueConfig(void) {}
    /** Get the queue id of the queue
     */
    queue_id_t id();
    /** Tell if the queue is special or classic

       Special queues:
       - do not need to be stored into the Point (ie their state is constant)
       - can have a specific, pre-reserved, ID
     */
    virtual bool isSpecialQueue() { return false; }
     /** return a new queue state
     *
     * TODO: should check that the queue config is registered
     */
    Queue* newQueue() {
        return allocateQueue();
    }
  protected:
    virtual Queue* allocateQueue() = 0;
  private:
    queue_id_t _id; ///< code of this EventType as assigned by the configuration

    /** link to the configuration used when the queue is registrered */
    Configuration *_config;
    friend QueueConfig *Configuration::registerQueue(std::string, QueueConfig *);
    /** used by the Configuration to assign a id */
    void setConfig(Configuration *config, queue_id_t id) { _config = config; _id = id; }
  protected:
    Configuration *config(void) { return _config; }
};

/** State of a queue
 *
 * Each queue can have different state (at different time and/or to store envelops)
 */
class Queue {
  private:
    QueueConfig *_qconf;
  public:
    Queue(QueueConfig *conf) : _qconf(conf) {}
    QueueConfig *config() { return _qconf; }
    virtual bool isEmpty() = 0;
    virtual bool isFull() = 0;
    virtual int addClient(int nb=1) = 0; ///< return #clients really added (up to full)
    virtual int removeClient(int nb=1) = 0; ///< return #client really removed (until empty)
    virtual int compareTo(Queue*) = 0;
};

class StandardQueue : public QueueConfig {
private:
    int _capacity;
public:
    StandardQueue(int capacity) : QueueConfig(), _capacity(capacity) {};
    int capacity() { return _capacity; }
protected:
    virtual Queue* allocateQueue();
};
}

#endif
