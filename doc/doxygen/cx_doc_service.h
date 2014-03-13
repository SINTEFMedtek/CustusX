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
 * \defgroup cx_service Service Layer
 * \ingroup cx_base
 * \brief A collections of services each providing service within a special field.
 *
 * The service layer consists of a collection of services that are of use to
 * the entire system.
 *
 * All services are created in the \ref cx_logic and are thus available to the
 * entire \ref cx_gui and all \ref cx_plugins from there.
 *
 * Within the \ref cx_service, connection between services are limited. Each
 * service is initialized with a list of other services it has access to, refer
 * to each specific service for details.
 *
 *  - \ref cx_service_patient : Contains information related to the current patient.
 *    This includes entities of data, such as volumetric data, mesh data,
 *    landmarks, labels, and temporal data such as video streams. Spatial,
 *    hierarchical and temporal relations between these entities are also
 *    available.
 *  - \ref cx_service_tracking : An interface to the physical tracking devices,
 *    such as navigation pointers, US probes and surgical instruments.
 *  - \ref cx_service_video : An interface to realtime image stream sources such as
 *    endoscopic, ultrasound and fluoroscopy video, along with means to
 *    connect to them.
 *  - \ref cx_service_state :  Global application states: ApplicationState,
 *    WorkflowState.
 *  - \ref cx_service_visualization : Handles the layout and content of the views,
 *    i.e. visualization in 2D and 3D.
 *
 *
 */

/**
 * \defgroup cx_service_patient Patient Service
 * \ingroup cx_service
 * \brief The virtual patient.
 *
 * A model of the patient. Data sets of various modalities, fiducials
 * and labels are described in relation to each other.
 *
 * Note: This service currently is a hybrid between the old DataManager
 * and the new PatientService (they should be merged). The Patient Service
 * contains minimal functionality â the PatientData class. All other info
 * can be found in the DataManager.
 *
 */

/**
 * \defgroup cx_service_tracking Tracking Service
 * \ingroup cx_service
 * \brief Tracking hardware, US probes and navigation pointers.
 *
 * See \ref ToolManager for a description of the service.
 *
 * \image html us_probe.png "Ultrasound Probe with Polaris tracking sensors attached."
 *
 */

/**
 * \defgroup cx_service_video Video Service
 * \ingroup cx_service
 * \brief Video input.
 *
 * See \ref VideoService for a description of the service.
 *
 */

/**
 * \defgroup cx_service_visualization Visualization Service
 * \ingroup cx_service
 *
 * \brief 2D and 3D Visualization/Rendering engine
 *
 * See \ref ViewManager for a description of the service.
 *
 */

/**
 * \defgroup cx_service_state State Service
 * \ingroup cx_service
 * \brief Global application states
 *
 * See \ref StateService for a description of the service.
 *
 */

} // namespace ssc

