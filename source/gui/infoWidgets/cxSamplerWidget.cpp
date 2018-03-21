/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxSamplerWidget.h"

#include "cxCoordinateSystemHelpers.h"
#include <vtkImageData.h>
#include "cxTrackingService.h"
#include "cxSpaceEditWidget.h"
#include "cxTypeConversions.h"
#include "cxSettings.h"
#include "cxImage.h"
#include "cxSpaceProvider.h"
#include "cxSpaceListener.h"
#include "cxPatientModelService.h"
#include "cxActiveData.h"
#include "cxVisServices.h"

namespace cx
{

SamplerWidget::SamplerWidget(TrackingServicePtr trackingService, PatientModelServicePtr patientModelService, SpaceProviderPtr spaceProvider, QWidget* parent) :
  BaseWidget(parent, "sampler_widget", "Point Sampler"),
  mPatientModelService(patientModelService),
  mSpaceProvider(spaceProvider)
{
	this->setToolTip("Display current tool tip position");
	mListener = spaceProvider->createListener();
	mListener->setSpace(Space::reference());
//	mListener.reset(new CoordinateSystemListener(Space(csREF)));
	connect(mListener.get(), SIGNAL(changed()), this, SLOT(setModified()));

	mActiveTool = ActiveToolProxy::New(trackingService);
	connect(mActiveTool.get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(setModified()));
	connect(mActiveTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), SLOT(setModified()));
	connect(spaceProvider.get(), &SpaceProvider::spaceAddedOrRemoved, this, &SamplerWidget::spacesChangedSlot);

	mLayout = new QHBoxLayout(this);
	mLayout->setMargin(4);
	mLayout->setSpacing(4);

	mAdvancedAction = this->createAction(this,
	                                     QIcon(":/icons/open_icon_library/system-run-5.png"),
	                                     "Details",
	                                     "Show Advanced options",
	                                     SLOT(toggleAdvancedSlot()),
	                                     mLayout,
	                                     new CXSmallToolButton());

	mAdvancedWidget = new QWidget(this);
	mAdvancedLayout = new QHBoxLayout(mAdvancedWidget);
	mAdvancedLayout->setMargin(0);
	mLayout->addWidget(mAdvancedWidget);

	mSpaceSelector = SpaceProperty::initialize("selectSpace",
											  "Space",
											  "Select coordinate system to store position in.");
	mSpaceSelector->setSpaceProvider(spaceProvider);

	connect(mSpaceSelector.get(), &SpaceProperty::valueWasSet, this, &SamplerWidget::spacesChangedSlot);
	connect(mSpaceSelector.get(), &SpaceProperty::valueWasSet, this, &SamplerWidget::setModified);
	Space space = Space::fromString(settings()->value("sampler/Space", Space(csREF).toString()).toString());
	mSpaceSelector->setValue(space);
	SpaceEditWidget* spaceSelectorWidget = new SpaceEditWidget(this, mSpaceSelector);
	spaceSelectorWidget->showLabel(false);
	mAdvancedLayout->addWidget(spaceSelectorWidget);
	this->spacesChangedSlot();

	mCoordLineEdit = new QLineEdit(this);
	mCoordLineEdit->setStyleSheet("QLineEdit { width : 30ex; }"); // enough for "-xxx.x, -xxx.x, -xxx.x - xxxx" plus some slack
	mCoordLineEdit->setSizePolicy(QSizePolicy::Fixed,
                                  mCoordLineEdit->sizePolicy().verticalPolicy());
	mCoordLineEdit->setReadOnly(true);
    mLayout->addWidget(mCoordLineEdit);

	this->showAdvanced();
	this->setModified();
}

SamplerWidget::~SamplerWidget()
{}

void SamplerWidget::toggleAdvancedSlot()
{
	settings()->setValue("sampler/ShowDetails", !settings()->value("sampler/ShowDetails", "true").toBool());
	mAdvancedWidget->setVisible(!mAdvancedWidget->isVisible());
	this->showAdvanced();
}

void SamplerWidget::showAdvanced()
{
	bool on = settings()->value("sampler/ShowDetails").toBool();
	mAdvancedWidget->setVisible(on);
}

void SamplerWidget::spacesChangedSlot()
{
	CoordinateSystem space = mSpaceSelector->getValue();
	settings()->setValue("sampler/Space", space.toString());

//	mSpaceSelector->setValueRange(spaceProvider()->getSpacesToPresentInGUI());
//	mSpaceSelector->setValue(space);
	mListener->setSpace(space);
}

void SamplerWidget::prePaintEvent()
{
	CoordinateSystem space = mSpaceSelector->getValue();
	Vector3D p = mSpaceProvider->getActiveToolTipPoint(space, true);
	int w=1;
	QString coord = QString("%1, %2, %3").arg(p[0], w, 'f', 1).arg(p[1], w, 'f', 1).arg(p[2], w, 'f', 1);

	ActiveDataPtr activeData = mPatientModelService->getActiveData();
	ImagePtr image = activeData->getActive<Image>();
	if (image)
	{
		Vector3D p = mSpaceProvider->getActiveToolTipPoint(Space(csDATA_VOXEL,"active"), true);
		IntBoundingBox3D bb(Eigen::Vector3i(0,0,0),
		                         Eigen::Vector3i(image->getBaseVtkImageData()->GetDimensions())-Eigen::Vector3i(1,1,1));
		if (bb.contains(p.cast<int>()))
		{
			double val = image->getBaseVtkImageData()->GetScalarComponentAsFloat(p[0], p[1], p[2], 0);
			int intVal = val;
			coord += QString(" I=%1").arg(intVal);
		}
	}

	mCoordLineEdit->setText(coord);
	mCoordLineEdit->setStatusTip(QString("Position of active tool tip in %1 space\n"
	                                     "and the intensity of the active volume in that position").arg(space.toString()));
	mCoordLineEdit->setToolTip(mCoordLineEdit->statusTip());
}


} // namespace cx

