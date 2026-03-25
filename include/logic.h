#ifndef BATTERY_LIMITER_LOGIC_H
#define BATTERY_LIMITER_LOGIC_H

#include "monitor.h"

#include <string>

struct Config {
    int upper_limit = 80;
    int lower_limit = 40;
    int poll_interval_seconds = 30;
    bool enable_notifications = true;
    bool enable_sound = false;
    std::string stop_charging_command;
    std::string resume_charging_command;
    std::string notify_command = "notify-send";
    std::string sound_command;
};

struct Decision {
    bool should_stop_charging = false;
    bool should_resume_charging = false;
    std::string message;
};

class Logic {
public:
    explicit Logic(Config config);

    const Config& config() const;
    Decision evaluate(const BatterySample& sample);

private:
    Config config_;
    bool charging_paused_ = false;
};

Config load_config(const std::string& path);
void validate_config(const Config& config);

#endif
