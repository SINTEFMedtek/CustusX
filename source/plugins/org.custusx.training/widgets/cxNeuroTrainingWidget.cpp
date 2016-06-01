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

#include "cxNeuroTrainingWidget.h"
#include <boost/bind.hpp>
#include "cxApplication.h"
#include "cxSettings.h"
#include "cxLogger.h"
#include "cxVisServices.h"
#include "cxStateService.h"
#include "cxApplicationsParser.h"
#include "cxRegistrationWidget.h"
#include "cxStreamerServiceUtilities.h"

namespace cx
{

NeuroTrainingWidget::NeuroTrainingWidget(VisServicesPtr services, ctkPluginContext* context, QWidget *parent) :
	TrainingWidget(services, "NeuroSimulatorWidget", "Neuro Simulator", parent),
	mPluginContext(context)
{

    func_t transitionToStep1 = boost::bind(&NeuroTrainingWidget::onImport, this);
    func_t transitionToStep2 = boost::bind(&NeuroTrainingWidget::onRegisterStep, this);
    func_t transitionToStep3 = boost::bind(&NeuroTrainingWidget::onUse2DUSStep, this);
    func_t transitionToStep4 = boost::bind(&NeuroTrainingWidget::on3DUSAcqStep, this);

    TrainingWidget::registrateTransition(transitionToStep1);
    TrainingWidget::registrateTransition(transitionToStep2);
    TrainingWidget::registrateTransition(transitionToStep3);
    TrainingWidget::registrateTransition(transitionToStep4);

}

void NeuroTrainingWidget::onImport()
{
	this->setUSSimulatorInput();
	this->makeUnavailable("Kaisa");
	this->makeUnavailable("US", true);

	this->changeWorkflowToImport();
}

void NeuroTrainingWidget::setUSSimulatorInput()
{
	cx::StreamerService* streamerService = cx::StreamerServiceUtilities::getStreamerServiceFromType("ussimulator_streamer", mPluginContext);
	SimulatedStreamerService* simulatorStreamerService = dynamic_cast<SimulatedStreamerService*>(streamerService);
	if(simulatorStreamerService)
	{
		QString usUid = this->getFirstUSVolume();
		CX_LOG_DEBUG() << "Setting US simulator input to: " << usUid;
		simulatorStreamerService->setImageToStream(usUid);
	}
	else
		CX_LOG_WARNING() << "Cannot find SimulatedImageStreamerService";
}

void NeuroTrainingWidget::changeImageToPatientRegistrationToFast()
{
	QWidget* registrationWidget = findMainWindowChildWithObjectName<QWidget*>("org_custusx_registration_gui_widget");
	if(registrationWidget)
	{
		RegistrationWidget* widget =  dynamic_cast<RegistrationWidget*>(registrationWidget);
		if(widget)
		{
			widget->selectRegistrationMethod("ImageToPatient", "Fast");
		}
	}
}

void NeuroTrainingWidget::onRegisterStep()
{
    std::cout << "onRegisterStep" << std::endl;

	this->startTracking();

    //TODO:
    // Focus on the registration widget
    // Populate the registration widget

	this->changeWorkflowToRegistration();

	this->changeImageToPatientRegistrationToFast();

}

void NeuroTrainingWidget::onUse2DUSStep()
{
    std::cout << "onUse2DUSStep" << std::endl;
    //TODO: Change workflow and widgets

	this->changeWorkflowToUSAcquisition();
}

void NeuroTrainingWidget::on3DUSAcqStep()
{
    std::cout << "on3DUSAcqStep" << std::endl;
    //TODO: Change workflow and widgets

	this->changeWorkflowToUSAcquisition();
}

void NeuroTrainingWidget::startTracking()
{
	triggerMainWindowActionWithObjectName("TrackingTools");
}

void NeuroTrainingWidget::changeWorkflowToImport()
{
	mServices->state()->setWorkFlowState("PatientDataUid");
}

void NeuroTrainingWidget::changeWorkflowToRegistration()
{
	mServices->state()->setWorkFlowState("RegistrationUid");
}

void NeuroTrainingWidget::changeWorkflowToUSAcquisition()
{
	mServices->state()->setWorkFlowState("IntraOpImagingUid");
}

} // cx
