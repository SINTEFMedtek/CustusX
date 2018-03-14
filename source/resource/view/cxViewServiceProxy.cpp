/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

NavigationPtr ViewServiceProxy::getNavigation(int group)
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

void ViewServiceProxy::zoomCamera3D(int viewGroup3DNumber, int zoomFactor)
{
	mViewService->zoomCamera3D(viewGroup3DNumber, zoomFactor);
}

void ViewServiceProxy::addDefaultLayout(LayoutData layoutData)
{
	mViewService->addDefaultLayout(layoutData);
}

void ViewServiceProxy::enableContextMenuForViews(bool enable)
{
	mViewService->enableContextMenuForViews(enable);
}

cx::ClippersPtr ViewServiceProxy::getClippers()
{
	return mViewService->getClippers();
}

} //cx
