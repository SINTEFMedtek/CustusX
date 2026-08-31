#!/bin/bash
#
# setup_nvidia_dkms_580.sh
#
# Purpose:
#   Install the NVIDIA 580-server driver via DKMS instead of Canonical's
#   precompiled per-kernel packages. DKMS rebuilds the kernel module locally
#   against whatever kernel is currently running, so it survives kernel
#   upgrades (including jumps to brand-new major kernel versions) without
#   waiting for Canonical to publish a matching precompiled module.
#
#   Background: on Ubuntu 24.04, "linux-modules-nvidia-580-server-open-<kernel>"
#   packages only exist for specific kernel ABIs that Canonical has built.
#   If apt/unattended-upgrades pulls in a newer kernel before a matching
#   module package exists, `nvidia-smi` fails with "couldn't communicate
#   with the NVIDIA driver" and modprobe reports the module is not found.
#   DKMS avoids this by compiling the module itself.
#
#   Open vs closed kernel module: newer NVIDIA GPUs (Ada/Blackwell/Hopper-class,
#   e.g. PCI ID 10de:2f58) only work with the "open" kernel module variant
#   ("-open" packages). The legacy closed/proprietary module builds and loads
#   fine but silently refuses to bind to these GPUs at runtime, producing
#   "No devices were found" plus an NVRM message in dmesg saying the open
#   kernel modules are required. This script installs the "-open" DKMS
#   variant accordingly; do not switch it back to the closed variant.
#
# Usage:
#   chmod +x setup_nvidia_dkms_580.sh
#   ./setup_nvidia_dkms_580.sh
#
#   A reboot is required afterwards.
#
# Verify after reboot:
#   nvidia-smi
#   dkms status
#
set -euo pipefail

DRIVER_BRANCH="580-server"

echo "=== NVIDIA ${DRIVER_BRANCH} DKMS setup ==="

echo "--- Updating package lists ---"
sudo apt update

echo "--- Ensuring kernel headers for the running kernel are installed ---"
# linux-headers-generic tracks whatever kernel package is installed,
# so future kernel upgrades keep matching headers available for DKMS.
sudo apt install -y linux-headers-generic

echo "--- Removing old 595/525 driver-generation leftovers ---"
# These have caused mixed-version breakage before.
sudo apt purge -y \
    'linux-modules-nvidia-595*' \
    'nvidia-kernel-common-595' \
    'nvidia-compute-utils-595' \
    'libnvidia-compute-595' \
    2>/dev/null || true

echo "--- Installing NVIDIA ${DRIVER_BRANCH} driver via DKMS (open kernel modules) ---"
# The closed variant's nvidia-dkms-*/nvidia-kernel-source-* packages Conflict
# with the open variant's (both provide the same virtual packages). Removing
# the closed packages (trailing "-") in the SAME apt-get invocation as
# installing the open ones lets the resolver handle both sides of the
# conflict, and the swap, atomically - even though the closed packages are
# held. Doing this as two separate commands (purge, then install) makes apt
# refuse the install with "unmet dependencies", since the resolver won't
# touch held packages across separate invocations.
sudo apt-get install -y --allow-change-held-packages \
    nvidia-driver-${DRIVER_BRANCH}-open \
    nvidia-kernel-source-${DRIVER_BRANCH}-open \
    nvidia-dkms-${DRIVER_BRANCH}-open \
    nvidia-driver-${DRIVER_BRANCH}- \
    nvidia-kernel-source-${DRIVER_BRANCH}- \
    nvidia-dkms-${DRIVER_BRANCH}-

echo "--- Removing any leftover precompiled ${DRIVER_BRANCH} kernel modules ---"
# DKMS builds its own module, so no per-kernel "linux-modules-nvidia-*"
# package should be needed; purge them if apt pulled any in as a dependency.
sudo apt purge -y \
    "linux-modules-nvidia-${DRIVER_BRANCH}-*" \
    2>/dev/null || true

echo "--- Cleaning up unused packages ---"
sudo apt autoremove -y

echo "--- Pinning the ${DRIVER_BRANCH} driver branch so it won't be silently ---"
echo "--- upgraded to a newer branch (e.g. 595, 610) by ubuntu-drivers/apt ---"
sudo apt-mark hold \
    nvidia-driver-${DRIVER_BRANCH}-open \
    nvidia-dkms-${DRIVER_BRANCH}-open \
    nvidia-kernel-source-${DRIVER_BRANCH}-open \
    nvidia-kernel-common-${DRIVER_BRANCH} \
    2>/dev/null || true

echo ""
echo "=== Setup complete ==="
echo "Currently held packages (won't be auto-upgraded):"
apt-mark showhold | grep -i nvidia || true
echo ""
echo "REBOOT REQUIRED. After rebooting, verify with:"
echo "    nvidia-smi"
echo "    dkms status"
echo ""
echo "dkms status should show an 'installed' entry for the driver version"
echo "against your current kernel, e.g.:"
echo "    nvidia-open/580.159.03, $(uname -r), x86_64: installed"
