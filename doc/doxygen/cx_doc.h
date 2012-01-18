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

/**\namespace cx
 * \brief An IGT library used by Sintef Medical Technology.
 *
 */

/**
 * \defgroup cxResources CX Resources
 * \brief A collection of common utility classes available to everyone.
 *
 *  - This is classes that can be used by the entire system. Utility classes and
 *    functions of various kinds.
 *  - A geometrical math library, part of SSC, using Eigen as the engine
 *    (ssc::Vector3D, ssc::Transform3D etc).
 *  - Interfaces for common objects: ssc::Data, ssc::Image, ssc::Mesh,
 *    ssc::VideoSource, ssc::Tool, ...
 *  - Visualization components (ssc::Rep)
 *  - A thread-safe logging service (ssc::MessageManager). Singleton.
 *  - Persistent state, stored in ini files and xml files. Interface classes
 *    to these (cx::Settings, cx::DataLocations, ssc::XmlOptionFile).
 *
 */

/**
 * \defgroup cxService CX Service
 * \brief A collection of singletons providing basic services:
 * 	      Patient Model, Tracking, Video and Visualization.
 *
 * The service layer consists of a collection of services that are of use to
 * the entire system. Each is a singleton. All except Visualization are
 * mutually independent, i.e. no horizontal connections exist in this layer.
 * Connections are managed in the logic layer. Detailed info on some of these
 * services can be found in Implementation 5.
 *
 *  - Patient Service: Contains information related to the current patient.
 *    This includes entities of data, such as volumetric data, mesh data,
 *    landmarks, labels, and temporal data such as video streams. Spatial,
 *    hierarchical and temporal relations between these entities are also
 *    available.
 *  - Tracking Service: An interface to the physical tracking devices,
 *    such as navigation pointers, US probes and surgical instruments.
 *  - Video Service: An interface to realtime image stream sources such as
 *    endoscopic, ultrasound and fluoroscopy video, along with means to
 *    connect to them.
 *  - State Service: Global application states: Application state 4.1,
 *    workflow state 4.2.
 *  - Visualization Service: Handles the layout and content of the views,
 *    i.e. visualization in 2D and 3D. This service uses the other services
 *    freely, for historical reasons.
 *
 *
 */

/**
 * \defgroup cxLogic CX Logic Layer
 * \brief The logic layer add blocks of functionality on top of the services.
 *
 * A few core classes exist to provide initialization of the services
 * and connections between them. The rest of the functionality is
 * provided as plugins 3.2.
 *
 *  Check out \ref cx_page_plugins.
 */

/**
 * \defgroup cxGUI CX GUI
 * \brief The GUI layer is all the widgets that is available to the user.
 *
 * The GUI layer is all the widgets that is available to the user. The central
 * idea is the QMainWindow+DockWidgets designs, as described in 4.3. The
 * default MainWindow can be replaced by any other GUI if that is better for
 * your particular needs.
 *
 * Other utilities:
 *  - Core widgets: Widgets that enable core functionality.
 *  - Utility widgets: Widgets usable as building blocks elsewhere.
 *  - DataAdapters: Adapters that provide a common interface to getting/setting
 *    of backend data, such as strings, scalars and integers.
 *
 *  Plugins 3.2 also provide widgets for their particular functionality.
 *
 *  Check out \ref cx_page_plugins.
 *
 */

/** \defgroup cxNotUsed CX Not used
 *  \brief All classes that seem to be unused in real code.
 *
 * Everything in this group are candidates for removal.
 *
 */


} // namespace ssc

