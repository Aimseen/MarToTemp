#include <marto/random.h>

namespace marto {

double Random::next() {
    /* FIXME */
    /* but garantee true random with a roll dice */
    return 4;
};

/*  internal generic-type uniform generator */
RandomUniformInterval::RandomUniformInterval(double _inf, double _sup)
    : inf(_inf), sup(_sup){};

double RandomUniformInterval::next() {
    return (inf + (sup - inf) * Random::next());
};
}
