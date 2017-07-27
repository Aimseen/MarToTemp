#include <marto.h>
#include <iostream>
#include <string>

using namespace marto;

class JSQ2 : public Transition {
	Point *apply(Point *p, Event *ev) { //Event ev contains transition specification (parameters)
	// fromList is a random sequence of Queues (specified in ev) to prevent access to random generation and protect monotonicity
		//marto::ParameterValues<marto::Queue>
		auto *fromList = ev->getParameters("from");
		auto *toList = ev->getParameters("to");
		
		auto from = fromList->get<Queue>(0);// the only source queue
		auto to0 = toList->get<Queue>(0);// the first random destination queue
		auto to1 = toList->get<Queue>(1);// second random destination queue 
		
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

// Do test stuff
int main() {
	Point *p = new Point();
	p->resize(3);
	for (int i=0; i<3; i++)
		p->at(i) = i+1;
	Event *e = nullptr;
	std::cout << "Success" << std::endl;
	return 0;
}
