#!/bin/bash
#
# fixNvidiaDriverTransition.sh
#
# Purpose:
#   Fix a broken/interrupted NVIDIA driver generation upgrade on Ubuntu,
#   which otherwise blocks installFraxinus.sh (or any "apt-get install")
#   with an "E: Unmet dependencies" error.
#
#   Symptom seen on a Fraxinus laptop (Ubuntu 22.04, e.g. a Katana 17
#   B12VFK with an RTX 4050 Laptop GPU): running installFraxinus.sh - or
#   even a plain "sudo apt-get install <anything>" - fails with unmet
#   dependencies listing several nvidia-*/libnvidia-*-<N> packages that
#   depend on an exact NVIDIA driver package version reported as "not
#   installed" / "not going to be installed", even though that exact
#   version is available in the configured repos.
#
#   Root cause: Canonical ships NVIDIA driver generation upgrades (e.g.
#   535 -> 580) by turning the old generation's packages into empty
#   "transitional" packages that depend on the matching new-generation
#   package and hand off to it. If that handoff is interrupted (an
#   apt/dpkg run gets killed, the machine loses power/is force-rebooted
#   mid-upgrade, etc.), the old generation's transitional packages are
#   left "unpacked but not configured" (dpkg status "iU") and apt then
#   refuses to touch ANY nvidia-related package - including completing
#   the handoff - until that is resolved.
#
#   `dpkg --configure -a` + `apt --fix-broken install` lets dpkg/apt
#   finish the interrupted handoff and pull in whatever the new driver
#   generation needs. This is the actual fix that resolved it - no
#   manual package name/version wrangling was needed, and none is
#   hardcoded here, so this should keep working for future driver
#   generations (e.g. 580 -> 595) without editing this script.
#
# Usage:
#   chmod +x fixNvidiaDriverTransition.sh
#   ./fixNvidiaDriverTransition.sh
#
#   Safe to run even if nothing is actually broken - every step is a
#   no-op in that case. Run this BEFORE installFraxinus.sh if apt is
#   reporting unmet nvidia dependencies.
#
#   A reboot is required afterwards if a new kernel module/driver was
#   installed (the script tells you at the end whether one was).
#
# Verify after reboot:
#   nvidia-smi
#   dkms status
#
set -euo pipefail

echo "=== NVIDIA driver transition fix ==="

echo "--- nvidia/libnvidia packages before ---"
dpkg -l | grep -i nvidia || true

# Hybrid (Intel + NVIDIA) laptops use nvidia-prime for GPU switching. It can
# get removed as a side effect of the driver package swap below; note
# whether it was present so we can reinstall it afterwards if so.
HAD_NVIDIA_PRIME=false
if dpkg -s nvidia-prime >/dev/null 2>&1; then
    HAD_NVIDIA_PRIME=true
fi

echo "--- Updating package lists ---"
sudo apt-get update -y

echo "--- Finishing any interrupted package configuration ---"
sudo dpkg --configure -a || true

echo "--- Letting apt resolve and complete the broken driver transition ---"
sudo apt-get --fix-broken install -y

if [ "$HAD_NVIDIA_PRIME" = true ] && ! dpkg -s nvidia-prime >/dev/null 2>&1; then
    echo "--- Reinstalling nvidia-prime (removed by the driver package swap) ---"
    sudo apt-get install -y nvidia-prime
fi

echo "--- Cleaning up leftover packages from the old driver generation ---"
sudo apt-get autoremove -y

echo ""
echo "--- nvidia/libnvidia packages after ---"
dpkg -l | grep -i nvidia || true

echo ""
echo "=== Fix complete ==="
if [ -f /var/run/reboot-required ]; then
    echo "A reboot IS required (a new kernel module/driver was installed)."
else
    echo "No reboot flagged as required, but rebooting is still the safest"
    echo "way to confirm the new driver/kernel module actually loads."
fi
echo ""
echo "Reboot, then verify with:"
echo "    nvidia-smi"
echo "    dkms status"
echo ""
echo "Once that looks good, (re-)run installFraxinus.sh."
