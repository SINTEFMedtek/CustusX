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

#include "cxViewImplService.h"

#include <ctkPluginContext.h>
#include "cxViewManager.h"
#include "cxViewGroup.h"
#include "cxRepManager.h"
#include "cxVisServices.h"
#include "cxSessionStorageServiceProxy.h"
#include "cxXMLNodeWrapper.h"
#include "cxLogger.h"

namespace cx
{

VisualizationImplService::VisualizationImplService(ctkPluginContext *context) :
	mContext(context )
{
	VisServicesPtr services = VisServices::create(context);
	mSession = SessionStorageServiceProxy::create(mContext);
	mBase = ViewManager::create(services);

	if(!viewManager())
		std::cout << "VisualizationImplService got no viewManager" << std::endl;
//	connect(viewManager(), SIGNAL(activeViewChanged()), this, SIGNAL(activeViewChanged()));
//	connect(viewManager(), &ViewManager::renderingEnabledChanged, this, &VisualizationService::renderingEnabledChanged);

	connect(mSession.get(), &SessionStorageService::sessionChanged, this, &VisualizationImplService::onSessionChanged);
	connect(mSession.get(), &SessionStorageService::cleared, this, &VisualizationImplService::onSessionCleared);
	connect(mSession.get(), &SessionStorageService::isLoading, this, &VisualizationImplService::onSessionLoad);
	connect(mSession.get(), &SessionStorageService::isSaving, this, &VisualizationImplService::onSessionSave);

	connect(viewManager(), &ViewManager::activeViewChanged, this, &VisualizationService::activeViewChanged);
	connect(viewManager(), &ViewManager::fps, this, &VisualizationService::fps);
	connect(viewManager(), &ViewManager::activeLayoutChanged, this, &VisualizationService::activeLayoutChanged);
	connect(viewManager(), &ViewManager::renderingEnabledChanged, this, &VisualizationService::renderingEnabledChanged);
	connect(viewManager(), &ViewManager::pointSampled, this, &VisualizationService::pointSampled);
}

VisualizationImplService::~VisualizationImplService()
{
}

ViewPtr VisualizationImplService::get3DView(int group, int index)
{
	return viewManager()->get3DView(group, index);
}

int VisualizationImplService::getActiveGroupId() const
{
	return viewManager()->getActiveViewGroup();
}
ViewGroupDataPtr VisualizationImplService::getGroup(int groupIdx) const
{
	return viewManager()->getViewGroup(groupIdx);
}

bool VisualizationImplService::isNull()
{
	return false;
}

void VisualizationImplService::aboutToStop()
{
    CX_LOG_DEBUG() << "About to destruct the visualizationservice and viewmanager, making sure timers are stopped.";
    viewManager()->enableRender(false);
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

QWidget* VisualizationImplService::getLayoutWidget(QWidget* parent, int index)
{
	return viewManager()->getLayoutWidget(parent, index);
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

void VisualizationImplService::onSessionChanged()
{
}
void VisualizationImplService::onSessionCleared()
{
	viewManager()->clear();
}
void VisualizationImplService::onSessionLoad(QDomElement& node)
{
	XMLNodeParser root(node);
	QDomElement viewManagerNode = root.descend("managers/viewManager").node().toElement();
	if (!viewManagerNode.isNull())
		viewManager()->parseXml(viewManagerNode);
}
void VisualizationImplService::onSessionSave(QDomElement& node)
{
	XMLNodeAdder root(node);
	QDomElement managerNode = root.descend("managers").node().toElement();
	viewManager()->addXml(managerNode);
}


} /* namespace cx */
