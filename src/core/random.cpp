#include <marto/random.h>
#include <marto/RngStream.h>

namespace marto {

size_t load(void *buffer) {
    RngStream storage = (RngStream) buffer;
    *generator = *storage;
    return sizeof(struct RngStream_InfoState);
}

size_t store(void *buffer) {
    RngStream storage = (RngStream) buffer;
    *storage = *generator;
    return sizeof(struct RngStream_InfoState);
}

static int seedSize=6;
static size_t copySeed(void *dest, void *src) {
    memcpy(dest, src, seedSize*sizeof(double));
    return seedSize*sizeof(double);
}

size_t storeStream(void *buffer) {
    return copySeed(buffer, generator->Ig);
}

size_t restoreStream(void *buffer) {
    copySeed(generator->Ig, buffer);
    copySeed(generator->Bg, generator->Ig);
    return copySeed(generator->Cg, generator->Ig);
}

size_t storeSubStream(void *buffer) {
    return copySeed(buffer, generator->Bg);
}

size_t restoreSubStream(void *buffer) {
    copySeed(generator->Bg, buffer);
    return copySeed(generator->Cg, generator->Bg);
}

Random::Random() {
    generator = nullptr;
}

Random *Random::nextStream() {
    Random *result = new Random();
    // TODO : to be synchronized for multithreading
    generator = RngStream_CreateStream(name);
}

Random::~Random() {
    RngStream_DeleteStream(generator);
}

Random::Random(const Random &original) {
    generator = new struct RngStream_InfoState();
    *generator = *original.generator;
    generator->name = strdup(original.generator->name);
}

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
