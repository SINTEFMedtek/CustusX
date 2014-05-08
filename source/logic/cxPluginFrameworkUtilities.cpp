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
