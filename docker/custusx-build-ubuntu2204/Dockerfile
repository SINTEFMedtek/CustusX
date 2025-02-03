# Docker image with prebuilt libraries and CustusX
# Useful docker commands.
# It seems like we need to use sudo with all commands on Ubuntu 22.04 for now

# Build, tag, and upload on gitlab
# For gitlab docker login (before last step) see:  https://gitlab.sintef.no/custusx/CustusS/-/wikis/Docker-image-creation
# sudo docker build -t gitlab.sintef.no:5050/custusx/custusx-build-ubuntu2204 .
# sudo docker tag custusx-build-ubuntu2204 gitlab.sintef.no:5050/custusx/custusx/custusx-build-ubuntu2204
# sudo docker push gitlab.sintef.no:5050/custusx/custusx/custusx-build-ubuntu2204

# List all docker container
# sudo docker ps --all

# Delete container
# sudo docker rm ...

# Explore container with the command line:
# sudo docker run --rm -it --entrypoint=/bin/bash custusx-build-ubuntu2204

FROM ubuntu:22.04

RUN \
  apt-get upgrade && apt-get update && \
  DEBIAN_FRONTEND=noninteractive \
  apt install -y \
  build-essential opencl-headers libssl-dev \
  libboost-all-dev libcppunit-dev libglew-dev \
  git python3 python3-pip python-is-python3 \
  cmake-qt-gui cmake cmake-curses-gui ninja-build \
  libgl1-mesa-dev

# libgl1-mesa-dev #Test with mesa OpenGL (instead of NVIDIA)

#Needed for Qt packages
RUN apt-get update --fix-missing \
    && apt-get install -y software-properties-common \
    && rm -rf /var/lib/apt/lists/* \
    && add-apt-repository universe

RUN \
  apt-get upgrade -y && apt-get update -y && \
  DEBIAN_FRONTEND=noninteractive \
  apt install -y \
  qttools5-dev qttools5-dev-tools qttools5-doc qt5-doc libqt5webkit5-dev libqt5xmlpatterns5-dev libqt5x11extras5-dev qtmultimedia5-dev libxt-dev

RUN pip install future

RUN \
  apt-get upgrade -y && apt-get update -y && \
  DEBIAN_FRONTEND=noninteractive \
  apt install -y \
  nvidia-opencl-dev libglvnd-dev \
  vlc vlc-plugin-access-extra gstreamer1.0-libav \
  python3-lxml \
  nvidia-driver-525 nvidia-dkms-525


# Install Xvfb - to be run from gitlab runner
RUN \
  apt-get upgrade -y && apt-get update -y && \
  DEBIAN_FRONTEND=noninteractive \
  apt install -y \
  doxygen gnuplot \
  xvfb gcovr curl


# Prepare gitlab build
# gitlab CI_PROJECT_DIR="/builds/custusx/CustusX"
RUN \
  mkdir -p /builds/custusx && \
  cd /builds/custusx && \
  git clone https://gitlab.sintef.no/custusx/CustusX.git && \
  cd CustusX && \
  git checkout develop

RUN \
  cd /builds/custusx/CustusX && \
  ./install/cxJenkinsMasterBuild.py --create_package


RUN \
  apt-get upgrade -y && apt-get update -y && \
  DEBIAN_FRONTEND=noninteractive \
  apt install -y \
  libqt5serialport5-dev

WORKDIR /builds/custusx/CustusX

# Build-time metadata as defined at http://label-schema.org
# BUILD_DATE is omitted to avoid unneeded rebuild
ARG IMAGE
ARG VCS_REF
ARG VCS_URL
LABEL org.label-schema.name=$IMAGE \
      org.label-schema.vcs-ref=$VCS_REF \
      org.label-schema.vcs-url=$VCS_URL \
      org.label-schema.schema-version="0.1"
