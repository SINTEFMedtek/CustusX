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

#include "cxViewGroupPropertiesWidget.h"
#include <QLabel>
#include <QGroupBox>
#include <QToolBar>
#include "cxStringListSelectWidget.h"
#include "cxViewService.h"
#include "cxViewGroupData.h"
#include "cxStringProperty.h"
#include "cxBoolProperty.h"
#include "cxVisServices.h"
#include "cxHelperWidgets.h"
#include "cxLogger.h"
#include "cxCameraStyleInteractor.h"

namespace cx
{

ViewGroupPropertiesWidget::ViewGroupPropertiesWidget(int groupIndex,
													 VisServicesPtr services,
													 QWidget* parent) :
	BaseWidget(parent, "ViewGroupPropertiesWidget", "View Properties"),
	mGroupIndex(groupIndex),
	mServices(services)
{
	ViewGroupDataPtr group = this->getViewGroup();
	connect(group.get(), &ViewGroupData::optionsChanged, this, &ViewGroupPropertiesWidget::setModified);

	mLayout = new QVBoxLayout(this);
	this->setModified();
}

ViewGroupPropertiesWidget::~ViewGroupPropertiesWidget()
{
}

void ViewGroupPropertiesWidget::setupUI()
{
	if (mLayout->count()) // already created
		return;

	ViewGroupDataPtr group = this->getViewGroup();
	if (!group)
		return;

	mLayout->setMargin(2);

	StringListSelectWidget* slices3D = new StringListSelectWidget(this, group->getSliceDefinitionProperty());
	mLayout->addWidget(slices3D);

	this->createCameraStyleProperties();
	this->createCameraStyleWidget();

	mLayout->addStretch();
}

void ViewGroupPropertiesWidget::prePaintEvent()
{
	this->setupUI();
	this->updateFrontend();
}

void ViewGroupPropertiesWidget::updateFrontend()
{
	ViewGroupDataPtr group = this->getViewGroup();
	if (!group)
		return;

	ViewGroupData::Options options = group->getOptions();
	CameraStyleData data = options.mCameraStyle;

	mCameraFollowTool->setValue(data.mCameraFollowTool);
	mFocusFollowTool->setValue(data.mFocusFollowTool);
	mTableLock->setValue(data.mTableLock);
	mUniCam->setValue(data.mUniCam);
	mElevation->setValue(data.mElevation);
	mAutoZoomROI->setValue(data.mAutoZoomROI);
}

ViewGroupDataPtr ViewGroupPropertiesWidget::getViewGroup()
{
	return mServices->view()->getGroup(mGroupIndex);
}

void ViewGroupPropertiesWidget::createCameraStyleProperties()
{
	//	bool filterToolPositions = settings()->value("TrackingPositionFilter/enabled").value<bool>();
	mCameraFollowTool = BoolProperty::initialize("Camera Follow Tool", "",
												 "Camera position is fixed to the tool and moving along with it.",
												 true);
	mCameraStyleProperties.push_back(mCameraFollowTool);

	mFocusFollowTool = BoolProperty::initialize("Focus Follow Tool", "",
												"Scene focus is fixed to the tool and moving along with it.",
												true);
	mCameraStyleProperties.push_back(mFocusFollowTool);

	mTableLock = BoolProperty::initialize("Table lock", "",
										  "Table is always set down in the scene.",
										  true);
	mCameraStyleProperties.push_back(mTableLock);

	mElevation = DoubleProperty::initialize("Elevation", "",
											"View angle above tool, used if Follow tool is set.",
											0, DoubleRange(0, 80.0/180*M_PI, 5.0/180*M_PI), 0);
	mElevation->setInternal2Display(180.0/M_PI);
	mCameraStyleProperties.push_back(mElevation);

	mUniCam = BoolProperty::initialize("Unicam", "",
									   "Use the VTK Unicam interactor. Overrides other style settings",
									   true);
	mCameraStyleProperties.push_back(mUniCam);

	mAutoZoomROI = StringProperty::initialize("Auto Zoom ROI", "",
											  "Zoom so that the given ROI always is visible",
											  "",
											  QStringList()
											  );
	mCameraStyleProperties.push_back(mAutoZoomROI);

	for (unsigned i=0; i<mCameraStyleProperties.size(); ++i)
	{
		connect(mCameraStyleProperties[i].get(), &Property::changed,
				this, &ViewGroupPropertiesWidget::onCameraStyleChanged);
	}
}

void ViewGroupPropertiesWidget::createCameraStyleWidget()
{
	QGroupBox* groupBox = new QGroupBox("Camera Style");
	mLayout->addWidget(groupBox);

	QVBoxLayout* layout = new QVBoxLayout(groupBox);
	layout->setMargin(0);

	mCameraStyleInteractor.reset(new CameraStyleInteractor);
	mCameraStyleInteractor->connectCameraStyle(this->getViewGroup());

	QToolBar* toolBar = new QToolBar(this);
	layout->addWidget(toolBar);
	toolBar->addActions(mCameraStyleInteractor->getInteractorStyleActionGroup()->actions());


	QGridLayout *cameraStyleLayout = new QGridLayout;
	layout->addLayout(cameraStyleLayout);
	cameraStyleLayout->setMargin(0);
	groupBox->setLayout(cameraStyleLayout);
	int count = 0;
	for (unsigned i=0; i<mCameraStyleProperties.size(); ++i)
	{
		createDataWidget(mServices->view(), mServices->patient(), this, mCameraStyleProperties[i], cameraStyleLayout, count++);
	}
}

void ViewGroupPropertiesWidget::onCameraStyleChanged()
{
	ViewGroupDataPtr group = this->getViewGroup();
	ViewGroupData::Options options = group->getOptions();
	CameraStyleData data = options.mCameraStyle;

	data.mCameraFollowTool = mCameraFollowTool->getValue();
	data.mFocusFollowTool = mFocusFollowTool->getValue();
	data.mTableLock = mTableLock->getValue();
	data.mUniCam = mUniCam->getValue();
	data.mElevation = mElevation->getValue();
	data.mAutoZoomROI = mAutoZoomROI->getValue();

	options.mCameraStyle = data;
	group->setOptions(options);
}


} // cx
