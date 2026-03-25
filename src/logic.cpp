#include "logic.h"

#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
std::string read_file(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("failed to open config file: " + path);
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

int read_int(const std::string& text, const std::string& key, int fallback) {
    const std::regex pattern("\"" + key + "\"\\s*:\\s*(-?\\d+)");
    std::smatch match;
    if (std::regex_search(text, match, pattern)) {
        return std::stoi(match[1].str());
    }
    return fallback;
}

bool read_bool(const std::string& text, const std::string& key, bool fallback) {
    const std::regex pattern("\"" + key + "\"\\s*:\\s*(true|false)");
    std::smatch match;
    if (std::regex_search(text, match, pattern)) {
        return match[1].str() == "true";
    }
    return fallback;
}

std::string read_string(const std::string& text, const std::string& key, const std::string& fallback) {
    const std::regex pattern("\"" + key + "\"\\s*:\\s*\"([^\"]*)\"");
    std::smatch match;
    if (std::regex_search(text, match, pattern)) {
        return match[1].str();
    }
    return fallback;
}
}

Logic::Logic(Config config) : config_(std::move(config)) {}

const Config& Logic::config() const {
    return config_;
}

Decision Logic::evaluate(const BatterySample& sample) {
    Decision decision;

    if (!sample.present || sample.capacity < 0) {
        decision.message = "Battery information is unavailable.";
        return decision;
    }

    if (sample.charging && sample.capacity >= config_.upper_limit && !charging_paused_) {
        charging_paused_ = true;
        decision.should_stop_charging = true;
        decision.message = "Battery reached upper limit (" + std::to_string(sample.capacity) + "%).";
        return decision;
    }

    if (charging_paused_ && sample.capacity <= config_.lower_limit) {
        charging_paused_ = false;
        decision.should_resume_charging = true;
        decision.message = "Battery dropped to lower limit (" + std::to_string(sample.capacity) + "%).";
        return decision;
    }

    return decision;
}

Config load_config(const std::string& path) {
    Config config;
    const auto text = read_file(path);

    config.upper_limit = read_int(text, "upper_limit", config.upper_limit);
    config.lower_limit = read_int(text, "lower_limit", config.lower_limit);
    config.poll_interval_seconds = read_int(text, "poll_interval_seconds", config.poll_interval_seconds);
    config.enable_notifications = read_bool(text, "enable_notifications", config.enable_notifications);
    config.enable_sound = read_bool(text, "enable_sound", config.enable_sound);
    config.stop_charging_command = read_string(text, "stop_charging_command", config.stop_charging_command);
    config.resume_charging_command = read_string(text, "resume_charging_command", config.resume_charging_command);
    config.notify_command = read_string(text, "notify_command", config.notify_command);
    config.sound_command = read_string(text, "sound_command", config.sound_command);

    validate_config(config);
    return config;
}

void validate_config(const Config& config) {
    if (config.lower_limit < 0 || config.upper_limit > 100) {
        throw std::runtime_error("battery thresholds must stay within 0-100");
    }
    if (config.lower_limit >= config.upper_limit) {
        throw std::runtime_error("lower_limit must be smaller than upper_limit");
    }
    if (config.poll_interval_seconds <= 0) {
        throw std::runtime_error("poll_interval_seconds must be positive");
    }
}
