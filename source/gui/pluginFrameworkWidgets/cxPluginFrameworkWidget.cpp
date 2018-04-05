/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPluginBrowser.h"
#include "cxPluginFrameworkWidget.h"
#include "cxLogicManager.h"

namespace cx
{


PluginFrameworkWidget::PluginFrameworkWidget(QWidget* parent) :
	BaseWidget(parent, "plugin_framework_widget", "Plugin Framework"),
    mVerticalLayout(new QVBoxLayout(this))
{
	this->setToolTip("View available plugins");
	PluginFrameworkManagerPtr pluginFramework = LogicManager::getInstance()->getPluginFramework();
	mVerticalLayout->addWidget(new ctkPluginBrowser(pluginFramework));
}

PluginFrameworkWidget::~PluginFrameworkWidget()
{
}

} /* namespace cx */
