#include <marto.h>
#include <iostream>
#include <string>

namespace marto {

class JSQ2:public Transition {
    Point *apply(Point * p, Event * ev) {       //Event ev contains transition specification (parameters)
        // fromList is a random sequence of Queues (specified in ev) to prevent access to random generation and protect monotonicity
        //marto::ParameterValues<marto::Queue>
        auto *fromList = ev->getParameters("from");     // We only get vectors of values as defined in the configuration file
        auto *toList = ev->getParameters("to");

        auto from = fromList->get < Queue > (0);        // the only source queue
        auto to0 = toList->get < Queue > (0);   // the first random destination queue
        auto to1 = toList->get < Queue > (1);   // second random destination queue

        if (p->at(from) > 0) {
            if (p->at(to0) < p->at(to1))
                p->at(to0)++;
            else
                p->at(to1)++;
            p->at(from)--;
        }
        return p;
    }
};

}
