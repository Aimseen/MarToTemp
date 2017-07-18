#include <marto/event.h>

template <typename T>
marto::Parameters<T> *marto::Event::getParameters(string name) {
	if (auto it = parameters.find(name)) {
		return dynamic_cast<T>(*it);
	}
}
