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
  # TotalSegmentator needs Python >= 3.9: the system python3 on Ubuntu 22.04 (3.10) and 24.04 (3.12)
  pythonBin="python3"
  pythonMinor="$(python3 -c 'import sys; print(sys.version_info.minor)')"
  if [ "$pythonMinor" -lt 9 ]; then
    echo "ERROR: System python3 (3.$pythonMinor) is too old for TotalSegmentator (needs >= 3.9)."
    echo "Ubuntu 20.04 is not supported. Please use Ubuntu 22.04 or 24.04."
    exit 1
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
  # Pinned: TotalSegmentator depends on dipy (directly, and via fury<2) without a
  # version pin. dipy 1.12.0 still allows Python 3.10 but ships no cp310 wheel,
  # so on Ubuntu 22.04 (python3.10) pip picks it and builds it from source, which
  # needs Cython/meson and Python dev headers. 1.11.0 is the newest release with
  # prebuilt wheels for both cp310 (22.04) and cp312 (24.04); installing it first
  # means the TotalSegmentator install below leaves it alone. Same pin as
  # installFraxinus.sh/installFraxinusExcelsior.sh.
  pip install "dipy==1.11.0"
  # Pinned: TotalSegmentator has changed its CLI between releases (e.g. the
  # weights downloader moved from `python -m totalsegmentator.download_weights`
  # to the totalseg_download_weights console script), which silently broke the
  # Windows installer before (see installFraxinus.sh/CustusX#42). Bump this
  # deliberately, and re-check the totalseg_download_weights invocations below,
  # when updating.
  pip install "TotalSegmentator==2.18.0"
  # One task per class of structure CustusX's script filters use:
  # total -> LungLobes/Liver, lung_vessels -> LungVessels, lung_nodules ->
  # Nodules, liver_vessels/liver_lesions/liver_segments -> Liver.
  totalseg_download_weights -t total
  totalseg_download_weights -t lung_vessels
  totalseg_download_weights -t lung_nodules
  totalseg_download_weights -t liver_vessels
  totalseg_download_weights -t liver_lesions
  totalseg_download_weights -t liver_segments
else #Install other program, not tested
  python3 -m venv .;
  source bin/activate;
  pip install --upgrade pip;
  python -m pip install $reqPath;
fi

deactivate
