#include "event.h"
#include "logic.h"
#include "monitor.h"

#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <thread>

int main(int argc, char** argv) {
    const std::string config_path = argc > 1 ? argv[1] : "config/default.json";

    try {
        Config config = load_config(config_path);
        Monitor monitor;
        if (!monitor.initialize()) {
            std::cerr << "No battery device found under /sys/class/power_supply" << std::endl;
            return 1;
        }

        Logic logic(config);
        EventDispatcher dispatcher(config);

        std::cout << "Monitoring battery " << monitor.battery_path().filename().string()
                  << " with limits " << config.lower_limit << "%-" << config.upper_limit << "%"
                  << std::endl;

        while (true) {
            const BatterySample sample = monitor.read();
            const Decision decision = logic.evaluate(sample);
            dispatcher.handle(decision, sample);
            std::this_thread::sleep_for(std::chrono::seconds(config.poll_interval_seconds));
        }
    } catch (const std::exception& ex) {
        std::cerr << "battery-limiter error: " << ex.what() << std::endl;
        return 1;
    }
}
