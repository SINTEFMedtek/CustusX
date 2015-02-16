/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


namespace cx
{

/**
 * \page
 *
 * \brief An IGT library.
 *
 *
 *
 * \section cx_section_intro Introduction
 *
 * This page describes the architecture of CustusX.
 *
 * The <a href="modules.html">module list</a> is a recommended starting point.
 * The modules break down the library into manageable chunks. Also read the
 * \ref cx_section_arch section.
 *
 *
 * \image html metastase_mr_us_small.png "Neuro Navigation with MR and US"
 *
 *
 *
 * \section cx_section_other_sources Other Sources
 *
 * <a href="http://medtek.sintef.no/wiki/projects/custusx/CustusX.html">The CustusX Wiki</a>
 * More information about installation, plans etc can be found there.
 *
 * <a href="http://medtek.sintef.no/mantis/">Mantis Bug Tracker</a>
 *
 * Paper published at MICCAI 2011 on CustusX architecture can be found
 * <a href="http://hdl.handle.net/10380/3273">here</a> or
 * <a href="http://www.midasjournal.org/browse/publication/808">here</a>.
 *
 *
 * \section cx_section_arch Architecture
 *
 * The CustusX system is divided onto four layers: \ref cx_gui , \ref cx_logic ,
 * \ref cx_service and \ref cx_resource .
 *
 * <ul>
 * 	<li> \ref cx_resource : A collection of common utility classes available to
 * 	  everyone. A math library. Much of the SSC library can be found here.
 * 	<li> \ref cx_service : A collection of singletons providing basic services:
 * 	  Patient Model, Tracking, Video and Visualization. These provide core
 * 	  functionality to the system, along with system states.
 * 	<li> \ref cx_logic : Adds blocks of functionality on top of the services. A few core
 * 	  classes provide initialization of the services, everything else is
 * 	  added as plugins.
 * 	<li> \ref cx_gui : Graphical user interface (GUI) consisting of widgets,
 * 	  extensible by plugins.
 * </ul>
 *
 * \image html cxArchitecture_top_new.png "CX Structure"
 *
 * The layers are organized as a directed graph. A lower layer may never access
 * components from a higher layer. I.e: Logic may never control \ref cx_gui, but
 * \ref cx_logic may freely access \ref cx_logic (and \ref cx_service + \ref
 * cx_resource ). Objects within the same layer can interact, but only according
 * to the rules of that layer.
 *
 * The \ref cx_modules extends the core by adding specific functionality. Examples of
 * plugins are: Segmentation, Registration, Data Acquisition, 3D US
 * Reconstruction and Calibration. They can be added by people not necessarily
 * part of the core team, and can depend on other plugins. A plugin typically
 * consist of a Logic part that uses the services to perform an operation, and
 * possibly GUI widgets for user interaction.
 *
 * All external libraries used by CustusX can be used by the entire system.
 *
 *
 *
 * \section cx_section_legal Legal stuff
 *
 * CustusX is fully owned by SINTEF Medical Technology. CustusX source code and binaries
 * can only be used by SINTEF MT and those with explicit permission.
 * CustusX shall not be distributed to anyone else. In order to use CustusX,
 * a signed agreement with SINTEF must be made.
 *
 * CustusX contains elements from the discontinued SSC library, which is jointly owned by
 * SINTEF MT and Sonowand AS.
 *
 * CustusX contains code created by other parties, under various licenses.
 * This is described within the source code at the appropriate places.
 *
 *
 * \section cx_section_install Installation
 *
 * Installation instructions can be found on the
 * <a href="http://medtek.sintef.no/wiki/projects/custusx/CustusX.html">internal SINTEF MT Wiki</a>.
 *
 *
 * \section cx_section_platforms Platforms
 *
 * CustusX compiles and runs on Mac, Linux and Windows.
 * Usually the two latest Mac OSX operating system versions is supported.
 * Linux is supported on Ubuntu. It has also been tested on Fedora.
 *
 *
 * \section cx_section_libs Libraries
 *
 * CX is liberal in its use of libraries. They must be installed or
 * compiled on you system in order to make CX compile.
 *
 * Adding more libraries: If a new library is needed, remember that is
 * must be available under the LGPL, BSD or similar license, and must run
 * on all platforms. It should also be easy to install on all platforms,
 * or be Cmake-compatible.
 *
<TABLE>
	<TR>
		<TH>Name</TH>
		<TH>Description</TH>
		<TH>Required</TH>
	</TR>
	<TR>
		<TD>CMake</TD>
		<TD>Used as the build system.</TD>
		<TD>Yes</TD>
	</TR>
	<TR>
		<TD>Boost</TD>
		<TD>Some primitives, such as shared_ptr, array.</TD>
		<TD>Yes</TD>
	</TR>
	<TR>
		<TD>Qt</TD>
		<TD>SSC depends heavily on Qt on all levels. Apart from the library classes, the use of signals and slots changes the c++ programming style dramatically.</TD>
		<TD>Yes</TD>
	</TR>
	<TR>
		<TD>VTK</TD>
		<TD>All visualization is based on vtk.</TD>
		<TD>Yes</TD>
	</TR>
	<TR>
		<TD>ITK</TD>
		<TD>Image processing</TD>
		<TD>Yes</TD>
	</TR>
	<TR>
		<TD>OpenIGTLink</TD>
		<TD>Used for communication with an external video source.</TD>
		<TD>Yes</TD>
	</TR>
	<TR>
		<TD>Catch</TD>
		<TD>for unit testing.</TD>
		<TD>Yes</TD>
	</TR>
	<TR>
		<TD>DCMTK</TD>
		<TD>DICOM integration.</TD>
		<TD>Optional</TD>
	</TR>
	<TR>
		<TD>OpenCL</TD>
		<TD>US reconstruction algorithm. Can be removed from compilation.</TD>
		<TD>Optional</TD>
	</TR>
	<TR>
		<TD>OpenCV</TD>
		<TD>Cross-platform video grabbing</TD>
		<TD>Yes</TD>
	</TR>
	<TR>
		<TD>Eigen</TD>
		<TD>Math library</TD>
		<TD>Yes</TD>
	</TR>
</TABLE>
 *
 *
 * \section cx_section_style Code style
 *
 * See document <a href="cx_code_standard.pdf" target="_blank"><b>Code Style</b></a>
 *
 *
 * \section cx_section_coordinate_systems Coordinate Systems
 *
 * See \ref ssc_page_coords.
 *
 */


} // namespace ssc

