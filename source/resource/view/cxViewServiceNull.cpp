/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewServiceNull.h"

#include <QActionGroup>
#include <QDomNode>
#include "cxLogger.h"

namespace cx
{
ViewServiceNull::ViewServiceNull()
{
	mActionGroup = new QActionGroup(this);
}

ViewPtr ViewServiceNull::get3DView(int group, int index)
{
	printWarning();
	return ViewPtr();
}

int ViewServiceNull::getActiveGroupId() const
{
	printWarning();
	return -1;
}

ViewGroupDataPtr ViewServiceNull::getGroup(int groupIdx) const
{
	printWarning();
	return ViewGroupDataPtr();
}

void ViewServiceNull::setRegistrationMode(REGISTRATION_STATUS mode)
{
	printWarning();
}
bool ViewServiceNull::isNull()
{
	return true;
}

void ViewServiceNull::printWarning() const
{
	CX_LOG_WARNING() <<"Trying to use VideoServiceNull. Is VideoService (org.custusx.core.view) disabled?";
}

void ViewServiceNull::autoShowData(cx::DataPtr data)
{
	printWarning();
}

void ViewServiceNull::enableRender(bool val)
{

}

bool ViewServiceNull::renderingIsEnabled() const
{
	return false;
}

QWidget* ViewServiceNull::createLayoutWidget(QWidget *parent, int index) { return NULL; }
QWidget* ViewServiceNull::getLayoutWidget(int index) { return NULL; }
QString ViewServiceNull::getActiveLayout(int widgetIndex) const { return ""; }
void ViewServiceNull::setActiveLayout(const QString& uid, int widgetIndex) {}
InteractiveCropperPtr ViewServiceNull::getCropper() { return InteractiveCropperPtr(); }
CyclicActionLoggerPtr ViewServiceNull::getRenderTimer() { return CyclicActionLoggerPtr(); }
NavigationPtr ViewServiceNull::getNavigation(int group) { return NavigationPtr(); }
LayoutRepositoryPtr ViewServiceNull::getLayoutRepository() { return LayoutRepositoryPtr(); }
CameraControlPtr ViewServiceNull::getCameraControl() { return CameraControlPtr(); }
QActionGroup* ViewServiceNull::getInteractorStyleActionGroup() { return mActionGroup; }

void ViewServiceNull::centerToImageCenterInActiveViewGroup()
{
	printWarning();
}

void ViewServiceNull::setCameraStyle(CAMERA_STYLE_TYPE style, int groupIdx)
{
	printWarning();
}

void ViewServiceNull::zoomCamera3D(int viewGroup3DNumber, int zoomFactor)
{
	printWarning();
}

void ViewServiceNull::addDefaultLayout(LayoutData layoutData)
{
	printWarning();
}

void ViewServiceNull::enableContextMenuForViews(bool enable)
{
	printWarning();
}

cx::ClippersPtr cx::ViewServiceNull::getClippers()
{
	return ClippersPtr();
}

} //cx
