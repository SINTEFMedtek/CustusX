#!/bin/bash

#####################################################
# macOS setup script
#   Install all packages needed to:
#     * Develop in C++
#     * Run CustusX
#     * Several nice-to-have utilities.
#
#####################################################

# macOS Tahoe (16)
# Requires Homebrew: https://brew.sh
# Requires Xcode Command Line Tools: xcode-select --install

# Install Homebrew if not present
if ! command -v brew &> /dev/null; then
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

brew update
brew upgrade

# Xcode Command Line Tools (compiler, git, etc.)
xcode-select --install 2>/dev/null || true

# developer stuff
brew install openssl
brew install boost cppunit glew
brew install qt@5
brew install ninja
brew install openexr imath

# developer IDE's and apps
# brew install cmake   # Install manually
brew install --cask qt-creator  # Not needed, use vs code?

# doxygen
brew install doxygen gnuplot

# coverage testing
brew install lcov

# source control systems
brew install subversion cvs

# CI tools
brew install cppcheck

# python libs
brew install python3
pip3 install lxml

# used by IMFusion
brew install cryptopp nlopt

# utilities
brew install --cask gimp

# vlc
brew install --cask vlc

# medical image processing app
brew install --cask itk-snap

# GStreamer (for VLC integration and H.264 video)
brew install gstreamer gst-plugins-good gst-plugins-bad gst-libav

# OpenCL and OpenGL are provided by the macOS SDK — no extra packages needed

# Qt SerialPort is included in qt@5 — no extra package needed
