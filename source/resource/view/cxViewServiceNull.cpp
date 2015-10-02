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
VisualizationServiceNull::VisualizationServiceNull()
{
	mActionGroup = new QActionGroup(this);
}

ViewPtr VisualizationServiceNull::get3DView(int group, int index)
{
	printWarning();
	return ViewPtr();
}

int VisualizationServiceNull::getActiveGroupId() const
{
	printWarning();
	return -1;
}

ViewGroupDataPtr VisualizationServiceNull::getGroup(int groupIdx) const
{
	printWarning();
	return ViewGroupDataPtr();
}

void VisualizationServiceNull::setRegistrationMode(REGISTRATION_STATUS mode)
{
	printWarning();
}
bool VisualizationServiceNull::isNull()
{
	return true;
}

void VisualizationServiceNull::printWarning() const
{
//	reportWarning("Trying to use VideoServiceNull. Is VideoService (org.custusx.core.view) disabled?");
}

void VisualizationServiceNull::autoShowData(cx::DataPtr data)
{
	printWarning();
}

void VisualizationServiceNull::enableRender(bool val)
{

}

bool VisualizationServiceNull::renderingIsEnabled() const
{
	return false;
}

QWidget* VisualizationServiceNull::getLayoutWidget(QWidget *parent, int index) { return NULL; }
QString VisualizationServiceNull::getActiveLayout(int widgetIndex) const { return ""; }
void VisualizationServiceNull::setActiveLayout(const QString& uid, int widgetIndex) {}
InteractiveClipperPtr VisualizationServiceNull::getClipper() { return InteractiveClipperPtr(); }
InteractiveCropperPtr VisualizationServiceNull::getCropper() { return InteractiveCropperPtr(); }
CyclicActionLoggerPtr VisualizationServiceNull::getRenderTimer() { return CyclicActionLoggerPtr(); }
NavigationPtr VisualizationServiceNull::getNavigation() { return NavigationPtr(); }
LayoutRepositoryPtr VisualizationServiceNull::getLayoutRepository() { return LayoutRepositoryPtr(); }
CameraControlPtr VisualizationServiceNull::getCameraControl() { return CameraControlPtr(); }
QActionGroup* VisualizationServiceNull::createInteractorStyleActionGroup() { return mActionGroup; }

} //cx
