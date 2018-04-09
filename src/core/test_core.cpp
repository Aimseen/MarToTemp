#include <iostream>
#include <marto.h>

using namespace marto;

class TransitionBidon : public Transition {
    Point *apply(Point *p, __attribute__((unused)) Event *ev) {
        for (int i = 0; i < 3; i++)
            p->at(i)++;
        return p;
    }
};

// Do test stuff
int main() {
    Configuration *config = Global::getConfig();
    // Fill the hardcoded transition names
    config->registerTransition("TransitionBidon", new TransitionBidon());

    Point *p = new Point();
    p->resize(3);
    for (int i = 0; i < 3; i++)
        p->at(i) = i + 1;
    EventType *et =
        new EventType(config, "My super event", 42.0, "TransitionBidon");
    Event *e = new Event();
    // Fixme: handle randomGenerator
    e->generate(et, nullptr);

    std::cout << p << std::endl;
    e->apply(p);
    std::cout << p << std::endl;
    std::cout << "Success" << std::endl;
    return 0;
}
