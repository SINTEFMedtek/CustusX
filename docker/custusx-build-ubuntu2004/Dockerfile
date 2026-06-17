# Docker image with prebuilt libraries and CustusX
# Useful docker commands.
# It seems like we need to use sudo with all commands on Ubuntu 20.04 for now

# Build image:
# sudo docker build -t custusx-build-ubuntu2004:26.04.02 .
# or
# sudo docker build --no-cache -t custusx-build-ubuntu2004:26.04.02 .

# Tag image
# sudo docker tag custusx-build-ubuntu2004:26.04.02 gitlab.sintef.no:5050/custusx/custusx/custusx-build-ubuntu2004:26.04.02

# Upload image
# For gitlab docker login see:  https://gitlab.sintef.no/custusx/CustusS/-/wikis/Docker-image-creation
# sudo docker push gitlab.sintef.no:5050/custusx/custusx/custusx-build-ubuntu2004:26.04.02

# List all docker container
# sudo docker ps --all

# Delete container
# sudo docker rm ...

# Explore container with the command line:
# sudo docker run --rm -it --entrypoint=/bin/bash gitlab.sintef.no:5050/custusx/custusx/custusx-build-ubuntu2004:26.04.02

FROM ubuntu:20.04

#Required by CMake installation?
#ENV TZ=Europe/Oslo \
#RUN ln -snf "/usr/share/zoneinfo/$TZ" /etc/localtime \
#RUN echo "$TZ" > /etc/timezone \
#RUN apt-get install -y tzdata \

RUN \
  apt-get upgrade && apt-get update && \
  DEBIAN_FRONTEND=noninteractive \
  apt install -y \
  build-essential opencl-headers libssl-dev \
  libboost-all-dev libcppunit-dev libglew-dev \
  git python3 python3-pip python-is-python3 \
  cmake ninja-build \
  qt5-default qttools5-dev qttools5-dev-tools libqt5xmlpatterns5-dev libqt5x11extras5-dev qtmultimedia5-dev libxt-dev libqt5webkit5 \
  libgl1-mesa-dev

# libgl1-mesa-dev #Test with mesa OpenGL (instead of NVIDIA)

RUN \
  apt-get upgrade -y && apt-get update -y && \
  DEBIAN_FRONTEND=noninteractive \
  apt install -y \
  libqt5serialport5-dev

RUN pip install future

RUN \
  apt-get upgrade -y && apt-get update -y && \
  DEBIAN_FRONTEND=noninteractive \
  apt install -y \
  nvidia-opencl-dev libglvnd-dev \
  vlc vlc-plugin-access-extra gstreamer1.0-libav \
  python3-lxml

# apt install nvidia-driver-525 nvidia-dkms-525 #Test with some new nvidia drivers

# May also need to install nvidia drivers to get OpenCLUtilityLibrary to work?
# RUN nvidia-smi # Failed to initialize NVML: Unknown Error

RUN \
  apt-get upgrade -y && apt-get update -y && \
  DEBIAN_FRONTEND=noninteractive \
  apt install -y \
  doxygen \
  xvfb curl

# Ubuntu 20.04, from https://slicer.readthedocs.io/en/latest/developer_guide/build_instructions/linux.html
#RUN DEBIAN_FRONTEND=noninteractive apt install -y git subversion build-essential cmake cmake-curses-gui cmake-qt-gui qt5-default qtmultimedia5-dev qttools5-dev libqt5xmlpatterns5-dev libqt5svg5-dev qtwebengine5-dev qtscript5-dev qtbase5-private-dev libqt5x11extras5-dev libxt-dev
#RUN apt install -y ninja-build gawk

# Install Xvfb - to be run from gitlab runner


# These steps are not needed.
# They are just for verifying that Docker image are able to build CustusX
# Prepare gitlab build
# gitlab CI_PROJECT_DIR="/builds/custusx/CustusX"
#RUN \
#  mkdir -p /builds/Ubuntu2004/CX && \
#  cd /builds/Ubuntu2004/CX && \
#  git clone https://gitlab.sintef.no/custusx/CustusX.git CX && \
#  cd CX && \
#  git checkout develop

#RUN \
#  cd /builds/Ubuntu2004/CX/CX && \
#  ./install/cxJenkinsMasterBuild.py --create_package

WORKDIR /builds/Ubuntu2004/CX

# Build-time metadata as defined at http://label-schema.org
# BUILD_DATE is omitted to avoid unneeded rebuild
ARG IMAGE
ARG VCS_REF
ARG VCS_URL
LABEL org.label-schema.name=$IMAGE \
      org.label-schema.vcs-ref=$VCS_REF \
      org.label-schema.vcs-url=$VCS_URL \
      org.label-schema.schema-version="0.1"
