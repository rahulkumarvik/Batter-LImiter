#ifndef BATTERY_LIMITER_EVENT_H
#define BATTERY_LIMITER_EVENT_H

#include "logic.h"
#include "monitor.h"

class EventDispatcher {
public:
    explicit EventDispatcher(Config config);

    void handle(const Decision& decision, const BatterySample& sample) const;

private:
    Config config_;
};

#endif
