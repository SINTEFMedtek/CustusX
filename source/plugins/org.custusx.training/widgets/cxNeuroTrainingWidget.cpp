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

//Test code
#include <QDockWidget>
#include <QMainWindow>

namespace cx
{

NeuroTrainingWidget::NeuroTrainingWidget(VisServicesPtr services, QWidget *parent) :
	TrainingWidget(services, "NeuroSimulatorWidget", "Neuro Simulator", parent)
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
	QString usUid = this->getFirstUSVolume();
	CX_LOG_DEBUG() << "Setting US simulator input to: " << usUid;
	settings()->setValue("USsimulation/volume", usUid);

	this->makeUnavailable("Kaisa");
	this->makeUnavailable("US", true);

}

void NeuroTrainingWidget::onRegisterStep()
{
    std::cout << "onRegisterStep" << std::endl;

	this->startTracking();
//	this->changeRegistrationWorkflowDesktop();//Can't get this to work
	this->changeWorkflowToRegistration();

//	this->addSimulatorWidget();//Can't get this to work

    //TODO:
    // Focus on the registration widget
    // Populate the registration widget
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
}

void NeuroTrainingWidget::changeRegistrationWorkflowDesktop()
{
//	Desktop desktop = Desktop("LAYOUT_3D_ACS", QByteArray::fromBase64(""));

	ApplicationsParser parser;

//	Desktop desktop = mServices->state()->getActiveDesktop();
	Desktop desktop = parser.getDesktop("RegistrationUid");

	QStringList toolbars;
	toolbars << "Workflow" << "Navigation" << "Tools" << "Screenshot" << "Desktop" << "RegistrationHistory" << "Help";

	//Copied from ApplicationsParser::addToolbarsToDesktop
	for (int i=0; i<toolbars.size(); ++i)
	{
		desktop.addPreset(toolbars[i]+"ToolBar", Qt::TopToolBarArea);
	}

	desktop.addPreset("org_custusx_registration_gui_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("RegistrationHistoryWidget", Qt::LeftDockWidgetArea, false);
	desktop.addPreset("NeuroSimulatorWidget", Qt::LeftDockWidgetArea, false);

//	desktop.mLayoutUid = "RegistrationUid";//Only if creating new

	parser.setDesktop("RegistrationUid", desktop);

}

void NeuroTrainingWidget::addSimulatorWidget()
{
	QMainWindow* mainwindow = dynamic_cast<QMainWindow*>(getMainWindow());
	if(!mainwindow)
		CX_LOG_WARNING() << "Cannot find MainWindow";
	else
		CX_LOG_DEBUG() << "Found MainWindow";

	//Test code from DynamicMainWindowWidgets::restorePreset()
	QString widgetName = "NeuroSimulatorWidget";
	QDockWidget* dw = mainwindow->findChild<QDockWidget*>(widgetName+"DockWidget");

	if(!dw)
		CX_LOG_WARNING() << "Cannot find dockwidget: " << widgetName;
	else
		CX_LOG_DEBUG() << "Found dockwidget: " << widgetName;

	dw->show();
	mainwindow->addDockWidget(Qt::LeftDockWidgetArea, dw);
}

void NeuroTrainingWidget::startTracking()
{
	triggerMainWindowActionWithObjectName("TrackingTools");
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
