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

#include "cxViewServiceNull.h"

#include <QActionGroup>
#include <QDomNode>

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
//	reportWarning("Trying to use VideoServiceNull. Is VideoService (org.custusx.core.view) disabled?");
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
NavigationPtr ViewServiceNull::getNavigation() { return NavigationPtr(); }
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

cx::ClippersPtr cx::ViewServiceNull::getClippers()
{
	return ClippersPtr();
}

} //cx
