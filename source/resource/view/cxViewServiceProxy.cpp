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

ViewServicePtr ViewServiceProxy::create(ctkPluginContext *pluginContext)
{
	return ViewServicePtr(new ViewServiceProxy(pluginContext));
}

ViewServiceProxy::ViewServiceProxy(ctkPluginContext *pluginContext) :
	mPluginContext(pluginContext),
	mViewService(ViewService::getNullObject())
{
	this->initServiceListener();
}

ViewPtr ViewServiceProxy::get3DView(int group, int index)
{
	return mViewService->get3DView(group, index);
}

int ViewServiceProxy::getActiveGroupId() const
{
	return mViewService->getActiveGroupId();
}

ViewGroupDataPtr ViewServiceProxy::getGroup(int groupIdx) const
{
	return mViewService->getGroup(groupIdx);
}

void ViewServiceProxy::setRegistrationMode(REGISTRATION_STATUS mode)
{
	mViewService->setRegistrationMode(mode);
}

void ViewServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<ViewService>(
								 mPluginContext,
								 boost::bind(&ViewServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (ViewService*)>(),
								 boost::bind(&ViewServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}
void ViewServiceProxy::onServiceAdded(ViewService* service)
{
	mViewService.reset(service, null_deleter());

	connect(service, &ViewService::activeViewChanged, this, &ViewService::activeViewChanged);
	connect(service, &ViewService::fps, this, &ViewService::fps);
	connect(service, &ViewService::activeLayoutChanged, this, &ViewService::activeLayoutChanged);
	connect(service, &ViewService::renderingEnabledChanged, this, &ViewService::renderingEnabledChanged);
	connect(service, &ViewService::pointSampled, this, &ViewService::pointSampled);
	connect(service, &ViewService::renderFinished, this, &ViewService::renderFinished);

	emit activeLayoutChanged();
	emit activeViewChanged();
	emit renderingEnabledChanged();
}

void ViewServiceProxy::onServiceRemoved(ViewService *service)
{
	disconnect(service, &ViewService::activeViewChanged, this, &ViewService::activeViewChanged);
	disconnect(service, &ViewService::fps, this, &ViewService::fps);
	disconnect(service, &ViewService::activeLayoutChanged, this, &ViewService::activeLayoutChanged);
	disconnect(service, &ViewService::renderingEnabledChanged, this, &ViewService::renderingEnabledChanged);
	disconnect(service, &ViewService::pointSampled, this, &ViewService::pointSampled);
	disconnect(service, &ViewService::renderFinished, this, &ViewService::renderFinished);

	mViewService = ViewService::getNullObject();

	emit activeLayoutChanged();
	emit activeViewChanged();
	emit renderingEnabledChanged();
}

bool ViewServiceProxy::isNull()
{
	return mViewService->isNull();
}

void ViewServiceProxy::aboutToStop()
{
    mViewService->aboutToStop();
}

void ViewServiceProxy::autoShowData(cx::DataPtr data)
{
	mViewService->autoShowData(data);
}

void ViewServiceProxy::enableRender(bool val)
{
	mViewService->enableRender(val);
}

bool ViewServiceProxy::renderingIsEnabled() const
{
	return mViewService->renderingIsEnabled();
}

QWidget* ViewServiceProxy::createLayoutWidget(QWidget* parent, int index)
{
    return mViewService->createLayoutWidget(parent, index);
}

QWidget* ViewServiceProxy::getLayoutWidget(int index)
{
    return mViewService->getLayoutWidget(index);
}

QString ViewServiceProxy::getActiveLayout(int widgetIndex) const
{
	return mViewService->getActiveLayout(widgetIndex);
}

void ViewServiceProxy::setActiveLayout(const QString& uid, int widgetIndex)
{
	mViewService->setActiveLayout(uid, widgetIndex);
}

InteractiveCropperPtr ViewServiceProxy::getCropper()
{
	return mViewService->getCropper();
}

CyclicActionLoggerPtr ViewServiceProxy::getRenderTimer()
{
	return mViewService->getRenderTimer();
}

NavigationPtr ViewServiceProxy::getNavigation()
{
	return mViewService->getNavigation();
}

LayoutRepositoryPtr ViewServiceProxy::getLayoutRepository()
{
	return mViewService->getLayoutRepository();
}

CameraControlPtr ViewServiceProxy::getCameraControl()
{
	return mViewService->getCameraControl();
}

QActionGroup* ViewServiceProxy::getInteractorStyleActionGroup()
{
	return mViewService->getInteractorStyleActionGroup();
}

void ViewServiceProxy::centerToImageCenterInActiveViewGroup()
{
	return mViewService->centerToImageCenterInActiveViewGroup();
}

void ViewServiceProxy::setCameraStyle(CAMERA_STYLE_TYPE style, int groupIdx)
{
	mViewService->setCameraStyle(style, groupIdx);
}

void ViewServiceProxy::addDefaultLayout(LayoutData layoutData)
{
	mViewService->addDefaultLayout(layoutData);
}

void ViewServiceProxy::enableContextMenuForViews(bool enable)
{
	mViewService->enableContextMenuForViews(enable);
}

SharedOpenGLContextPtr ViewServiceProxy::getSharedOpenGLContext()
{
	return mViewService->getSharedOpenGLContext();
}

void ViewServiceProxy::setSharedOpenGLContext(SharedOpenGLContextPtr sharedOpenGLContext)
{
	return mViewService->setSharedOpenGLContext(sharedOpenGLContext);
}

cx::ClippersPtr ViewServiceProxy::getClippers()
{
	return mViewService->getClippers();
}

/*
vtkRenderWindowPtr ViewServiceProxy::getRenderWindow(QString uid, bool offScreenRendering)
{
	return mViewService->getRenderWindow(uid, offScreenRendering);
}

vtkRenderWindowPtr ViewServiceProxy::getSharedRenderWindow() const
{
	return mViewService->getSharedRenderWindow();
}
*/

} //cx
