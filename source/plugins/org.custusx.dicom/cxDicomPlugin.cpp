/*
 * cxDicomPlugin.cpp
 *
 *  Created on: May 1, 2014
 *      Author: christiana
 */

#include "cxDicomPlugin.h"
#include <QLabel>
#include "cxDicomWidget.h"
#include <QResource>

namespace cx
{


DicomPlugin::DicomPlugin()
{
	QResource::registerResource("./Resources/ctkDICOM.qrc");
}

DicomPlugin::~DicomPlugin()
{
}

std::vector<PluginBase::PluginWidget> DicomPlugin::createWidgets() const
{
	std::vector<PluginWidget> retval;

	retval.push_back(PluginBase::PluginWidget(
			new DicomWidget(),
			"Plugins"));

	return retval;
}


} /* namespace cx */
