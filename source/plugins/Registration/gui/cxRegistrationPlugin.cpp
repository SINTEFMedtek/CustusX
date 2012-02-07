/*
 * cxRegistrationPlugin.cpp
 *
 *  \date Jun 16, 2011
 *      \author christiana
 */

#include "cxRegistrationPlugin.h"

//#include "cxCalibrationMethodsWidget.h"
#include "cxAcquisitionData.h"
#include "cxRegistrationManager.h"

#include "cxRegistrationHistoryWidget.h"
#include "cxRegistrationMethodsWidget.h"

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

	return retval;
}

}
