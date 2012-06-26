// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxRegistrationPlugin.h"

#include "cxAcquisitionData.h"
#include "cxRegistrationManager.h"

#include "cxRegistrationHistoryWidget.h"
#include "cxRegistrationMethodsWidget.h"
#include "cxWirePhantomWidget.h"

namespace cx
{

RegistrationPlugin::RegistrationPlugin(AcquisitionDataPtr acquisitionData)
{
	mRegistrationManager.reset(new RegistrationManager(acquisitionData));
}

std::vector<PluginBase::PluginWidget> RegistrationPlugin::createWidgets() const
{
	std::vector<PluginWidget> retval;

	retval.push_back(PluginBase::PluginWidget(
			new RegistrationHistoryWidget(NULL),
			"Browsing"));
	retval.push_back(PluginBase::PluginWidget(
			new RegistrationMethodsWidget(mRegistrationManager, NULL, "RegistrationMethodsWidget", "Registration Methods"),
			"Algorithms"));
	retval.push_back(PluginBase::PluginWidget(
			new WirePhantomWidget(mRegistrationManager, NULL),
			"Algorithms"));

	return retval;
}

}
