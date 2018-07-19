#include <marto.h>
#include <vector>

namespace marto {

Point::Point(Configuration *const config) : WithConfiguration(config) {
    resize(config->queueConfigsVector.size());
    size_t i = 0;
    for (auto qc = config->queueConfigsVector.begin();
         qc < config->queueConfigsVector.end(); qc++, i++) {
        std::vector<Queue *>::at(i) = (*qc)->newQueue();
    }
}

Point::Point(Configuration *const config, queue_state_t value)
    : Point(config, [value](queue_id_t __marto_unused(id),
                            QueueConfig *__marto_unused(qc),
                            Point *__marto_unused(p)) { return value; }) {}

Point::Point(Configuration *const config, Point::initCallback_t *f, void *arg)
    : Point(config, [arg, f](queue_id_t id, QueueConfig *qc, Point *p) {
          return f(id, qc, p, arg);
      }) {}

Point::Point(Configuration *const config, const Point::initLambdaCallback_t &f)
    : WithConfiguration(config) {
    resize(config->queueConfigsVector.size());
    size_t i = 0;
    for (auto qc = config->queueConfigsVector.begin();
         qc < config->queueConfigsVector.end(); qc++, i++) {
        std::vector<Queue *>::at(i) = (*qc)->newQueue(f(i, *qc, this));
    }
}

// Specialisation: a Point always gives a Point
Point *Point::accept(Transition *t, Event *ev) { return t->apply(this, ev); }

SetImpl *HyperRectangle::accept(Transition *t, Event *ev) {
    return t->apply(this, ev);
}

SetImpl *Union::accept(Transition *t, Event *ev) { return t->apply(this, ev); }
}
