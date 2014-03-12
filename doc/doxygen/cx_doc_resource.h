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
 * \defgroup cx_resource Resource Layer
 * \ingroup cx_base
 *
 * \brief A collection of common utility classes available to everyone.
 *
 *  - This is classes that can be used by the entire system. Utility classes and
 *    functions of various kinds.
 *  - A geometrical math library, using Eigen as the engine
 *    (Vector3D, Transform3D etc).
 *  - Interfaces for common objects: Data, Image, Mesh,
 *    VideoSource, Tool, ...
 *  - Visualization components (Rep)
 *  - A thread-safe logging service (MessageManager). Singleton.
 *  - Persistent state, stored in ini files and xml files. Interface classes
 *    to these (Settings, DataLocations, XmlOptionFile).
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
 * \defgroup cx_resource_core Core
 * \ingroup cx_resource
 *
 * \brief Core building blocks.
 */

/**
 * \defgroup cx_resource_core_data Data
 * \ingroup cx_resource_core
 *
 * \brief A model of the patient.
 *
 * Data sets of various modalities, fiducials and labels are described in relation to each other.
 *
 * \image html sscArchitecture_data.png "Basic relation between the Data interfaces and classes."
 *
 */


/**
 * \defgroup cx_resource_core_dataadapters DataAdapters
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
 * \defgroup cx_resource_core_math Math
 * \ingroup cx_resource_core
 *
 * \brief The SSC Math library, based on Eigen.
 *
 * The math in SSC should preferrably be done in terms of Eigen, http://eigen.tuxfamily.org/dox/
 *
 * The core classes Vector3D and Transform3D have been reduced to typedefs of
 * Eigen structures.
 *
 */

/**
 * \defgroup cx_resource_core_tool Tool
 * \ingroup cx_resource_core
 *
 * \brief Tool related interfaces and data classes.
 *
 * \image html sscArchitecture_tracking.png "Basic relation between the Tool interfaces."
 *
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
 * \defgroup cx_resource_visualization Visualization
 * \ingroup cx_resource
 *
 * \brief Visualization classes.
 *
 */

/**
 * \defgroup cx_resource_visualization_rep Representations
 * \ingroup cx_resource_visualization
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
 * \defgroup cx_resource_visualization_rep2D Rep 2D
 * \ingroup cx_resource_visualization_rep
 *
 * \brief Representations of 2D entities.
 */

/**
 * \defgroup cx_resource_visualization_rep3D Rep 3D
 * \ingroup cx_resource_visualization_rep
 *
 * \brief Representations of 3D entities.
 */

/**
 * \defgroup cx_resource_visualization_repvideo Rep Video
 * \ingroup cx_resource_visualization_rep
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







} // namespace cx

