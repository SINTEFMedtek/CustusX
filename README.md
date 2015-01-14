CustusX
========
### A Research Platform for Image-Guided Therapy



The system aids surgeons during interventions, enabling the surgeon to navigate 3D image data using tracked instruments. Focus is on use of ultrasound in surgery.

## Usage

DISCLAIMER: CustusX is a research tool: It is not intended for normal clinical use, and is not not FDA nor CE approved.

The code is free to download and use under a BSD-3 license. However, the code base is currently undergoing large changes, thus there is no guarantee that internal interfaces will be stable.

### Build instructions

[Build instructions available here.](cx_user_build_instructions.md)

## Structure

CustusX is written in C++ using CMake, Qt, CTK, VTK, ITK, Eigen, OpenCV, IGSTK, OpenIGTLink and other libraries. The OSGi plugin framework implemented by CTK is used, and is the recommended way to extend CustusX yourself. The platform contains the CustusX application, but it is straightforward to build your own applications using the same platform.

## Contributors

CustusX is maintained by SINTEF Medical Technology, in cooperation with the Norwegian National Competence Services for Ultrasound and Image-Guided Therapy (St. Olavs Hospital, SINTEF, NTNU). The platform is used daily in research at St. Olavs Hospital and partners.

### Financial Contributors:

 - Norwegian National Competence Centre for Ultrasound and Image-Guided Therapy (Norwegian Ministry of Health and Care Services)
 - SINTEF
 - NorMIT – Norwegian Centre for Minimally Invasive Image Guided Therapy and Medical Technologies (The Research Council of Norway)
 - NorMIT-mini (Samarbeidsorganet Helse Midt-Norge - NTNU)
 - eMIT  (The Research Council of Norway)

## Links

- USIGT: http://www.usigt.no/index.php/the-custusx-navigation-system
- Midas Journal: http://hdl.handle.net/10380/3273
- SINTEF: http://www.sintef.no/SINTEF-Teknologi-og-samfunn/Om-oss/Medisinsk-teknologi/Ultralyd/Navigasjonssystemet-CustusX/

