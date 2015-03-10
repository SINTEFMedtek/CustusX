CustusX
========
### A Research Platform for Image-Guided Therapy

CustusX is a Navigation System for Image-Guided Surgery. The intended audience is clinical and technological researchers in the fields of medical imaging and navigation. The platform can be used both as a customizable navigation system and as a toolkit for developing new applications.  The main focus of CustusX is the use of intraoperative Ultrasound. See [custusx.org](custusx.org) for more.

## Usage

DISCLAIMER: CustusX is a research tool: It is not intended for normal clinical use, and is not not FDA nor CE approved.

The code is free to download and use under a BSD-3 license. However, the code base is currently undergoing large changes, thus there is no guarantee that internal interfaces will be stable.

Binaries and documentation can be found here: [custusx.org](custusx.org)

### Build instructions

[Build instructions available here.](doc/dev_manual/cx_dev_build_instructions.md)

## Structure

CustusX is written in C++ using CMake, Qt, CTK, VTK, ITK, Eigen, OpenCV, IGSTK, OpenIGTLink and other libraries. The OSGi plugin framework implemented by CTK is used, and is the recommended way to extend CustusX yourself. The platform contains the CustusX application, but it is straightforward to build your own applications using the same platform.

## Contributors

CustusX is maintained by SINTEF Medical Technology, in cooperation with the Norwegian National Competence Services for Ultrasound and Image-Guided Therapy (St. Olavs Hospital, SINTEF, NTNU). 
