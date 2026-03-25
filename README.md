# battery-limiter

`battery-limiter` is a small Linux C++ daemon that monitors battery percentage and triggers configured commands when charge thresholds are crossed.

## Features

- Monitors the first battery exposed under `/sys/class/power_supply`
- Uses upper and lower thresholds to avoid rapid toggling
- Can call custom commands to stop and resume charging
- Can send desktop notifications and optional sound alerts
- Ships with CMake build files, install helpers, and a systemd unit

## Project layout

```text
battery-limiter/
├── src/
├── include/
├── config/
├── packaging/
├── systemd/
├── CMakeLists.txt
└── README.md
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/battery-limiter
./build/battery-limiter config/default.json
```

## Configuration

`config/default.json` supports:

- `upper_limit`: stop threshold
- `lower_limit`: resume threshold
- `poll_interval_seconds`: polling period
- `enable_notifications`: toggle notifications
- `enable_sound`: toggle sound command
- `stop_charging_command`: shell command to disable charging
- `resume_charging_command`: shell command to re-enable charging
- `notify_command`: notification binary, usually `notify-send`
- `sound_command`: optional shell command to play a sound

Example vendor-specific charging commands depend on your laptop firmware or kernel driver. Keep them empty until you know the correct commands for your machine.

## Install

```bash
chmod +x packaging/install.sh packaging/uninstall.sh
./packaging/install.sh
```

## Notes

- The daemon does not hardcode a charging-control method because Linux battery charge control is hardware-specific.
- On many laptops you will need to configure `stop_charging_command` and `resume_charging_command` yourself.
