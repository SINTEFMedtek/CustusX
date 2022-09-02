/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxApplicationsParser.h"

#include <iostream>
#include <QApplication>
#include <QByteArray>
#include <QDir>
#include "cxXmlOptionItem.h"

#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxWorkflowStateMachine.h"
#include "cxDataLocations.h"
#include "cxConfig.h"

#include "cxTrackingServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxSpaceProviderImpl.h"
#include "cxVideoServiceProxy.h"
#include "cxProfile.h"

namespace cx
{

ApplicationsParser::ApplicationsParser()
{
	Desktop desktop;

	QStringList standardToolbars;
	standardToolbars << "Workflow" << "Navigation" << "Tools" << "Screenshot" << "Desktop";


	//-----------------------------------------------------
	desktop = Desktop("LAYOUT_3D_ACS", QByteArray::fromBase64(""));
	QStringList toolbars;
	toolbars << standardToolbars << "Data" << "Help";
	this->addToolbarsToDesktop(desktop, toolbars);
	desktop.addPreset("active_mesh_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("volume_properties_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("slice_properties_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("dicom_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("export_data_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("import_widget", Qt::LeftDockWidgetArea, true);//The last one added becomes the active widget
	desktop.addPreset("console_widget", Qt::LeftDockWidgetArea, false);
	desktop.addPreset("help_widget", Qt::RightDockWidgetArea, false);
	mWorkflowDefaultDesktops["PatientDataUid"] = desktop;
	//-----------------------------------------------------

	//-----------------------------------------------------
	desktop = Desktop("LAYOUT_3D_ACS", QByteArray::fromBase64(""));
	toolbars.clear();
	toolbars << standardToolbars << "RegistrationHistory" << "Help";
	this->addToolbarsToDesktop(desktop, toolbars);
	desktop.addPreset("console_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("org_custusx_registration_gui_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("registration_history_widget", Qt::LeftDockWidgetArea, false);
	mWorkflowDefaultDesktops["RegistrationUid"] = desktop;
	//-----------------------------------------------------

	//-----------------------------------------------------
	desktop = Desktop("LAYOUT_3D_ACS", QByteArray::fromBase64(""));
	toolbars.clear();
	toolbars << standardToolbars << "Camera3DViews" << "Sampler" << "Help";
	this->addToolbarsToDesktop(desktop, toolbars);
	desktop.addPreset("active_mesh_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("volume_properties_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("slice_properties_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("console_widget", Qt::LeftDockWidgetArea, false);
	mWorkflowDefaultDesktops["PreOpPlanningUid"] = desktop;
	//-----------------------------------------------------

	//-----------------------------------------------------
	desktop = Desktop("LAYOUT_3D_ACS", QByteArray::fromBase64(""));
	toolbars.clear();
	toolbars << standardToolbars << "InteractorStyle" << "ToolOffset" << "Help";
	this->addToolbarsToDesktop(desktop, toolbars);
	mWorkflowDefaultDesktops["NavigationUid"] = desktop;
	//-----------------------------------------------------

	//-----------------------------------------------------
	desktop = Desktop("LAYOUT_US_Acquisition", QByteArray::fromBase64(""));
	toolbars.clear();
	toolbars << standardToolbars << "Help";
	this->addToolbarsToDesktop(desktop, toolbars);
	desktop.addPreset("igt_link_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("us_reconstruction", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("org_custusx_acquisition_widgets_acquisition", Qt::LeftDockWidgetArea, true);
	mWorkflowDefaultDesktops["IntraOpImagingUid"] = desktop;
	//-----------------------------------------------------

	//-----------------------------------------------------
	desktop = Desktop("LAYOUT_3D_ACS", QByteArray::fromBase64(""));
	toolbars.clear();
	toolbars << standardToolbars << "Sampler" << "Help";
	this->addToolbarsToDesktop(desktop, toolbars);
	desktop.addPreset("active_mesh_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("volume_properties_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("slice_properties_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("metric_widget", Qt::LeftDockWidgetArea, true);
	desktop.addPreset("console_widget", Qt::LeftDockWidgetArea, false);
	desktop.addPreset("playback_widget", Qt::BottomDockWidgetArea, false);
	mWorkflowDefaultDesktops["PostOpControllUid"] = desktop;
	//-----------------------------------------------------
}

void ApplicationsParser::addToolbarsToDesktop(Desktop& desktop, QStringList toolbars)
{
	for (int i=0; i<toolbars.size(); ++i)
	{
		desktop.addPreset(toolbars[i]+"ToolBar", Qt::TopToolBarArea);
	}
}

void ApplicationsParser::addDefaultDesktops(QString workflowStateUid, QString layoutUid, QString mainwindowstate)
{
	mWorkflowDefaultDesktops[workflowStateUid] = Desktop(layoutUid,
														 QByteArray::fromBase64(mainwindowstate.toLatin1()));
}

Desktop ApplicationsParser::getDefaultDesktop(QString workflowName)
{
	//TODO use applicationName!!!
	if (!mWorkflowDefaultDesktops.count(workflowName))
		return mWorkflowDefaultDesktops["DEFAULT"];
	return mWorkflowDefaultDesktops[workflowName];
}

Desktop ApplicationsParser::getDesktop(QString workflowName)
{
	Desktop retval;
	XmlOptionFile file = this->getSettings();
	QDomElement workflowElement = file.descend(workflowName).getElement();
	QDomElement desktopElement;
	if (workflowElement.namedItem("custom").isNull())
	{
		return this->getDefaultDesktop(workflowName);
	}
	else
	{
		desktopElement = workflowElement.namedItem("custom").toElement();
	}
	retval.mMainWindowState = QByteArray::fromBase64(desktopElement.attribute("mainwindowstate").toLatin1());
	retval.mLayoutUid = desktopElement.attribute("layoutuid");
	retval.mSecondaryLayoutUid = desktopElement.attribute("secondarylayoutuid");

	return retval;
}

void ApplicationsParser::setDesktop(QString workflowName, Desktop desktop)
{
	XmlOptionFile file = this->getSettings();
	QDomElement desktopElement = file.descend(workflowName).descend("custom").getElement();
	//	QDomElement desktopElement =
	//			mXmlFile.descend(applicationName).descend("workflows").descend(workflowName).descend("custom").getElement();
	desktopElement.setAttribute("mainwindowstate", QString(desktop.mMainWindowState.toBase64()));
	desktopElement.setAttribute("layoutuid", desktop.mLayoutUid);
	desktopElement.setAttribute("secondarylayoutuid", desktop.mSecondaryLayoutUid);
	file.save();
}

XmlOptionFile ApplicationsParser::getSettings()
{
	XmlOptionFile retval = ProfileManager::getInstance()->activeProfile()->getXmlSettings();
	retval = retval.descend("workflows");
	return retval;
}

void ApplicationsParser::resetDesktop(QString workflowName)
{
	XmlOptionFile file = this->getSettings();

	QDomElement workflowElement = file.descend(workflowName).getElement();
	workflowElement.removeChild(workflowElement.namedItem("custom"));
	file.save();
}


} //namespace cx
