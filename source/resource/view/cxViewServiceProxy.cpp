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

#include "cxViewServiceProxy.h"

#include <boost/bind.hpp>
#include <ctkPluginContext.h>
#include "cxNullDeleter.h"

#include <QDomNode>

namespace cx
{

VisualizationServicePtr VisualizationServiceProxy::create(ctkPluginContext *pluginContext)
{
	return VisualizationServicePtr(new VisualizationServiceProxy(pluginContext));
}

VisualizationServiceProxy::VisualizationServiceProxy(ctkPluginContext *pluginContext) :
	mPluginContext(pluginContext),
	mVisualizationService(VisualizationService::getNullObject())
{
	this->initServiceListener();
}

ViewPtr VisualizationServiceProxy::get3DView(int group, int index)
{
	return mVisualizationService->get3DView(group, index);
}

int VisualizationServiceProxy::getActiveGroupId() const
{
	return mVisualizationService->getActiveGroupId();
}

ViewGroupDataPtr VisualizationServiceProxy::getGroup(int groupIdx) const
{
	return mVisualizationService->getGroup(groupIdx);
}

void VisualizationServiceProxy::setRegistrationMode(REGISTRATION_STATUS mode)
{
	mVisualizationService->setRegistrationMode(mode);
}

void VisualizationServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<VisualizationService>(
								 mPluginContext,
								 boost::bind(&VisualizationServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (VisualizationService*)>(),
								 boost::bind(&VisualizationServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}
void VisualizationServiceProxy::onServiceAdded(VisualizationService* service)
{
	mVisualizationService.reset(service, null_deleter());

	connect(service, &VisualizationService::activeViewChanged, this, &VisualizationService::activeViewChanged);
	connect(service, &VisualizationService::fps, this, &VisualizationService::fps);
	connect(service, &VisualizationService::activeLayoutChanged, this, &VisualizationService::activeLayoutChanged);
	connect(service, &VisualizationService::renderingEnabledChanged, this, &VisualizationService::renderingEnabledChanged);
	connect(service, &VisualizationService::pointSampled, this, &VisualizationService::pointSampled);

	emit activeLayoutChanged();
	emit activeViewChanged();
	emit renderingEnabledChanged();
}

void VisualizationServiceProxy::onServiceRemoved(VisualizationService *service)
{
	disconnect(service, &VisualizationService::activeViewChanged, this, &VisualizationService::activeViewChanged);
	disconnect(service, &VisualizationService::fps, this, &VisualizationService::fps);
	disconnect(service, &VisualizationService::activeLayoutChanged, this, &VisualizationService::activeLayoutChanged);
	disconnect(service, &VisualizationService::renderingEnabledChanged, this, &VisualizationService::renderingEnabledChanged);
	disconnect(service, &VisualizationService::pointSampled, this, &VisualizationService::pointSampled);

	mVisualizationService = VisualizationService::getNullObject();

	emit activeLayoutChanged();
	emit activeViewChanged();
	emit renderingEnabledChanged();
}

bool VisualizationServiceProxy::isNull()
{
	return mVisualizationService->isNull();
}

void VisualizationServiceProxy::aboutToStop()
{
    mVisualizationService->aboutToStop();
}

void VisualizationServiceProxy::autoShowData(cx::DataPtr data)
{
	mVisualizationService->autoShowData(data);
}

void VisualizationServiceProxy::enableRender(bool val)
{
	mVisualizationService->enableRender(val);
}

bool VisualizationServiceProxy::renderingIsEnabled() const
{
	return mVisualizationService->renderingIsEnabled();
}

QWidget* VisualizationServiceProxy::getLayoutWidget(QWidget* parent, int index)
{
	return mVisualizationService->getLayoutWidget(parent, index);
}

QString VisualizationServiceProxy::getActiveLayout(int widgetIndex) const
{
	return mVisualizationService->getActiveLayout(widgetIndex);
}

void VisualizationServiceProxy::setActiveLayout(const QString& uid, int widgetIndex)
{
	mVisualizationService->setActiveLayout(uid, widgetIndex);
}

InteractiveClipperPtr VisualizationServiceProxy::getClipper()
{
	return mVisualizationService->getClipper();
}

InteractiveCropperPtr VisualizationServiceProxy::getCropper()
{
	return mVisualizationService->getCropper();
}

CyclicActionLoggerPtr VisualizationServiceProxy::getRenderTimer()
{
	return mVisualizationService->getRenderTimer();
}

NavigationPtr VisualizationServiceProxy::getNavigation()
{
	return mVisualizationService->getNavigation();
}

LayoutRepositoryPtr VisualizationServiceProxy::getLayoutRepository()
{
	return mVisualizationService->getLayoutRepository();
}

CameraControlPtr VisualizationServiceProxy::getCameraControl()
{
	return mVisualizationService->getCameraControl();
}

QActionGroup* VisualizationServiceProxy::createInteractorStyleActionGroup()
{
	return mVisualizationService->createInteractorStyleActionGroup();
}

} //cx
