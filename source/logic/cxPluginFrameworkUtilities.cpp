/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPluginFrameworkUtilities.h"

namespace cx
{

namespace
{
static const QString uninstalled("UNINSTALLED");
static const QString installed("INSTALLED");
static const QString resolved("RESOLVED");
static const QString starting("STARTING");
static const QString stopping("STOPPING");
static const QString active("ACTIVE");
}

QString getStringForctkPluginState(const ctkPlugin::State state)
{
	switch(state)
	{
	case ctkPlugin::UNINSTALLED: return uninstalled;
	case ctkPlugin::INSTALLED: return installed;
	case ctkPlugin::RESOLVED: return resolved;
	case ctkPlugin::STARTING: return starting;
	case ctkPlugin::STOPPING: return stopping;
	case ctkPlugin::ACTIVE: return active;
	default: return QString("unknown");
	}
}

ctkPlugin::State getctkPluginStateForString(QString text)
{
	text = text.toUpper();

	if (text==uninstalled)
		return ctkPlugin::UNINSTALLED;
	if (text==installed)
		return ctkPlugin::INSTALLED;
	if (text==resolved)
		return ctkPlugin::RESOLVED;
	if (text==starting)
		return ctkPlugin::STARTING;
	if (text==stopping)
		return ctkPlugin::STOPPING;
	if (text==active)
		return ctkPlugin::ACTIVE;

	return ctkPlugin::UNINSTALLED;
}

} /* namespace cx */
