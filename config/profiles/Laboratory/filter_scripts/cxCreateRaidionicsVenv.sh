#!/bin/bash

# Create raidionics python virtual environment
# Usage:
# cxCreateRaidionicsVenv venvBasePath
# venvBasePath = Path to location where the venv should be craeted

venvBasePath=$1;

#if [ $1 -eq 0 ]; then
if [ -z $venvBasePath ]; then
  echo "No venvBasePath input. Setting it to ./"
  venvBasePath="./";
fi

cd "$venvBasePath";
sudo apt-get update
sudo apt install python3.8-venv
python3 -m venv raidionicsVenv
source raidionicsVenv/bin/activate
pip install --upgrade pip
pip install git+https://github.com/dbouget/raidionics-rads-lib.git
deactivate

