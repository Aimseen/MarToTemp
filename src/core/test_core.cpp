#include <marto.h>
#include <iostream>

using namespace marto;

class TransitionBidon:public Transition {
    Point *apply(Point * p, Event * ev) {
        for (int i = 0; i < 3; i++)
            p->at(i)++;
    }
};

// Do test stuff
int main() {
    // Fill the hardcoded transition names
    Global::getConfig()->setTransition("TransitionBidon", new TransitionBidon());

    Point *p = new Point();
    p->resize(3);
    for (int i = 0; i < 3; i++)
        p->at(i) = i + 1;
    EventType *et = new EventType("My super event", 42.0, "TransitionBidon", new FormalParameters());
    Event *e = new Event(et);
    
    std::cout << p << std::endl;
    e->apply(p);
    std::cout << p << std::endl;
    std::cout << "Success" << std::endl;
    return 0;
}
