/* -*-c++-*- C++ mode for emacs */
/* Transition used in various tests */
#ifndef MARTO_TRANSITION_TEST_H
#define MARTO_TRANSITION_TEST_H

#include <marto.h>

class TransitionTest : public marto::Transition {
    default_transition_constructors;
    marto::Point *apply(marto::Point *p,
                        __attribute__((unused)) marto::Event *ev) {
        for (auto q = p->begin(); q < p->end(); q++)
            (*q)->addClient(1);
        return p;
    }
};

#endif
