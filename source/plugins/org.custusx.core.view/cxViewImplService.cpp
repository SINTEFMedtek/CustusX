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
#include "cxViewGroupData.h"
#include "cxClippers.h"

namespace cx
{

ViewImplService::ViewImplService(ctkPluginContext *context) :
	mContext(context )
{
	VisServicesPtr services = VisServices::create(context);
	mSession = SessionStorageServiceProxy::create(mContext);
	mBase = ViewManager::create(services);
	mClippers = ClippersPtr(new Clippers(services));

	if(!viewManager())
		std::cout << "ViewImplService got no viewManager" << std::endl;
//	connect(viewManager(), SIGNAL(activeViewChanged()), this, SIGNAL(activeViewChanged()));
//	connect(viewManager(), &ViewManager::renderingEnabledChanged, this, &ViewService::renderingEnabledChanged);

	connect(mSession.get(), &SessionStorageService::sessionChanged, this, &ViewImplService::onSessionChanged);
	connect(mSession.get(), &SessionStorageService::cleared, this, &ViewImplService::onSessionCleared);
	connect(mSession.get(), &SessionStorageService::isLoading, this, &ViewImplService::onSessionLoad);
	connect(mSession.get(), &SessionStorageService::isSaving, this, &ViewImplService::onSessionSave);

	connect(viewManager(), &ViewManager::activeViewChanged, this, &ViewService::activeViewChanged);
	connect(viewManager(), &ViewManager::fps, this, &ViewService::fps);
	connect(viewManager(), &ViewManager::activeLayoutChanged, this, &ViewService::activeLayoutChanged);
	connect(viewManager(), &ViewManager::renderingEnabledChanged, this, &ViewService::renderingEnabledChanged);
	connect(viewManager(), &ViewManager::pointSampled, this, &ViewService::pointSampled);
}

ViewImplService::~ViewImplService()
{
}

ViewPtr ViewImplService::get3DView(int group, int index)
{
	return viewManager()->get3DView(group, index);
}

int ViewImplService::getActiveGroupId() const
{
	return viewManager()->getActiveViewGroup();
}
ViewGroupDataPtr ViewImplService::getGroup(int groupIdx) const
{
	return viewManager()->getViewGroup(groupIdx);
}

void ViewImplService::setRegistrationMode(REGISTRATION_STATUS mode)
{
	this->getGroup(0)->setRegistrationMode(mode);
}

bool ViewImplService::isNull()
{
	return false;
}

void ViewImplService::aboutToStop()
{
    CX_LOG_DEBUG() << "About to destruct the visualizationservice and viewmanager, making sure timers are stopped.";
    viewManager()->enableRender(false);
}

void ViewImplService::autoShowData(cx::DataPtr data)
{
	viewManager()->autoShowData(data);
}

void ViewImplService::enableRender(bool val)
{
	viewManager()->enableRender(val);
}

bool ViewImplService::renderingIsEnabled() const
{
	return viewManager()->renderingIsEnabled();
}

QWidget* ViewImplService::createLayoutWidget(QWidget* parent, int index)
{
    return viewManager()->createLayoutWidget(parent, index);
}

QWidget* ViewImplService::getLayoutWidget(int index)
{
    return viewManager()->getLayoutWidget(index);
}

QString ViewImplService::getActiveLayout(int widgetIndex) const
{
	return viewManager()->getActiveLayout(widgetIndex);
}

void ViewImplService::setActiveLayout(const QString& uid, int widgetIndex)
{
	viewManager()->setActiveLayout(uid, widgetIndex);
}

InteractiveClipperPtr ViewImplService::getClipper()
{
	return viewManager()->getClipper();
}

ClippersPtr ViewImplService::getClippers()
{
	return this->mClippers;
}

InteractiveCropperPtr ViewImplService::getCropper()
{
	return viewManager()->getCropper();
}

CyclicActionLoggerPtr ViewImplService::getRenderTimer()
{
	return viewManager()->getRenderTimer();
}

NavigationPtr ViewImplService::getNavigation()
{
	return viewManager()->getNavigation();
}

LayoutRepositoryPtr ViewImplService::getLayoutRepository()
{
	return viewManager()->getLayoutRepository();
}

CameraControlPtr ViewImplService::getCameraControl()
{
	return viewManager()->getCameraControl();
}

QActionGroup* ViewImplService::createInteractorStyleActionGroup()
{
	return viewManager()->createInteractorStyleActionGroup();
}

void ViewImplService::onSessionChanged()
{
}
void ViewImplService::onSessionCleared()
{
	viewManager()->clear();
}
void ViewImplService::onSessionLoad(QDomElement& node)
{
	XMLNodeParser root(node);
	QDomElement viewManagerNode = root.descend("managers/viewManager").node().toElement();
	if (!viewManagerNode.isNull())
		viewManager()->parseXml(viewManagerNode);
}
void ViewImplService::onSessionSave(QDomElement& node)
{
	XMLNodeAdder root(node);
	QDomElement managerNode = root.descend("managers").node().toElement();
	viewManager()->addXml(managerNode);
}


} /* namespace cx */
