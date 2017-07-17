#include <marto.h>
#include <iostream>

using namespace marto;

class JSQ2 : public Transition {
	Point *apply(Point *p, Event *ev) { //Event ev contains transition specification (parameters)
	// fromList is a random sequence of Queues (specified in ev) to prevent access to random generation and protect monotonicity
		marto::Parameters<Queue> fromList = ev->getParameters("from");
		marto::Parameters<Queue> toList = ev->getParameters("to");
		
		MartoQueue from = fromList->get(0);// the only source queue
		MartoQueue to0 = toList->get(0);// the first random destination queue
		MartoQueue to1 = toList->get(1);// second random destination queue 
		
		if (p->at(from) > 0) {
			if (p->at(to0) < p->at(to1))
				p->at(to0)++;
			else
				p->at(at(to1)++;
			p->at(at(from))--;
		}
		return p;
	}
};

// Do test stuff
int main() {
	Point p = new Point()
	std::cout << "Success" << std::endl;
	return 0;
}
