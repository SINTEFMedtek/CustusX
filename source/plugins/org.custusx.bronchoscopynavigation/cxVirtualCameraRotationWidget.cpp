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

#include <cmath>
#include "cxVirtualCameraRotationWidget.h"
#include "cxStringPropertySelectTool.h"
#include "cxHelperWidgets.h"
#include "cxTrackingService.h"
#include "cxToolProperty.h"
#include "cxLogger.h"

namespace cx
{

VirtualCameraRotationWidget::VirtualCameraRotationWidget(VisServicesPtr services, StringPropertySelectToolPtr toolSelector, QWidget* parent) :
	BaseWidget(parent,"virtual_camera_rotation_widget", "Virtual Camera Rotation"),
	mVerticalLayout(new QVBoxLayout(this)),
	mToolSelector(toolSelector)
{
	QLabel *title = new QLabel(tr("Virtual Camera Rotation"));
	title->setStyleSheet("font-weight: bold");
	mVerticalLayout->addWidget(title,0,Qt::AlignLeft);
	
	//mVerticalLayout->addWidget(sscCreateDataWidget(this, mToolSelector));
	mVerticalLayout->addWidget(new QLabel("<font color=red>Caution: sMt is changed directly by this control.</font>"));
	
	QLabel *labelRot = new QLabel(tr("Rotate (360 deg)"));
	mRotateDial = new QDial;
	mRotateDial->setMinimum(-180);
	mRotateDial->setMaximum(180);
	mRotateDial->setNotchesVisible(true);
	
	mVerticalLayout->addWidget(labelRot,0,Qt::AlignLeft);
	mVerticalLayout->addWidget(mRotateDial,0,Qt::AlignRight);
	
	this->setLayout(mVerticalLayout);
	
	connect(mRotateDial, &QDial::valueChanged, this, &VirtualCameraRotationWidget::toolRotationChanged);
	connect(mToolSelector.get(), SIGNAL(changed()), this, SLOT(toolCalibrationChanged()));
	connect(services->tracking().get(), &TrackingService::stateChanged, this, &VirtualCameraRotationWidget::toolCalibrationChanged);
}

VirtualCameraRotationWidget::~VirtualCameraRotationWidget()
{
}

QString VirtualCameraRotationWidget::getWidgetName()
{
	return "virtual_camera_rotation_widget"; 
}

void VirtualCameraRotationWidget::toolCalibrationChanged()
{
	ToolPtr tool = this->getTool();
	if (!tool)
		return;

	mRotateDial->blockSignals(true);

	mDecomposition.reset(tool->getCalibration_sMt());
	Vector3D angles = mDecomposition.getAngles();
	int zAngle = (int) std::round(angles(2)*180/M_PI);
	mRotateDial->setValue(zAngle);
	
	mRotateDial->blockSignals(false);
}

void VirtualCameraRotationWidget::toolRotationChanged()
{
	ToolPtr tool = this->getTool();
	if (!tool)
		return;

	Transform3D M = tool->getCalibration_sMt();
	double zAngleUpdated = mRotateDial->value()*M_PI/180;
	Vector3D angles = mDecomposition.getAngles();
	angles(2) = zAngleUpdated;
	mDecomposition.setAngles(angles);
	M = mDecomposition.getMatrix();	
	tool->setCalibration_sMt(M);
}

ToolPtr VirtualCameraRotationWidget::getTool()
{
	ToolPtr tool = mToolSelector->getTool();
	if (tool)
	{	
		ToolPtr baseTool = tool->getBaseTool();
		if (baseTool)
			tool = baseTool;
	}
	return tool;
}

QString VirtualCameraRotationWidget::defaultWhatsThis() const
{
	return	"<html>"
					"<h3>VirtualCameraRotationWidget.</h3>"
					"<p>Rotates virtual camera (calibration).</p>"
					"</html>";
}


} /* namespace cx */
