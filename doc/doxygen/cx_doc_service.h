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
 * \defgroup cxService Service Layer
 * \brief A collection of singletons providing basic services:
 * 	      Patient Model, Tracking, Video and Visualization.
 *
 * The service layer consists of a collection of services that are of use to
 * the entire system. Each is a singleton. All except Visualization are
 * mutually independent, i.e. no horizontal connections exist in this layer.
 * Connections are managed in the logic layer. Detailed info on some of these
 * services can be found in Implementation 5.
 *
 *  - \ref cxServicePatient : Contains information related to the current patient.
 *    This includes entities of data, such as volumetric data, mesh data,
 *    landmarks, labels, and temporal data such as video streams. Spatial,
 *    hierarchical and temporal relations between these entities are also
 *    available.
 *  - \ref cxServiceTracking : An interface to the physical tracking devices,
 *    such as navigation pointers, US probes and surgical instruments.
 *  - \ref cxServiceVideo : An interface to realtime image stream sources such as
 *    endoscopic, ultrasound and fluoroscopy video, along with means to
 *    connect to them.
 *  - \ref cxServiceState :  Global application states: Application state 4.1,
 *    workflow state 4.2.
 *  - \ref cxServiceVisualization : Handles the layout and content of the views,
 *    i.e. visualization in 2D and 3D. This service uses the other services
 *    freely, for historical reasons.
 *
 *
 */

/**
 * \defgroup cxServicePatient Patient Service
 * \ingroup cxService
 * \brief The virtual patient.
 *
 * A model of the patient. Data sets of various modalities, fiducials
 * and labels are described in relation to each other.
 *
 * Note: This service currently is a hybrid between the old ssc::DataManager
 * and the new PatientService (they should be merged). The Patient Service
 * contains minimal functionality – The PatientData class. All other info
 * can be found in the old ssc::DataManager and the subclass cx::DataManager.
 *
 */

/**
 * \defgroup cxServiceTracking Tracking Service
 * \ingroup cxService
 * \brief Tracking hardware, US probes and navigation pointers.
 *
 * See \ref cx::ToolManager for a description of the service.
 *
 * \image html us_probe.png "Ultrasound Probe with Polaris tracking sensors attached."
 *
 */

/**
 * \defgroup cxServiceVideo Video Service
 * \ingroup cxService
 * \brief Video input.
 *
 * See \ref cx::VideoService for a description of the service.
 *
 */

/**
 * \defgroup cxServiceVisualization Visualization Service
 * \ingroup cxService
 * \brief 2D and 3D Visualization in Views.
 *
 * See \ref cx::ViewManager for a description of the service.
 *
 */

/**
 * \defgroup cxServiceVisualizationRep Visualization Representations
 * \ingroup cxServiceVisualization
 * \brief CX-specific versions of ssc::Rep .
 *
 */

/**
 * \defgroup cxServiceState State Service
 * \ingroup cxService
 * \brief Global application states
 *
 * See \ref cx::StateService for a description of the service.
 *
 */

} // namespace ssc

