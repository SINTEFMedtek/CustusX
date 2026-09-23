#!/bin/bash

##########################################################################################################
#
# CustusX install script
#
# Download this script from the releases page:
#   https://gitlab.sintef.no/custusx/CustusX/-/releases
#
# Then run:
#   chmod +x installCustusX.sh
#   ./installCustusX.sh
#
# The script downloads the CustusX release tarball automatically.
# If the package registry requires authentication, set your GitLab token first:
#   export GITLAB_TOKEN=your_personal_access_token
#
##########################################################################################################

set -e

# ---------------------------------------------------------------------------
# Retry a download a few times before giving up -- a single transient
# network/DNS hiccup shouldn't require rerunning the whole install script.
# ---------------------------------------------------------------------------
download_with_retry() {
    local attempt
    for attempt in 1 2 3 4 5; do
        if wget "$@"; then
            return 0
        fi
        if [ "$attempt" -lt 5 ]; then
            echo "Download attempt $attempt failed, retrying in 5s..."
            sleep 5
        fi
    done
    return 1
}

# ---------------------------------------------------------------------------
# Version — set by CI for each release; empty when run from a local checkout
# ---------------------------------------------------------------------------
CUSTUSX_VERSION=""

# GitLab project API base for package downloads
GITLAB_PROJECT_URL="https://gitlab.sintef.no/api/v4/projects/custusx%2FCustusX"

# ---------------------------------------------------------------------------
# Detect Ubuntu version
# ---------------------------------------------------------------------------
UBUNTU_VERSION=$(lsb_release -rs 2>/dev/null || echo "unknown")
echo "Detected Ubuntu version: $UBUNTU_VERSION"

case "$UBUNTU_VERSION" in
    20.04)
        echo "ERROR: Ubuntu 20.04 is no longer supported (CustusX#51). Please use Ubuntu 22.04 or 24.04."
        exit 1
        ;;
    22.04) OS="Ubuntu2204" ;;
    24.04) OS="Ubuntu2404" ;;
    *)
        echo "ERROR: Unsupported Ubuntu version: $UBUNTU_VERSION"
        echo "Supported versions: 22.04, 24.04"
        exit 1
        ;;
esac

# ---------------------------------------------------------------------------
# Install system packages
#
# libglew-dev: CustusX deliberately does not bundle GLEW into the release
# package (see gp_resolved_file_type_override() in CMake/cxInstallUtilities.cmake)
# since it must match the system's own OpenGL/driver stack, so it has to come
# from the system instead. libpcre2-16-0/libdouble-conversion3: transitive
# runtime deps of Qt5Core that are likewise not bundled.
# ---------------------------------------------------------------------------
sudo apt-get -y update
sudo apt-get -y install libglew-dev libpcre2-16-0 libdouble-conversion3 git wget unzip

# ---------------------------------------------------------------------------
# Find or download the CustusX release tarball
# ---------------------------------------------------------------------------
if [ -n "$CUSTUSX_VERSION" ]; then
    TARBALL="CustusX-${OS}.tar.gz"
    DOWNLOAD_URL="${GITLAB_PROJECT_URL}/packages/generic/CustusX/${CUSTUSX_VERSION}/${TARBALL}"

    echo "Downloading CustusX ${CUSTUSX_VERSION} for Ubuntu ${UBUNTU_VERSION}..."
    WGET_ARGS=()
    if [ -n "$GITLAB_TOKEN" ]; then
        WGET_ARGS+=(--header "PRIVATE-TOKEN: $GITLAB_TOKEN")
    fi
    if ! download_with_retry "${WGET_ARGS[@]}" -O "$TARBALL" "$DOWNLOAD_URL"; then
        echo ""
        echo "ERROR: Download failed. URL: $DOWNLOAD_URL"
        if [ -z "$GITLAB_TOKEN" ]; then
            echo "If the package registry requires authentication, set your token first:"
            echo "  export GITLAB_TOKEN=your_personal_access_token"
            echo "  ./installCustusX.sh"
        fi
        rm -f "$TARBALL"
        exit 1
    fi
else
    TARBALL=$(ls CustusX*.tar.gz 2>/dev/null | head -1)
    if [ -z "$TARBALL" ]; then
        echo "ERROR: No CustusX*.tar.gz found in the current directory."
        echo "Download the versioned installer from the releases page:"
        echo "  https://gitlab.sintef.no/custusx/CustusX/-/releases"
        exit 1
    fi
    echo "Using local tarball: $TARBALL"
fi

# ---------------------------------------------------------------------------
# Unpack to ~/CustusX/CustusX (~/CustusX is the shared family folder --
# venvs/models/Patients live there too, alongside CustusS's own
# ~/CustusX/CustusS app folder when that's installed)
# ---------------------------------------------------------------------------
if [ -d "CustusX_temp" ]; then
    rm -rf CustusX_temp
fi
mkdir CustusX_temp
tar -xzf "$TARBALL" -C CustusX_temp
# CPack's TGZ generator wraps the installed tree in a top-level directory named
# after the package (e.g. CustusX_26.08_Ubuntu24.04/), so the CustusX/ folder
# isn't always directly under CustusX_temp/ -- search for it instead of
# assuming a fixed depth.
CUSTUSX_ROOT=$(find CustusX_temp -mindepth 1 -maxdepth 2 -type d -name CustusX | head -1)
if [ -z "$CUSTUSX_ROOT" ]; then
    echo "ERROR: Could not find a CustusX folder inside the extracted tarball."
    exit 1
fi
# The new version is extracted above, so it's safe to now wipe any old
# install: ~/CustusX/CustusX is pure install payload for this app (no user
# state -- venvs/models/Patients live at the ~/CustusX family-folder level,
# shared with CustusS's own ~/CustusX/CustusS, and must not be touched here).
# A full wipe of just this app's subfolder avoids stale files from a previous
# version (e.g. an old/renamed plugin .so) lingering and getting loaded
# alongside the new set.
rm -rf ~/CustusX/CustusX
mkdir -p ~/CustusX/CustusX
cp -r "$CUSTUSX_ROOT"/* ~/CustusX/CustusX/
rm -rf CustusX_temp

# ---------------------------------------------------------------------------
# Shortcut to the (shared, family-level) Patients folder
# ---------------------------------------------------------------------------
mkdir -p ~/CustusX/Patients
if [ -d ~/Desktop ]; then
    cat > ~/Desktop/CustusX_Patients.desktop <<EOF
[Desktop Entry]
Type=Link
Name=CustusX Patients
Icon=folder
URL=$HOME/CustusX/Patients
EOF
    gio set ~/Desktop/CustusX_Patients.desktop metadata::trusted true 2>/dev/null || true
    chmod +x ~/Desktop/CustusX_Patients.desktop
fi

# ---------------------------------------------------------------------------
# Install desktop launcher
# ---------------------------------------------------------------------------
cd ~/CustusX/CustusX
if [ -f "CustusX.desktop" ] && [ -d ~/Desktop ]; then
    EXEC_PATH="$HOME/CustusX/CustusX/bin/CustusX"
    ICON_PATH="$HOME/CustusX/CustusX/icons/CustusX.png"
    sed -i "s|Path=.*|Path=$HOME/CustusX/CustusX/bin|g" CustusX.desktop
    sed -i "s|Exec=.*|Exec=$EXEC_PATH|g" CustusX.desktop
    sed -i "s|Icon=.*|Icon=$ICON_PATH|g" CustusX.desktop
    cp CustusX.desktop ~/Desktop/
    gio set ~/Desktop/CustusX.desktop metadata::trusted true 2>/dev/null || true
    chmod +x ~/Desktop/CustusX.desktop
fi

echo ""
echo "---------- CustusX installation complete ----------"
echo "Launch CustusX from the desktop shortcut (if created) or run:"
echo "  cd $HOME/CustusX/CustusX/bin && ./CustusX"
