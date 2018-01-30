#include <marto/state.h>
#include <marto/transition.h>
#include <vector>

namespace marto {

// Specialisation: a Point always gives a Point
Point *marto::Point::accept(Transition * t, Event * ev) {
    return t->apply(this, ev);
}

SetImpl *marto::HyperRectangle::accept(Transition * t, Event * ev) {
    return t->apply(this, ev);
}

SetImpl *marto::Union::accept(Transition * t, Event * ev) {
    return t->apply(this, ev);
}

}
