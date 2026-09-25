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
    # -t: if more than one matching tarball is sitting here (e.g. an old one
    # left over from before an OS upgrade, or from a previous manual
    # download), prefer the most recently modified one over an arbitrary
    # alphabetical pick.
    TARBALL=$(ls -t CustusX*.tar.gz 2>/dev/null | head -1)
    if [ -z "$TARBALL" ]; then
        echo "ERROR: No CustusX*.tar.gz found in the current directory."
        echo "Download the versioned installer from the releases page:"
        echo "  https://gitlab.sintef.no/custusx/CustusX/-/releases"
        exit 1
    fi
    # Local dev/CI builds encode the OS as e.g. "_Ubuntu22.04" (with a dot);
    # tagged releases encode it as "-Ubuntu2204" (no dot, matching $OS above).
    # Refuse a tarball built for a different Ubuntu version outright -- used
    # silently, it installs fine but fails at runtime with a confusing
    # missing-.so error (e.g. a 20.04 build's libGLEW.so.2.1 vs 22.04's
    # libGLEW.so.2.2), long after a clear error here would have helped.
    case "$TARBALL" in
        *"$OS"*|*"Ubuntu${UBUNTU_VERSION}"*) ;;
        *)
            echo "ERROR: $TARBALL does not look like it was built for Ubuntu $UBUNTU_VERSION."
            echo "Remove it and place a CustusX*${OS}*.tar.gz build here instead, then re-run."
            exit 1
            ;;
    esac
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
#
# Type=Application + an absolute Exec path, not Type=Link -- Ubuntu's GNOME
# Shell desktop-icons extension (which renders desktop icons, not Nautilus
# itself) rejects Type=Link entries outright ("Broken Desktop File") and also
# rejects a bare command name in Exec= (e.g. "xdg-open", relying on $PATH)
# with the same error, needing the executable's absolute path instead.
# ---------------------------------------------------------------------------
# xdg-user-dirs localizes the Desktop folder's name (e.g. ~/Skrivebord on a
# Norwegian install), so ~/Desktop doesn't reliably exist -- ask xdg-user-dir
# for the real path, falling back to ~/Desktop if xdg-user-dirs isn't set up.
DESKTOP_DIR="$(xdg-user-dir DESKTOP 2>/dev/null || true)"
if [ -z "$DESKTOP_DIR" ]; then
    DESKTOP_DIR="$HOME/Desktop"
fi

mkdir -p ~/CustusX/Patients
XDG_OPEN_PATH="$(command -v xdg-open || echo /usr/bin/xdg-open)"
if [ -d "$DESKTOP_DIR" ]; then
    cat > "$DESKTOP_DIR/CustusX_Patients.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=CustusX Patients
Icon=folder
Exec=$XDG_OPEN_PATH $HOME/CustusX/Patients
Terminal=false
EOF
    # chmod before gio set -- see the comment on the app shortcut below.
    chmod +x "$DESKTOP_DIR/CustusX_Patients.desktop"
    gio set "$DESKTOP_DIR/CustusX_Patients.desktop" metadata::trusted true 2>/dev/null || true
fi

# ---------------------------------------------------------------------------
# Install desktop launcher
# ---------------------------------------------------------------------------
cd ~/CustusX/CustusX
if [ -f "CustusX.desktop" ] && [ -d "$DESKTOP_DIR" ]; then
    EXEC_PATH="$HOME/CustusX/CustusX/bin/CustusX"
    ICON_PATH="$HOME/CustusX/CustusX/icons/CustusX.png"
    sed -i "s|Path=.*|Path=$HOME/CustusX/CustusX/bin|g" CustusX.desktop
    sed -i "s|Exec=.*|Exec=$EXEC_PATH|g" CustusX.desktop
    sed -i "s|Icon=.*|Icon=$ICON_PATH|g" CustusX.desktop
    cp CustusX.desktop "$DESKTOP_DIR/"
    # chmod before gio set: GNOME's desktop trust check only takes the
    # metadata::trusted flag into account for a file that's already
    # executable, so setting it first (against a not-yet-executable
    # freshly-copied file) doesn't stick -- Nautilus then renders it as
    # an untrusted/invalid launcher (broken icon, raw filename as label).
    chmod +x "$DESKTOP_DIR/CustusX.desktop"
    gio set "$DESKTOP_DIR/CustusX.desktop" metadata::trusted true 2>/dev/null || true
fi

echo ""
echo "---------- CustusX installation complete ----------"
echo "Launch CustusX from the desktop shortcut (if created) or run:"
echo "  cd $HOME/CustusX/CustusX/bin && ./CustusX"
