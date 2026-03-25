#include "event.h"
#include "logic.h"
#include "monitor.h"

#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <thread>

namespace {
int prompt_threshold(const std::string& label, int current_value) {
    while (true) {
        std::cout << label << " [" << current_value << "]: ";

        std::string line;
        if (!std::getline(std::cin, line)) {
            return current_value;
        }

        if (line.empty()) {
            return current_value;
        }

        try {
            const int value = std::stoi(line);
            if (value < 0 || value > 100) {
                std::cout << "Please enter a number between 0 and 100." << std::endl;
                continue;
            }
            return value;
        } catch (const std::exception&) {
            std::cout << "Please enter a valid number." << std::endl;
        }
    }
}
}

int main(int argc, char** argv) {
    const std::string config_path = argc > 1 ? argv[1] : "config/default.json";

    try {
        Config config = load_config(config_path);
        std::cout << "Enter battery thresholds. Press Enter to keep the value from config." << std::endl;
        config.upper_limit = prompt_threshold("Max threshold", config.upper_limit);
        config.lower_limit = prompt_threshold("Min threshold", config.lower_limit);
        validate_config(config);

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
