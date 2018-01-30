#include <marto/global.h>

namespace marto {

Configuration *Global::getConfig() {
    if (config == nullptr) {
        // lecture de la config ...
    }
    return config;
}

EventType *Configuration::getEventType(int num) {
    return eventTypesVector[num];
}

}
