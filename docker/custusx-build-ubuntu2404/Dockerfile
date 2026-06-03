# Docker image for building and testing CustusX on Ubuntu 24.04
#
# Build, tag, and upload on gitlab:
# For gitlab docker login see: https://gitlab.sintef.no/custusx/CustusS/-/wikis/Docker-image-creation
# sudo docker build --no-cache -t custusx-build-ubuntu2404:26.04.02 .
# sudo docker tag custusx-build-ubuntu2404:26.04.02 gitlab.sintef.no:5050/custusx/custusx/custusx-build-ubuntu2404:26.04.02
# sudo docker push gitlab.sintef.no:5050/custusx/custusx/custusx-build-ubuntu2404:26.04.02
#
# Explore container with the command line:
# sudo docker run --rm -it --entrypoint=/bin/bash custusx-build-ubuntu2404:26.04.02

FROM ubuntu:24.04

# Core build tools
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    git \
    cmake \
    ninja-build \
    python3 \
    python3-pip \
    python-is-python3 \
    python3-lxml \
    libssl-dev

# future 1.0.0+ is required for Python 3.12 (imp module removed); the apt package (0.18.x) is too old
RUN pip3 install future --break-system-packages

# Needed for Qt5 packages
RUN apt-get install -y software-properties-common && \
    add-apt-repository universe && \
    apt-get update

# Qt5
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y \
    qttools5-dev \
    qttools5-dev-tools \
    libqt5xmlpatterns5-dev \
    libqt5x11extras5-dev \
    qtmultimedia5-dev \
    libqt5serialport5-dev

# OpenGL and OpenCL
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y \
    opencl-headers \
    nvidia-opencl-dev \
    libglvnd-dev \
    libgl1-mesa-dev \
    libglew-dev \
    libxt-dev

# Boost
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y \
    libboost-all-dev \
    libcppunit-dev

# Media and streaming
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y \
    vlc \
    vlc-plugin-access-extra \
    gstreamer1.0-libav

# Documentation and CI tools
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y \
    doxygen \
    xvfb \
    curl

# Prepare build by downloading code (not needed if skipping build step below)
#RUN \
#  mkdir -p /builds/Ubuntu2404/CX && \
#  cd /builds/Ubuntu2404/CX && \
#  git clone https://gitlab.sintef.no/custusx/CustusX.git CX && \
#  cd CX && \
#  git checkout develop

# Compile and build package (not needed, just for verifying that Docker image are able to build CustusX)
#RUN \
#  cd /builds/Ubuntu2404/CX/CX && \
#  ./install/cxJenkinsMasterBuild.py --create_package

# Install and test package (not needed, just for testing previous step)
#RUN \
#  cd /builds/Ubuntu2404/CX/CX && \
#  xvfb-run ./install/cxJenkinsMasterBuild.py --test_package

WORKDIR /builds/Ubuntu2404/CX

# Build-time metadata as defined at http://label-schema.org
# BUILD_DATE is omitted to avoid unneeded rebuild
ARG IMAGE
ARG VCS_REF
ARG VCS_URL
LABEL org.label-schema.name=$IMAGE \
      org.label-schema.vcs-ref=$VCS_REF \
      org.label-schema.vcs-url=$VCS_URL \
      org.label-schema.schema-version="0.1"
