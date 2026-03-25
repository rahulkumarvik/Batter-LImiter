#include "event.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {
void run_command(const std::string& command) {
    if (command.empty()) {
        return;
    }
    std::system(command.c_str());
}

void send_notification(const Config& config, const std::string& title, const std::string& body) {
    if (!config.enable_notifications || config.notify_command.empty()) {
        return;
    }

    const std::string command = config.notify_command + " \"" + title + "\" \"" + body + "\"";
    run_command(command);
}

void play_sound(const Config& config) {
    if (!config.enable_sound || config.sound_command.empty()) {
        return;
    }
    run_command(config.sound_command);
}
}

EventDispatcher::EventDispatcher(Config config) : config_(std::move(config)) {}

void EventDispatcher::handle(const Decision& decision, const BatterySample& sample) const {
    if (!decision.should_stop_charging && !decision.should_resume_charging) {
        return;
    }

    std::cout << decision.message << " [" << sample.battery_name << ": " << sample.capacity << "%, "
              << sample.status << "]" << std::endl;

    if (decision.should_stop_charging) {
        run_command(config_.stop_charging_command);
        send_notification(config_, "Battery limiter", "Upper limit reached. Charging stop requested.");
        play_sound(config_);
    }

    if (decision.should_resume_charging) {
        run_command(config_.resume_charging_command);
        send_notification(config_, "Battery limiter", "Lower limit reached. Charging resume requested.");
        play_sound(config_);
    }
}
