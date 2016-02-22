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

#include "cxStateServiceImpl.h"

#include <iostream>
#include <QApplication>
#include <QByteArray>
#include <QDir>
#include "cxDefinitions.h"
#include "cxEnumConverter.h"
#include "cxXmlOptionItem.h"

#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxWorkflowStateMachine.h"
#include "cxCustusXWorkflowStateMachine.h"
#include "cxDataLocations.h"
#include "cxConfig.h"
#include "cxVLCRecorder.h"
#include "cxStateServiceBackend.h"

#include "cxTrackingServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxSpaceProviderImpl.h"
#include "cxVideoServiceProxy.h"
#include "cxApplicationsParser.h"
#include "cxProfile.h"
#include "cxLogger.h"

namespace cx
{

StateServiceImpl::StateServiceImpl(ctkPluginContext* context)
{
	this->initialize(this->createBackend(context));
}

StateServiceBackendPtr StateServiceImpl::createBackend(ctkPluginContext* context)
{
	TrackingServicePtr tracker = TrackingServiceProxy::create(context);
	PatientModelServicePtr pasm = PatientModelServiceProxy::create(context);
	SpaceProviderPtr spacer(new SpaceProviderImpl(tracker, pasm));
	VideoServicePtr video = VideoServiceProxy::create(context);

	StateServiceBackendPtr backend;
	backend.reset(new StateServiceBackend(tracker, video, spacer, pasm));
	return backend;
}

StateServiceImpl::~StateServiceImpl()
{
}

bool StateServiceImpl::isNull()
{
	return false;
}

void StateServiceImpl::initialize(StateServiceBackendPtr backend)
{
	mBackend = backend;
	this->fillDefaultSettings();

	ProfileManager::initialize();

    mWorkflowStateMachine.reset(new CustusXWorkflowStateMachine(mBackend));
	mWorkflowStateMachine->start();

	connect(mWorkflowStateMachine.get(), &WorkflowStateMachine::activeStateChanged, this, &StateServiceImpl::workflowStateChanged);
	connect(mWorkflowStateMachine.get(), &WorkflowStateMachine::activeStateAboutToChange, this, &StateServiceImpl::workflowStateAboutToChange);

	connect(ProfileManager::getInstance(), &ProfileManager::activeProfileChanged, this, &StateServiceImpl::applicationStateChanged);
}

QString StateServiceImpl::getApplicationStateName() const
{
	return ProfileManager::getInstance()->activeProfile()->getUid();
}

QStringList StateServiceImpl::getAllApplicationStateNames() const
{
	return ProfileManager::getInstance()->getProfiles();
}

QString StateServiceImpl::getVersionName()
{
	return QString("%1").arg(CustusX_VERSION_STRING);
}

QActionGroup* StateServiceImpl::getWorkflowActions()
{
	return mWorkflowStateMachine->getActionGroup();
}

WorkflowStateMachinePtr StateServiceImpl::getWorkflow()
{
	return mWorkflowStateMachine;
}

void StateServiceImpl::setWorkFlowState(QString uid)
{
	mWorkflowStateMachine->setActiveState(uid);
}

template<class T>
void StateServiceImpl::fillDefault(QString name, T value)
{
	settings()->fillDefault(name, value);
}

/**Enter all default Settings here.
 *
 */
void StateServiceImpl::fillDefaultSettings()
{
	this->fillDefault("Automation/autoStartTracking", true);
	this->fillDefault("Automation/autoStartStreaming", true);
	this->fillDefault("Automation/autoReconstruct", true);
	this->fillDefault("Automation/autoSelectActiveTool", true);
	this->fillDefault("Automation/autoSave", true);
	this->fillDefault("Automation/autoLoadRecentPatient", true);
	this->fillDefault("Automation/autoLoadRecentPatientWithinHours", 8);
	this->fillDefault("Automation/autoShowNewData", false);

	this->fillDefault("TrackingPositionFilter/enabled", false);

	this->fillDefault("renderingInterval", 33);
//	this->fillDefault("backgroundColor", QColor("black"));
	this->fillDefault("backgroundColor", QColor(30,60,70)); // a dark, grey-blue hue
	this->fillDefault("vlcPath", vlc()->getVLCPath());
	this->fillDefault("globalPatientNumber", 1);
	this->fillDefault("Ultrasound/acquisitionName", "US-Acq");
	this->fillDefault("Ultrasound/8bitAcquisitionData", false);
	this->fillDefault("Ultrasound/CompressAcquisition", true);
	this->fillDefault("View3D/sphereRadius", 1.0);
	this->fillDefault("View3D/labelSize", 2.5);
	this->fillDefault("Navigation/anyplaneViewOffset", 0.25);
	this->fillDefault("Navigation/followTooltip", true);
	this->fillDefault("Navigation/followTooltipBoundary", 0.1);

	this->fillDefault("showSectorInRTView", true);
    this->fillDefault("View/showOrientationAnnotation", true);
    this->fillDefault("View3D/stereoType", stFRAME_SEQUENTIAL);
	this->fillDefault("View3D/eyeAngle", 4.0);
	this->fillDefault("View/showDataText", true);
	this->fillDefault("View/showLabels", true);
	this->fillDefault("View/showMetricNamesInCorner", false);
	this->fillDefault("View3D/annotationModelSize", 0.2);
	this->fillDefault("View3D/annotationModel", "woman.stl");
	this->fillDefault("View3D/depthPeeling", false);
	this->fillDefault("View3D/ImageRender3DVisualizer", "vtkGPUVolumeRayCastMapper");
	this->fillDefault("View3D/maxRenderSize", 10 * pow(10.0,6));
	this->fillDefault("View/shadingOn", true);

	this->fillDefault("stillUpdateRate", 0.001);

#ifdef __APPLE__
	this->fillDefault("useGPU2DRendering", true);
#elif WIN32
	this->fillDefault("useGPU2DRendering", false);
#else
	this->fillDefault("useGPU2DRendering", true);
#endif

	this->fillDefault("optimizedViews", true);
	this->fillDefault("smartRender", true);

	this->fillDefault("IGSTKDebugLogging", false);
	this->fillDefault("giveManualToolPhysicalProperties", false);
	this->fillDefault("renderSpeedLogging", false);

	this->fillDefault("applyTransferFunctionPresetsToAll", false);

	this->fillDefault("USsimulation/type", "Original data");
	this->fillDefault("USsimulation/volume", "");
	this->fillDefault("USsimulation/gain", 0.70);
}


Desktop StateServiceImpl::getActiveDesktop()
{
	ApplicationsParser parser;
	return parser.getDesktop(mWorkflowStateMachine->getActiveUidState());
}

void StateServiceImpl::saveDesktop(Desktop desktop)
{
	ApplicationsParser parser;
	parser.setDesktop(mWorkflowStateMachine->getActiveUidState(),
					  desktop);
}

void StateServiceImpl::resetDesktop()
{
	ApplicationsParser parser;
	parser.resetDesktop(mWorkflowStateMachine->getActiveUidState());
}

} //namespace cx
