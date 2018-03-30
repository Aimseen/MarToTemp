#include <marto/random.h>
/* We include the .c because we need the static constants it contains */
#include <RngStream.c>
#include <string.h>

namespace marto {

static void advanceStream(double *state) {
    /* Taken from Lecuyer implementation (see RngStream_CreateStream in RngStream.c).
       Extracted because it is not separated from memory allocation */
    MatVecModM (A1p127, state, state, m1);
    MatVecModM (A2p127, &state[3], &state[3], m2);
}

/* We use some knowledge about the implementation from Lecuyer :
   - the begining of the current stream is in state.Ig
   - the begining of the current substream is in state.Bg
   - the current position is in state.Cg
   - a seed is an array of 6 double
*/

static int seedSize=6;
static size_t copySeed(void *dest, void *src) {
    memcpy(dest, src, seedSize*sizeof(double));
    return seedSize*sizeof(double);
}

void Random::nextSubStream() {
    RngStream_ResetNextSubstream(&state);
}

void Random::resetSubStream() {
    copySeed(state.Cg, state.Bg);
}

double Random::next() {
    return RngStream_RandU01(&state);
};

int Random::next(int i, int j) {
    return RngStream_RandInt(&state, i, j);
}

size_t Random::load(void *buffer) {
    RngStream storage = (RngStream) buffer;
    state = *storage;
    return sizeof(struct RngStream_InfoState);
}

size_t Random::store(void *buffer) {
    RngStream storage = (RngStream) buffer;
    *storage = state;
    return sizeof(struct RngStream_InfoState);
}

size_t Random::storeStream(void *buffer) {
    return copySeed(buffer, state.Ig);
}

size_t Random::loadStream(void *buffer) {
    size_t result = copySeed(state.Ig, buffer);
    resetStream();
    return result;
}

size_t Random::storeSubStream(void *buffer) {
    return copySeed(buffer, state.Bg);
}

size_t Random::loadSubStream(void *buffer) {
    size_t result = copySeed(state.Bg, buffer);
    resetSubStream();
    return result;
}

Random::Random() {
    loadStream(nextSeed);
}

Random::Random(unsigned long seed[]) {
    RngStream_SetSeed(&state, seed);
}

Random::Random(const Random &original) {
    state = original.state;
}

void Random::nextStream() {
    advanceStream(state.Ig);
    resetStream();
}

void Random::resetStream() {
    copySeed(state.Bg, state.Ig);
    copySeed(state.Cg, state.Ig);
}

/*  internal generic-type uniform generator */
RandomUniformInterval::RandomUniformInterval(double _inf, double _sup)
    : inf(_inf), sup(_sup){};

double RandomUniformInterval::next() {
    return (inf + (sup - inf) * Random::next());
};
}
