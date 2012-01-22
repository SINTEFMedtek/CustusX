// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

namespace cx
{

/**
 * \mainpage
 *
 * \brief An IGT library.
 *
 *
 *
 * \section cx_section_intro Introduction
 *
 * This page describes the architecture of CustusX3.
 *
 * \image html metastase_mr_us_small.png "Neuro Navigation with MR and US"
 *
 *
 *
 * \section cx_section_other_sources Other Sources
 *
 * <a href="medtekserver.sintef.no/groups/custusx">The CustusX Wiki</a>
 * More information about installation, plans etc can be found there.
 *
 * <a href="medtekserver.sintef.no/mantis">Mantis Bug Tracker</a>
 *
 * Paper published at MICCAI 2011 on CustusX architecture can be found
 * <a href="http://hdl.handle.net/10380/3273">here</a> or
 * <a href="http://www.midasjournal.org/browse/publication/808">here</a>.
 *
 *
 *
 * \section cx_section_doc Documentation
 *
 * The <a href="modules.html">module list</a> is a recommended starting point.
 * The modules break down the library into manageable chunks. Also read the
 * \ref cx_section_arch section.
 *
 * Remember to check out the SSC documentation. It should be merged with the
 * CustusX doxygen somehow... TBD
 *
 *
 *
 * \section cx_section_arch Architecture
 *
 * The CustusX system is divided onto four layers: \ref cxGUI , \ref cxLogic ,
 * \ref cxService and \ref cxResource .
 *
 * <ul>
 * 	<li> \ref cxResource : A collection of common utility classes available to
 * 	  everyone. A math library. Much of the SSC library can be found here.
 * 	<li> \ref cxService : A collection of singletons providing basic services:
 * 	  Patient Model, Tracking, Video and Visualization. These provide core
 * 	  functionality to the system, along with system states.
 * 	<li> \ref cxLogic : Adds blocks of functionality on top of the services. A few core
 * 	  classes provide initialization of the services, everything else is
 * 	  added as plugins.
 * 	<li> \ref cxGUI : Graphical user interface (GUI) consisting of widgets,
 * 	  extensible by plugins.
 * </ul>
 *
 * \image html cxArchitecture_top_new.png "CX Structure"
 *
 * The layers are organized as a directed graph. A lower layer may never access
 * components from a higher layer. I.e: Logic may never control \ref cxGUI, but
 * \ref cxLogic may freely access \ref cxLogic (and \ref cxService + \ref
 * cxResource ). Objects within the same layer can interact, but this should be
 * kept to a minimum.
 *
 * The \ref cxPlugins extends the core by adding specific functionality. Examples of
 * plugins are: Segmentation, Registration, Data Acquisition, 3D US
 * Reconstruction and Calibration. They can be added by people not necessarily
 * part of the core team, and can depend on other plugins. A plugin typically
 * consist of a Logic part that uses the services to perform an operation, and
 * possibly GUI widgets for user interaction.
 *
 * \subsection cx_section_arch_external_resources External resources
 *
 * Not strictly a layer, but consider it as one below resources. All external
 * libraries used by CustusX can be used by the entire system. They are
 * described in 3.7. SSC (5.6) is an exception: It is so tightly integrated
 * into CustusX that it should be considered part of it.
 *
 *
 *
 * \section cx_section_legal Legal stuff
 *
 * CustusX is fully owned by SINTEF MedTech. CustusX source code and binaries
 * can only be used by SINTEF MedTech and those with explicit permission.
 * CustusX shall not be distributed to anyone else. In order to use CustusX,
 * a signed agreement with SINTEF must be made. SSC is owned by SINTEF MedTech
 * and SonoWand AS. It can be used by both parties in any way. For SINTEF
 * MedTech, the same rules apply as for CustusX.
 *
 * CustusX contains code created by other parties, under various licenses.
 * This is described within the source code at the appropriate places.
 *
 *
 * \section cx_section_install Installation
 *
 * Installation instructions can be found on the
 * <a href="medtekserver.sintef.no/groups/custusx">Wiki</a>.
 *
 *
 * \section cx_section_platforms Platforms
 *
 * CustusX compiles and runs on Mac, Linux and Windows.
 * Mac and Windows also has an installer.
 * Linux must be compiled on Fedora Linux.
 *
 *
 * \section cx_section_libs Libraries
 *
 * CX is liberal in its use of libraries. They must be installed or
 * compiled on you system in order to make CX compile.
 *
 * Adding more libraries: If a new library is needed, remember that is
 * must be available under the LGPL license or similar, and must run
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
		<TD>Cppunit</TD>
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
		<TD>Others</TD>
		<TD>Some other libraries are included directly in ssc/Code/3rdParty.
		This includes Eigen, the math library.</TD>
		<TD>Included</TD>
	</TR>
</TABLE>
 *
 *
 * \section cx_section_style Code style
 *
 * This is described under SSC. TBD
 *
 * \section cx_section_coordinate_systems Coordinate Systems
 *
 * See \ref page_coords.
 *
 */


} // namespace ssc

