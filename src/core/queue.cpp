#include <marto.h>

namespace marto {

class StandardQueueState : public TypedQueue<StandardQueue> {
  protected:
    queue_state_t value;

  private:
    friend StandardQueue;
    StandardQueueState(StandardQueue *c)
        : TypedQueue<StandardQueue>(c), value(0) {
    } // queue empty at creation by default
    StandardQueueState(StandardQueue *c, queue_state_t v)
        : TypedQueue<StandardQueue>(c), value(0) {
        if (v < 0 || v > capacity()) {
            throw std::out_of_range("queue state");
        }
        value = v;
    }

  public:
    queue_state_t capacity() const { return conf()->capacity(); }
    virtual bool isEmpty() { return value == 0; }
    virtual bool isFull() { return value == capacity(); }
    virtual int addClient(int nb = 1) {
        int prev = value;
        value += nb;
        if (marto_unlikely(value > capacity())) {
            value = capacity();
        }
        return value - prev;
    }
    virtual int removeClient(int nb = 1) {
        int prev = value;
        // TODO: assuming that value is less than 2^31
        if (marto_unlikely((int)value < nb)) {
            value = 0;
        } else {
            value -= nb;
        }
        return prev - value;
    }
    virtual int compareTo(Queue *q) {
        StandardQueueState *other =
            (StandardQueueState *)q; // TODO correctly check cast
        return value - other->value;
    };
};

Queue *StandardQueue::allocateQueue() { return new StandardQueueState(this); }
Queue *StandardQueue::allocateQueue(queue_state_t v) {
    return new StandardQueueState(this, v);
}

class OutsideQueueState : public TypedQueue<OutsideQueue> {
  private:
    friend OutsideQueue;
    OutsideQueueState(OutsideQueue *c) : TypedQueue<OutsideQueue>(c) {}

  public:
    virtual bool isEmpty() { return false; }
    virtual bool isFull() { return false; }
    virtual int addClient(int nb = 1) { return nb; }
    virtual int removeClient(int nb = 1) { return nb; }
    virtual int compareTo(Queue *__marto_unused(q)) {
        throw std::invalid_argument(
            "Trying to compare queue state with outside queue");
    };
};

Queue *OutsideQueue::allocateQueue() { return new OutsideQueueState(this); }
}
