#include <marto.h>
#include <iostream>

using namespace marto;

// Do test stuff
int main() {
    auto c=new Configuration();
    auto h=new EventsHistory(c);
    Event e;
    auto it=h->iterator();
    for (int i=0; i<10; i++) {
        it->storeNextEvent(&e);
    }
    return 0;
}
