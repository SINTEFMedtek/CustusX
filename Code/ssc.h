#ifndef SSC_H_
#define SSC_H_

namespace ssc
{

/**
 * \mainpage
 *
 * \brief An IGT library used by Sonowand and Sintef Medical Technology.
 *
 *
 *
 * \section section_intro Introduction
 *
 * SSC is short for Sonowand / Sintef Medical Technology Cooperation. The
 * library was created in 2008 as a means to share code between the two
 * organizations. It has since been developed by both parties.
 *
 * The library contains a common set of interfaces that enables the
 * two companies to share software components. Please take that into account when
 * changing interfaces: You might not be the only user of a particular class.
 *
 * \image html Registration2.png "Example of ssc usage: CustusX during Registration"
 *
 *
 *
 * \section section_doc Documentation
 *
 * The <a href="modules.html">module list</a> is a recommended starting point.
 * The modules break down the library into manageable chunks. Also read the
 * \ref section_arch section.
 *
 * Both CustusX and Sonowand Invite documentation include ssc
 * documentation to some extent. Seek and find.
 *
 * Paper published at MICCAI 2011 on CustusX architecture (with ssc as a subset) can be found
 * <a href="http://hdl.handle.net/10380/3273">here</a> or
 * <a href="http://www.midasjournal.org/browse/publication/808">here</a>.
 *
 *
 * \section section_arch Architecture
 *
 * \image html sscArchitecture_top.png "SSC Structure"
 *
 * SSC is a collection of classes for use in an IGT application.
 *
 * The \ref sscMath module is used in the entire system.
 * \ref sscUtility and \ref sscWidget contains
 * unordered functionality. Some of it is used in the rest of ssc,
 * some is used only by client libraries.
 *
 * \ref sscData, \ref sscTool and \ref sscVideo primarily defines interfaces that
 * can be subclassed to specific implementations. Note that ssc is NOT a
 * standalone library. It is assumed that the client code implements several
 * interfaces in order to get up and running. These also modules contain some
 * functionality build in top of the interfaces.
 *
 * The \ref sscRep module primarily contains Rep (representation) subclasses.
 * They form the core of ssc. Each Rep is capable of visualizing entities
 * based on information from the data layer.
 *
 * The Reps must be added to a View in order to be shown on screen. A View
 * represents either a 2D or 3D (or Video) scene. A View cab be integrated into
 * a Qt application like any other QWidget.
 *
 * \image html sscArchitecture_view.png "View structure"
 *
 * Check out ToolRep3D and Texture3DSlicerRep for detailed examples of how a Rep is connected
 * to the View and data layer.
 *
 * SSC also contains secondary libraries for DICOM and ultrasound support. TBD.
 *
 *
 * \subsection section_update_policy Update policy
 *
 * The vtk rendering pipeline is updated when Render() in the vtkRenderWindow inside
 * View is called. The application using SSC is supposed to contain a QTimer or similar
 * that calls Render() at a certain update rate.
 *
 * The internal structure of SSC is updated whenever something changes, using Qt signals.
 * I.e, when a Data object changes, all Rep instances that use it will update immediately.
 *
 *
 *
 * \section section_legal Legal stuff
 *
 * SSC is owned by SINTEF MedTech and SonoWand AS. It can be used by both parties in
 * any way. A written agreement exists that regulates the usage.
 * SSC contains code created by other parties, under various licenses. This is
 * described within the source code at the appropriate places.
 *
 *
 *
 * \section section_install Installation
 *
 * The source can be downloaded from
 * \verbatim git@github.com:SINTEFMedisinskTeknologi/SSC.git \endverbatim
 *
 * Use
 * \verbatim git clone git@github.com:SINTEFMedisinskTeknologi/SSC.git \endverbatim
 * to clone the repository.
 *
 * You need to have a Github account and be added to the user
 * group by an administrator.
 *
 * Howtos on installation can be found on the CustusX wiki:
 * http://medtekserver.sintef.no/groups/custusx This is partly in
 * norwegian, and only available from inside Sintef.
 *
 * SSC compiles and runs on Mac, Linux and Windows.
 * Some features are available only one some of the OS.
 *
 * \section section_libs Libraries
 *
 * SSC is liberal in its use of libraries. They must be installed or
 * compiled on you system in order to make SSC compile.
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
		<TD>Not used yet, might be.</TD>
		<TD>No</TD>
	</TR>
	<TR>
		<TD>OpenIGTLink</TD>
		<TD>Used for communication with an external video source.</TD>
		<TD>No</TD>
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
		<TD>Experimental us reconstruction algorithm. Can be removed from compilation.</TD>
		<TD>Optional</TD>
	</TR>
	<TR>
		<TD>OpenCV</TD>
		<TD>Cross-platform video grabbing</TD>
		<TD>No</TD>
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
 * \section section_style Code style
 *
 * This is described in a separate document. TBD
 *
 *
 *
 * \section section_coords Coordinate Systems
 *
 * TBD
 *
 */


/**\namespace ssc
 * \brief An IGT library used by Sonowand and Sintef Medical Technology.
 *
 */

/**
 * \defgroup sscUtility SSC Utility
 * \brief Defines and miscellaneous utilities for the other parts of SSC.
 *
 */

/**
 * \defgroup sscWidget SSC Widget
 * \brief Widget collection.
 *
 */

/**
 * \defgroup sscMath SSC Math
 * \brief The SSC Math library, based on Eigen.
 *
 * The math in SSC should preferrably be done in terms of Eigen, http://eigen.tuxfamily.org/dox/
 *
 * The core classes Vector3D and Transform3D have been reduced to typedefs of
 * Eigen structures.
 *
 */

/**
 * \defgroup sscData SSC Data
 * \brief A model of the patient.
 *
 * Data sets of various modalities, fiducials and labels are described in relation to each other.
 *
 * \image html sscArchitecture_data.png "Basic relation between the Data interfaces and classes."
 *
 */

/**
 * \defgroup sscTool SSC Tool
 * \brief Tool related interfaces and data classes.
 *
 * \image html sscArchitecture_tracking.png "Basic relation between the Tool interfaces."
 *
 *
 */

/**
 * \defgroup sscVideo SSC Video
 * \brief Video related interfaces and data classes.
 *
 */

/**
 * \defgroup sscRep SSC Rep
 * \brief Representations that visualize Data entities in a View.
 *
 * Visualization of underlying structures assembled in Representations that are
 * easily configurable and insertible into views. View widgets that can be added
 * directly to a Qt layout.
 *
 * The representations are the core part of ssc. Each Rep contains one kind of
 * visualization. The point is to hide the vtk/OpenGL complexity and create a
 * toolkit where Data, Tools, Reps and Views can be assembled.
 *
 */



/** \defgroup sscProxy SSC Proxy
 *  \brief Helper classes for Rep subclasses.
 *
 *  \ingroup sscRep
 *
 *  SSC Proxy is a subgroup of \ref sscRep
 *
 *  In many cases, several reps share functionality. Instead of creating a complex
 *  inheritance hierarchy, this functionality is collected into reusable classes
 *  called proxies. A Rep will typically consist of proxies that provide reusable
 *  behaviour, and internal logic that is unique to that Rep. See below for examples.
 */

/** \defgroup sscRep2D SSC 2D Rep
 *  \brief All Rep subclasses that display in 2D views.
 *
 *  \ingroup sscRep
 *
 *  SSC 2D Rep is a subgroup of \ref sscRep
 *
 */

/** \defgroup sscRep3D SSC 3D Rep
 *  \brief All Rep subclasses that display in 3D views.
 *
 *  \ingroup sscRep
 *
 *  SSC 3D Rep is a subgroup of \ref sscRep
 *
 */

/** \defgroup sscRepVideo SSC Video Rep
 *  \brief All Rep subclasses that display in Video views.
 *
 *  \ingroup sscRep
 *
 *  SSC Video Rep is a subgroup of \ref sscRep
 *
 */

/** \defgroup sscNotUsed SSC Not used
 *  \brief All classes that seem to be unused in real code.
 *
 * Everything in this group are candidates for removal.
 *
 */


} // namespace ssc

#endif /*SSC_H_*/
