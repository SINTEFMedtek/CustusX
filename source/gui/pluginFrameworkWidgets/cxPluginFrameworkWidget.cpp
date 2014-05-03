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

#include "cxPluginFrameworkWidget.h"
#include "cxLogicManager.h"
#include "cxPluginBrowser.h"

namespace cx
{

PluginFrameworkWidget::PluginFrameworkWidget(QWidget* parent) :
    BaseWidget(parent, "PluginFrameworkWidget", "Plugin Framework"),
    mVerticalLayout(new QVBoxLayout(this))
{
	PluginFrameworkManagerPtr pluginFramework = LogicManager::getInstance()->getPluginFramework();
	mVerticalLayout->addWidget(new ctkPluginBrowser(pluginFramework));
}

PluginFrameworkWidget::~PluginFrameworkWidget()
{
}

QString PluginFrameworkWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Plugin Framework.</h3>"
      "<p>View the available plugins</p>"
      "<p>Plugins add functionality to the application</p>"
      "</html>";
}

} /* namespace cx */
