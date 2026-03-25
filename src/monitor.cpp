#include "monitor.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {
std::string trim(std::string value) {
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    value.erase(std::find_if(value.rbegin(), value.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), value.end());
    return value;
}
}

Monitor::Monitor(std::filesystem::path power_supply_root)
    : power_supply_root_(std::move(power_supply_root)) {}

bool Monitor::initialize() {
    if (!std::filesystem::exists(power_supply_root_)) {
        return false;
    }

    for (const auto& entry : std::filesystem::directory_iterator(power_supply_root_)) {
        if (!entry.is_directory()) {
            continue;
        }

        const auto type_file = entry.path() / "type";
        if (!std::filesystem::exists(type_file)) {
            continue;
        }

        if (trim(read_text_file(type_file)) == "Battery") {
            battery_path_ = entry.path();
            return true;
        }
    }

    return false;
}

BatterySample Monitor::read() const {
    if (battery_path_.empty()) {
        throw std::runtime_error("battery path is not initialized");
    }

    BatterySample sample;
    sample.present = true;
    sample.battery_name = battery_path_.filename().string();
    sample.status = trim(read_text_file(battery_path_ / "status"));
    sample.capacity = std::stoi(trim(read_text_file(battery_path_ / "capacity")));

    std::string lowered = sample.status;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    sample.charging = lowered == "charging";

    return sample;
}

std::filesystem::path Monitor::battery_path() const {
    return battery_path_;
}

std::string Monitor::read_text_file(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("failed to open " + path.string());
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}
