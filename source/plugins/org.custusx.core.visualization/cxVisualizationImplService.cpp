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

#include "cxVisualizationImplService.h"

#include <ctkPluginContext.h>
#include "cxViewManager.h"
#include "cxLegacySingletons.h"
#include "cxViewGroup.h"

namespace cx
{

VisualizationImplService::VisualizationImplService(ctkPluginContext *context) :
	mContext(context )
{
	if(!viewManager())
		std::cout << "VisualizationImplService got no viewManager" << std::endl;
//	connect(viewManager(), SIGNAL(activeViewChanged()), this, SIGNAL(activeViewChanged()));
//	connect(viewManager(), &ViewManager::renderingEnabledChanged, this, &VisualizationService::renderingEnabledChanged);

	connect(viewManager(), &ViewManager::activeViewChanged, this, &VisualizationService::activeViewChanged);
	connect(viewManager(), &ViewManager::fps, this, &VisualizationService::fps);
	connect(viewManager(), &ViewManager::activeLayoutChanged, this, &VisualizationService::activeLayoutChanged);
	connect(viewManager(), &ViewManager::renderingEnabledChanged, this, &VisualizationService::renderingEnabledChanged);
}

VisualizationImplService::~VisualizationImplService()
{
//	if(viewManager())
//	{
//		disconnect(viewManager(), SIGNAL(activeViewChanged()), this, SIGNAL(activeViewChanged()));
//		disconnect(viewManager(), &ViewManager::renderingEnabledChanged, this, &VisualizationService::renderingEnabledChanged);
//	}
}

ViewPtr VisualizationImplService::get3DView(int group, int index)
{
	return viewManager()->get3DView(group, index);
}

int VisualizationImplService::getActiveViewGroup() const
{
	return viewManager()->getActiveViewGroup();
}
ViewGroupDataPtr VisualizationImplService::getViewGroupData(int groupIdx) const
{
	return viewManager()->getViewGroup(groupIdx);
//	std::vector<ViewGroupPtr> viewGroups = viewManager()->getViewGroups();
//	if (!viewGroups.empty())
//		return viewGroups[groupIdx]->getData();
//	else
//		return ViewGroupDataPtr();
}

bool VisualizationImplService::isNull()
{
	return false;
}

void VisualizationImplService::autoShowData(cx::DataPtr data)
{
	viewManager()->autoShowData(data);
}

void VisualizationImplService::enableRender(bool val)
{
	viewManager()->enableRender(val);
}

bool VisualizationImplService::renderingIsEnabled() const
{
	return viewManager()->renderingIsEnabled();
}

QWidget* VisualizationImplService::getLayoutWidget(int index)
{
	return viewManager()->getLayoutWidget(index);
}

QString VisualizationImplService::getActiveLayout(int widgetIndex) const
{
	return viewManager()->getActiveLayout(widgetIndex);
}

void VisualizationImplService::setActiveLayout(const QString& uid, int widgetIndex)
{
	viewManager()->setActiveLayout(uid, widgetIndex);
}

InteractiveClipperPtr VisualizationImplService::getClipper()
{
	return viewManager()->getClipper();
}

InteractiveCropperPtr VisualizationImplService::getCropper()
{
	return viewManager()->getCropper();
}

CyclicActionLoggerPtr VisualizationImplService::getRenderTimer()
{
	return viewManager()->getRenderTimer();
}

NavigationPtr VisualizationImplService::getNavigation()
{
	return viewManager()->getNavigation();
}

LayoutRepositoryPtr VisualizationImplService::getLayoutRepository()
{
	return viewManager()->getLayoutRepository();
}

CameraControlPtr VisualizationImplService::getCameraControl()
{
	return viewManager()->getCameraControl();
}

QActionGroup* VisualizationImplService::createInteractorStyleActionGroup()
{
	return viewManager()->createInteractorStyleActionGroup();
}


} /* namespace cx */
