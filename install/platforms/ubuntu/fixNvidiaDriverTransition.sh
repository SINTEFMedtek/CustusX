#!/bin/bash
#
# fixNvidiaDriverTransition.sh
#
# Purpose:
#   Fix a broken/interrupted NVIDIA driver generation upgrade on Ubuntu,
#   which otherwise blocks installFraxinus.sh (or any "apt-get install")
#   with an "E: Unmet dependencies" error - and make sure the fix actually
#   leaves a working NVIDIA driver behind, not just an unblocked apt.
#
#   Symptom seen on Fraxinus laptops (Ubuntu 22.04, e.g. a Katana 17
#   B12VFK with an RTX 40-series Laptop GPU): running installFraxinus.sh -
#   or even a plain "sudo apt-get install <anything>" - fails with unmet
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
#   generation needs.
#
#   IMPORTANT (learned the hard way on the Katana 17 B12VFK): apt's
#   dependency solver can "fix" the unmet-dependency error by REMOVING
#   nvidia-driver-<N>, nvidia-dkms-<N> and dkms itself instead of
#   installing them, if it decides that's the smaller change - leaving
#   only the libnvidia-*-<N> support libraries in place. apt is then
#   happy (no unmet deps), but there is no NVIDIA kernel module on disk
#   and no DKMS to build one, so `nvidia-smi` fails and the GPU silently
#   falls back to nouveau. This is exactly what happened here: the first
#   run of this script unblocked apt but left the driver itself removed,
#   and it took a follow-up manual reinstall to actually restore
#   `nvidia-smi`. To prevent that recurring, this script now explicitly
#   (re)installs `dkms` + the correct `nvidia-driver-<N>` package by name
#   after the fix-broken step, forcing apt to converge on "driver
#   present" rather than "driver absent". The driver generation/flavor
#   (proprietary vs "-open") is auto-detected via `ubuntu-drivers
#   devices` (falling back to whatever libnvidia-compute-<N> generation
#   is already on disk), so no version number is hardcoded and this
#   should keep working across future driver generations (e.g. 580 ->
#   595) without editing this script.
#
# Usage:
#   chmod +x fixNvidiaDriverTransition.sh
#   ./fixNvidiaDriverTransition.sh
#
#   Safe to run even if nothing is actually broken - every step is a
#   no-op (or a no-op reinstall) in that case. Run this BEFORE
#   installFraxinus.sh if apt is reporting unmet nvidia dependencies, or
#   any time `nvidia-smi` has stopped working after an update.
#
#   A reboot is required afterwards if a new kernel module/driver was
#   installed (the script tells you at the end whether one was).
#
#   Secure Boot note: if this machine has a DKMS Secure Boot MOK key
#   already enrolled in firmware from a previous driver install (check
#   with `mokutil --list-enrolled`), the rebuilt module should get
#   signed automatically and no MOK-enrollment prompt/reboot is needed.
#   If no key is enrolled yet, expect a blue "MOK management" screen on
#   the next reboot asking you to enroll one - follow the on-screen
#   prompts (it will ask for a one-time password you set during install).
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

echo "--- Determining which driver package should be installed ---"
# Prefer ubuntu-drivers' recommendation - it knows proprietary vs "-open"
# for this specific GPU. Fall back to the highest libnvidia-compute-<N>
# generation already on disk, which is what's actually left behind after
# an interrupted transition (or after apt "fixed" things by removing the
# driver, per the note above).
RECOMMENDED_PKG=""
if command -v ubuntu-drivers >/dev/null 2>&1; then
    RECOMMENDED_PKG=$(ubuntu-drivers devices 2>/dev/null | awk '/^driver.*recommended/{print $3}' | head -n1) || true
fi

if [ -z "$RECOMMENDED_PKG" ]; then
    GEN=$(dpkg -l 2>/dev/null | awk '/^ii  libnvidia-compute-[0-9]/{print $2}' | grep -oE '[0-9]+$' | sort -n | tail -1) || true
    if [ -n "$GEN" ]; then
        RECOMMENDED_PKG="nvidia-driver-${GEN}"
    fi
fi

if [ -n "$RECOMMENDED_PKG" ]; then
    echo "--- Ensuring dkms + ${RECOMMENDED_PKG} are actually installed (not just apt being unblocked) ---"
    sudo apt-get install -y dkms "$RECOMMENDED_PKG"
else
    echo "Could not auto-detect the driver generation/package to install."
    echo "Run 'ubuntu-drivers devices' manually and install the recommended"
    echo "package (e.g. 'sudo apt-get install dkms nvidia-driver-580') if"
    echo "nvidia-smi still fails after this script finishes."
fi

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
echo "--- Sanity check: is dkms itself and a driver package actually installed now? ---"
DKMS_OK=false
DRIVER_OK=false
command -v dkms >/dev/null 2>&1 && DKMS_OK=true
dpkg -l 2>/dev/null | grep -qE '^ii  nvidia-driver-[0-9]' && DRIVER_OK=true

if [ "$DKMS_OK" = false ] || [ "$DRIVER_OK" = false ]; then
    echo "WARNING: dkms and/or an nvidia-driver-<N> package are still not"
    echo "installed. The fix did not fully converge - do not assume"
    echo "nvidia-smi will work after reboot. Investigate manually (check"
    echo "'apt-get install -f', 'apt-cache policy nvidia-driver-<N>')."
else
    echo "OK: dkms and an nvidia-driver-<N> package are both installed."
fi

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
