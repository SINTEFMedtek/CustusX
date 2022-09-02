#!/bin/bash

# Create python virtual environment
# Usage:
# cxCreateVenv venvBasePath reqPath
# venvBasePath = Path to location where the venv should be craeted
# reqPath = Path to requirements.txt

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
python3 -m venv venv;
source venv/bin/activate;
pip install --upgrade pip;
python -m pip install -r "$reqPath/requirements.txt";
deactivate
