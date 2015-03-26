CustusX
========
### A Research Platform for Image-Guided Therapy

CustusX is a Navigation System for Image-Guided Surgery. The intended audience is clinical and technological researchers in the fields of medical imaging and navigation. The platform can be used both as a customizable navigation system and as a toolkit for developing new applications.  The main focus of CustusX is the use of intraoperative Ultrasound. 

See [custusx.org](http://custusx.org) for more.

## Usage

DISCLAIMER: CustusX is a research tool: It is not intended for normal clinical use, and is not not FDA nor CE approved.

The code is free to download and use under a BSD-3 license. However, the code base is currently undergoing large changes, thus there is no guarantee that internal interfaces will be stable.

See [custusx.org](http://custusx.org) for [downloads](http://custusx.org/index.php/downloads) and [documentation](http://custusx.org/uploads/developer_doc/nightly/).

### Build instructions

[Build instructions available here.](http://custusx.org/uploads/developer_doc/nightly/build_instructions.html)

## Structure

CustusX is written in C++ using CMake, Qt, CTK, VTK, ITK, Eigen, OpenCV, IGSTK, OpenIGTLink and other libraries. The OSGi plugin framework implemented by CTK is used, and is the recommended way to extend CustusX yourself. The platform includes the CustusX application, but it is straightforward to build your own applications using the same platform.

## Contributors

CustusX is maintained by [SINTEF Medical Technology](http://www.sintef.no/home/Technology-and-Society/departments/medical-technology/#/), in cooperation with the [Norwegian National Competence Services for Ultrasound and Image-Guided Therapy](http://www.usigt.org/index.php/the-custusx-navigation-system) ([St. Olavs Hospital](http://www.stolav.no/en/), [SINTEF](http://www.sintef.no/home/Technology-and-Society/departments/medical-technology/#/), [NTNU](http://www.ntnu.edu/)). 

