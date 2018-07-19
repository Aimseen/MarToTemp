/* -*-c++-*- C++ mode for emacs */
/* Queues management */
#ifndef MARTO_QUEUE_H
#define MARTO_QUEUE_H

#include <marto/forwardDecl.h>
#include <marto/global.h>

namespace marto {

/** Static parameters for Queues
 *
 * One object of this type will be created by the config
 */
class QueueConfig : protected WithConfiguration {
  public:
    QueueConfig(Configuration *c, const std::string &name);
    virtual ~QueueConfig(void) {}
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
    Queue* newQueue(queue_state_t value) {
        return allocateQueue(value);
    }
  protected:
    virtual Queue* allocateQueue() = 0;
    virtual Queue* allocateQueue(queue_state_t value);
  private:
    queue_id_t _id; ///< queue id as assigned by the configuration

    friend QueueConfig *Configuration::registerQueue(std::string, QueueConfig *);
    /** used by the Configuration to assign a id */
    void setId(queue_id_t id) { _id = id; }
};

/** State of a queue
 *
 * Each queue can have different state (at different time and/or to store envelops)
 */
class Queue {
  public:
    Queue() {}
    virtual ~Queue() {}
    virtual QueueConfig *queueConfig() const = 0;
    virtual bool isEmpty() = 0;
    virtual bool isFull() = 0;
    virtual int addClient(int nb=1) = 0; ///< return #clients really added (up to full)
    virtual int removeClient(int nb=1) = 0; ///< return #client really removed (until empty)
    virtual int compareTo(Queue*) = 0;
  protected:
    friend QueueConfig;
    virtual void setInitialState(queue_state_t value) = 0;
};

inline Queue* QueueConfig::allocateQueue(queue_state_t value) {
    Queue* q=allocateQueue();
    q->setInitialState(value);
    return q;
}

// is T derived from QueueConfig?
template < typename T > // http://en.cppreference.com/w/cpp/header/type_traits
struct is_QueueConfig : std::conditional< std::is_base_of<QueueConfig,T>::value,
                                         std::true_type, std::false_type >::type {} ;
/** Queue specialized for QueueConfig
 *
 * Queue is an abstract class. Each instance should be linked to
 * its QueueConfig object (also an abstract class).
 *
 * TypedQueue<QC> is derived from Queue with a QC pointer attribute.
 * QC must be derivated from QueueConfig (checked at compile time)
 *
 * The conf() method returns an QC (QueueConfig derivated) object that
 * avoid dynamic or even static cast when accessing to the relatec
 * QueueConfig object.
 */
template<class QC>
class TypedQueue : public Queue {
    // compile-time assertion: QC must be derived from QueueConfig
    static_assert( is_QueueConfig<QC>::value, "QC must be derived from QueueConfig" ) ;
  private:
    QC * const _qconf;
  protected:
    virtual QC *conf() const { return _qconf; }
  public:
    TypedQueue(QC *conf) : Queue(), _qconf(conf) {}
    virtual ~TypedQueue() {}
    virtual QueueConfig *queueConfig() const { return static_cast<QueueConfig *>(conf()); }
    virtual bool isEmpty() = 0;
    virtual bool isFull() = 0;
    virtual int addClient(int nb=1) = 0; ///< return #clients really added (up to full)
    virtual int removeClient(int nb=1) = 0; ///< return #client really removed (until empty)
    virtual int compareTo(Queue*) = 0;
};

class StandardQueue : public QueueConfig {
private:
    const int _capacity;
public:
    StandardQueue(Configuration *c, const std::string &name, int capacity) : QueueConfig(c, name), _capacity(capacity) {};
    int capacity() const { return _capacity; }
protected:
    virtual Queue* allocateQueue();
};

class OutsideQueue : public QueueConfig {
public:
    OutsideQueue(Configuration *c, const std::string &name) : QueueConfig(c, name) {};
protected:
    virtual Queue* allocateQueue();
};

}

#ifndef MARTO_H
// In case of direct inclusion (instead of using <marto.h>),
// we try to include the implementation, hoping to avoid include loops
#include <marto/queue-impl.h>
#endif

#endif
