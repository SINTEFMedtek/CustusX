#!/bin/bash

# Create raidionics python virtual environment
# Usage:
# cxCreateRaidionicsVenv venvBasePath
# venvBasePath = Path to location where the venv should be created. Skip for . (current folder)

venvBasePath=$1;

#if [ $1 -eq 0 ]; then
if [ -z $venvBasePath ]; then
  echo "No venvBasePath input. Setting it to ./"
  venvBasePath="./";
fi

cd "$venvBasePath";
sudo apt-get -y update
sudo apt install -y python3.8-venv
# venvBasePath is already the intended venv root (GenericScriptFilter derives it
# by stripping "bin/python" off the .ini's environment path), so the venv must be
# created directly in "." here - creating a nested "raidionicsVenv" subdir instead
# leaves the real venv one level too deep, and CustusX then can't find its python binary.
python3 -m venv .
source bin/activate
pip install --upgrade pip
pip install git+https://github.com/dbouget/raidionics-rads-lib.git
deactivate

