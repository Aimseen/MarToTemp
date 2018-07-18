#include <marto.h>
#include <vector>

namespace marto {

Point::Point(Configuration * const config) {
    resize(config->queueConfigsVector.size());
    size_t i=0;
    for (auto qc = config->queueConfigsVector.begin();
         qc<config->queueConfigsVector.end(); qc++, i++) {
        at(i) = (*qc)->newQueue();
    }
}

// Specialisation: a Point always gives a Point
Point *Point::accept(Transition *t, Event *ev) {
    return t->apply(this, ev);
}

SetImpl *HyperRectangle::accept(Transition *t, Event *ev) {
    return t->apply(this, ev);
}

SetImpl *Union::accept(Transition *t, Event *ev) {
    return t->apply(this, ev);
}
}
