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
# venvBasePath is already the intended venv root (GenericScriptFilter derives it
# by stripping "bin/python" off the .ini's environment path), so the venv must be
# created directly in "." here - creating a nested "venv" subdir instead leaves
# the real venv one level too deep, and CustusX then can't find its python binary.
if [[ $reqPath == *"."* ]] || [[ $reqPath == *"/"* ]]; then # If using requirements.txt
  python3 -m venv .;
  source bin/activate;
  pip install --upgrade pip;
  python -m pip install -r "$reqPath/requirements.txt";
elif [[ $reqPath == "TotalSegmentator" ]]; then
  # TotalSegmentator requires Python >= 3.9. Ubuntu 20.04 ships python3.8, which
  # is too old, so pull in python3.10 via deadsnakes there. 22.04 (python3.10)
  # and 24.04 (python3.12) already satisfy this with their own system python3,
  # so use that directly instead of forcing a specific version that may not
  # exist/be installable on newer distros.
  pythonBin="python3"
  pythonMinor="$(python3 -c 'import sys; print(sys.version_info.minor)')"
  if [ "$pythonMinor" -lt 9 ]; then
    echo "System python3 (3.$pythonMinor) is too old for TotalSegmentator (needs >=3.9) - installing python3.10 via deadsnakes"
    sudo apt install -y software-properties-common #Needed for the deadsnakes PPA
    sudo add-apt-repository ppa:deadsnakes/ppa -y
    sudo apt install -y python3.10-venv #Also pulls in the python3.10 interpreter itself
    pythonBin="python3.10"
  fi

  if ! "$pythonBin" -m venv .; then
    echo "$pythonBin -m venv failed, trying to install ${pythonBin}-venv"
    sudo apt install -y "${pythonBin}-venv"
    "$pythonBin" -m venv .
  fi
  if [ ! -f bin/activate ]; then
    echo "ERROR: Could not create virtual environment at $(pwd) using $pythonBin"
    exit 1
  fi

  source bin/activate
  pip install --upgrade pip
  pip install TotalSegmentator
  totalseg_download_weights -t total
  totalseg_download_weights -t lung_vessels
else #Install other program, not tested
  python3 -m venv .;
  source bin/activate;
  pip install --upgrade pip;
  python -m pip install $reqPath;
fi

deactivate
