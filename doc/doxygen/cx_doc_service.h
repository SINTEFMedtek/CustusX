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
 * \defgroup cx_service Service Layer
 * \ingroup cx_base
 * \brief A collections of services each providing service within a special field.
 *
 * The service layer consists of a collection of services that are of use to
 * the entire system.
 *
 * All services are created in the \ref cx_logic and are thus available to the
 * entire \ref cx_gui and all \ref cx_modules from there.
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

