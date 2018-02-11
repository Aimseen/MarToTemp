#include <marto.h>
#include <iostream>

using namespace marto;

class TransitionBidon:public Transition {
    Point *apply(Point * p, __attribute__((unused)) Event * ev) {
        for (int i = 0; i < 3; i++)
            p->at(i)++;
        return p;
    }
    HyperRectangle *apply(HyperRectangle * h, __attribute__((unused)) Event * ev) {
        return h;
    }
    Union *apply(Union * u, __attribute__((unused)) Event * ev) {
        return u;
    }
};

// Do test stuff
int main() {
    auto c=Global::getConfig();
    c->setTransition("TransitionBidon", new TransitionBidon());
    auto h=new EventsHistory(c);
    EventType *et = new EventType(c, "My super event", 42.0, "TransitionBidon", new FormalParameters());
    Event *e = new Event(et);
    auto it=h->iterator();
    auto res=it->storeNextEvent(e);
    assert(res == EVENT_STORE_UNDEFINED_ERROR);
    e->generate();
    for (int i=0; i<10; i++) {
        res=it->storeNextEvent(e);
        assert(res == EVENT_STORED);
    }
    return 0;
}
