#!/usr/bin/env bash
set -euo pipefail

PREFIX="${PREFIX:-/usr/local}"

sudo systemctl disable --now battery-limiter.service 2>/dev/null || true
sudo rm -f /etc/systemd/system/battery-limiter.service
sudo systemctl daemon-reload

sudo rm -f "$PREFIX/bin/battery-limiter"
sudo rm -rf "$PREFIX/share/battery-limiter"

echo "Removed battery-limiter from $PREFIX"
