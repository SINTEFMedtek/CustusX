#!/bin/bash

# Create python virtual environment
# Usage:
# cxCreateVenv venvBasePath reqPath
# venvBasePath = Path to location where the venv should be created
# reqPath = Path to requirements.txt, or program name to install

venvBasePath=$1;
reqPath=$2;

#if [ $1 -eq 0 ]; then
if [ -z $venvBasePath ]; then
  echo "No venvBasePath input. Setting it to ./"
  venvBasePath="./";
fi

#if [ $2 -eq 0 ]; then
if [ -z $reqPath ]; then
  echo "No reqPath input. Setting it to $venvBasePath"
  reqPath=$venvBasePath;
fi

cd "$venvBasePath";
if [[ $reqPath == *"."* ]] || [[ $reqPath == *"/"* ]]; then # If using requirements.txt
  python3 -m venv venv;
  source venv/bin/activate;
  pip install --upgrade pip;
  python -m pip install -r "$reqPath/requirements.txt";
elif [[ $reqPath == "TotalSegmentator" ]]; then
  ubuntuVersion="$(lsb_release -rs)"
  if [[ $ubuntuVersion == *"20.04"* ]]; then
    echo "Ubuntu 20.04 - Installing Python 3.10 packages"
    sudo apt install software-properties-common -y #Needed for Python 3.10 on Ubuntu20.04
    sudo add-apt-repository ppa:deadsnakes/ppa -y #Needed for Python 3.10 on Ubuntu20.04
  fi
  sudo apt install -y python3.10-venv
  python3.10 -m venv venv
  source venv/bin/activate
  pip install --upgrade pip
  pip install TotalSegmentator
  totalseg_download_weights -t total
  totalseg_download_weights -t lung_vessels
else #Install other program, not tested
  python3 -m venv venv;
  source venv/bin/activate;
  pip install --upgrade pip;
  python -m pip install $reqPath;
fi

deactivate
