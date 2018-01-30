#include <marto.h>
#include <iostream>

using namespace marto;

// Do test stuff
int main() {
    Point *p = new Point();
    p->resize(3);
    for (int i = 0; i < 3; i++)
        p->at(i) = i + 1;
    Event *e = nullptr;
    std::cout << "Success" << std::endl;
    return 0;
}
