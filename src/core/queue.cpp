#include <marto.h>

namespace marto {

class StandardQueueState : public TypedQueue<StandardQueue> {
  protected:
    int value;
  private:
    friend StandardQueue;
    StandardQueueState(StandardQueue* c) : TypedQueue<StandardQueue>(c), value(0) {}
  public:
    int capacity() const { return conf()->capacity(); }
    virtual bool isEmpty() { return value == 0; }
    virtual bool isFull() { return value == capacity(); }
    virtual int addClient(int nb=1) {
        int prev = value;
        value += nb;
        if (marto_unlikely(value > capacity())) {
            value = capacity();
        }
        return value-prev;
    }
    virtual int removeClient(int nb=1) {
        int prev = value;
        value -= nb;
        if (marto_unlikely(value < 0)) {
            value = 0;
        }
        return prev-value;
    }
    virtual int compareTo(Queue* q) {
        StandardQueueState *other=(StandardQueueState*)q; // TODO correctly check cast
        return value-other->value;
    };

};

Queue* StandardQueue::allocateQueue() {
    return new StandardQueueState(this);
}

}
