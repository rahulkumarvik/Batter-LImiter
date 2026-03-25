#ifndef BATTERY_LIMITER_MONITOR_H
#define BATTERY_LIMITER_MONITOR_H

#include <filesystem>
#include <string>

struct BatterySample {
    bool present = false;
    int capacity = -1;
    bool charging = false;
    std::string status;
    std::string battery_name;
};

class Monitor {
public:
    explicit Monitor(std::filesystem::path power_supply_root = "/sys/class/power_supply");

    bool initialize();
    BatterySample read() const;
    std::filesystem::path battery_path() const;

private:
    std::filesystem::path power_supply_root_;
    std::filesystem::path battery_path_;

    static std::string read_text_file(const std::filesystem::path& path);
};

#endif
