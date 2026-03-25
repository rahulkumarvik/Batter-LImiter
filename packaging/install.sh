#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"
PREFIX="${PREFIX:-/usr/local}"

cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$PREFIX"
cmake --build "$BUILD_DIR" --config Release
sudo cmake --install "$BUILD_DIR"

sudo install -Dm644 systemd/battery-limiter.service /etc/systemd/system/battery-limiter.service
sudo systemctl daemon-reload

echo "Installed battery-limiter to $PREFIX"
echo "Enable with: sudo systemctl enable --now battery-limiter.service"
