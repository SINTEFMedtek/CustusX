/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

namespace cx
{

	/**
	 * \defgroup cx_resource Resource Layer
	 * \ingroup cx_base
	 *
	 * \brief A collection of common utility classes available to everyone.
	 *
	 * The Resource Layer consists of building blocks that are available to everyone.
	 * Everything here is/should be reusable, i.e. used by at least two other modules.
	 *
	 * The Core library is the basic component, all other libraries depend on that
	 * and are specialized in some way.
	 *
	 * In principle, this layer contains no global state, but a few exceptions exist:
	 *  - Reporter
	 *  - Settings
	 *  - XmlOptionFile
	 *
	 * Other notable content:
     * - A geometrical math library, using Eigen as the engine
     *   (Vector3D, Transform3D etc).
     * - Service interfaces.
     * - Interfaces for common objects: Data, Image, Mesh, VideoSource, Tool, ...
     * - Visualization components (View, Rep and descendants).
     * - A thread-safe logging service (Reporter).
     * - Persistent state, stored in ini files and xml files.
     *   (Settings, DataLocations, XmlOptionFile).
	 *
	 *
	 */

	/**
	 * \defgroup cx_resource_core Core
	 * \ingroup cx_resource
	 *
	 * \brief Core building blocks.
	 */

	/**
	 * \defgroup cx_resource_core_math Math
	 * \ingroup cx_resource_core
	 *
	 * \brief The CustusX Math library, based on Eigen.
	 *
	 * The math in CustusX should preferrably be done in terms of Eigen, http://eigen.tuxfamily.org/dox/
	 *
	 * The core classes Vector3D and Transform3D have been reduced to typedefs of
	 * Eigen structures.
	 *
	 */

	/**
	 * \defgroup cx_resource_core_tool Tool
	 * \ingroup cx_resource_core
	 *
	 * Tracking hardware, US probes and navigation pointers.
	 * Tool related interfaces and data classes.
	 *
	 * \see TrackingService
	 * \image html sscArchitecture_tracking.png "Basic relation between the Tool interfaces."
	 * \image html us_probe.png "Ultrasound Probe with Polaris tracking sensors attached."
	 *
	 */

	/**
	 * \defgroup cx_resource_core_data PatientModel
	 * \ingroup cx_resource_core
	 *
	 * \brief The virtual patient.
	 *
	 * A model of the patient. Data sets of various modalities, fiducials
	 * and labels are described in relation to each other.
	 *
	 * \see PatientModelService
	 * \image html sscArchitecture_data.png "Basic relation between the Data interfaces and classes."
	 *
	 */

	/**
	 * \defgroup cx_resource_core_algorithms Algorithms
	 * \ingroup cx_resource_core
	 *
	 * \brief In-house algorithms and adapters around external ones.
	 *
	 */

	/**
	 * \defgroup cx_resource_core_properties Property
	 * \ingroup cx_resource_core
	 *
	 * \brief Framework for a presenting interfaces to backend data.
	 *
	 */

	/**
	 * \defgroup cx_resource_core_logger Logger
	 * \ingroup cx_resource_core
	 *
	 * \brief Logging of status, errors, debug to console and file.
	 */

	/**
	 * \defgroup cx_resource_core_utilities Utilities
	 * \ingroup cx_resource_core
	 *
	 * \brief All basic unrelated functionality.
	 *
	 */

	/**
	 * \defgroup cx_resource_core_video Video
	 * \ingroup cx_resource_core
	 *
	 * \brief Video related interfaces and data classes.
	 * \see VideoService
	 *
	 */

	/**
	 * \defgroup cx_resource_core_settings Settings
	 * \ingroup cx_resource_core
	 *
	 * \brief Settings files and file system locations.
	 *
	 */

	/**
	 * \defgroup cx_resource_view View
	 * \ingroup cx_resource
	 *
	 * \brief Visualization classes.
	 * \see ViewService
	 *
	 */

	/**
	 * \defgroup cx_resource_view_rep Representations
	 * \ingroup cx_resource_view
	 *
	 * \brief Representations that visualize Data entities in a View.
	 *
	 * Visualization of underlying structures assembled in Representations that are
	 * easily configurable and insertible into views. View widgets that can be added
	 * directly to a Qt layout.
	 *
	 * Each Rep contains one kind of
	 * visualization. The point is to hide the vtk/OpenGL complexity and create a
	 * toolkit where Data, Tools, Reps and Views can be assembled.
	 *
	 */

	/**
	 * \defgroup cx_resource_view_rep2D Rep 2D
	 * \ingroup cx_resource_view_rep
	 *
	 * \brief Representations of 2D entities.
	 */

	/**
	 * \defgroup cx_resource_view_rep3D Rep 3D
	 * \ingroup cx_resource_view_rep
	 *
	 * \brief Representations of 3D entities.
	 */

	/**
	 * \defgroup cx_resource_view_repvideo Rep Video
	 * \ingroup cx_resource_view_rep
	 *
	 * \brief Representations of video entities.
	 */

	/**
	 * \defgroup cx_resource_OpenIGTLinkUtilities OpenIGTLink Utilities
	 * \ingroup cx_resource
	 *
	 * \brief Utilities for interfacing with OpenIGTLink.
	 *
	 */

	/**
	 * \defgroup cx_resource_videoserver Video Server
	 * \ingroup cx_resource
	 *
	 * \brief A server for receiving Video and US signals.
	 *
	 */

	/**
	 * \defgroup cx_resource_usreconstructiontypes US Reconstruction Types
	 * \ingroup cx_resource
	 *
	 * \brief Types used in relation to US reconstruction.
	 */

	/**
	 * \defgroup cx_resource_widgets Widgets
	 * \ingroup cx_resource
	 *
	 * \brief Basic widgets.
	 *
	 *
	 */

/**
 * \defgroup cx_resource_filter Filter
 * \ingroup cx_resource
 *
 * \brief Filter framework
 *
 *
 */

} // namespace cx

