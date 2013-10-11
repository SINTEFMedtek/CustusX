// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxBronchoscopyRegistrationWidget.h"
#include <vtkPolyData.h>
#include "sscTransform3D.h"
#include "cxDataSelectWidget.h"
#include "sscToolManager.h"
#include "sscMesh.h"
#include "cxRegistrationManager.h"
#include "cxAcquisitionData.h"
#include "cxSelectDataStringDataAdapter.h"
//#include "cxTrackedCenterlineWidget.h"
#include "cxRecordSessionWidget.h"
#include "cxRecordSession.h"
#include "cxRepManager.h"
#include "cxViewManager.h"
#include "sscView.h"
#include "sscToolRep3D.h"
#include "sscToolTracer.h"
#include "cxBronchoscopyRegistration.h"


namespace cx
{
BronchoscopyRegistrationWidget::BronchoscopyRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent) :
	RegistrationBaseWidget(regManager, parent, "BronchoscopyRegistrationWidget",
						   "Bronchoscopy Registration"), mVerticalLayout(new QVBoxLayout(this))
{
	mSelectMeshWidget = SelectMeshStringDataAdapter::New();
	mSelectMeshWidget->setValueName("Centerline: ");

	mRegisterButton = new QPushButton("Register");
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));
	mRegisterButton->setToolTip(this->defaultWhatsThis());


	AcquisitionDataPtr mAcquisitionData;
	mAcquisitionData.reset(new AcquisitionData());

	mAquisition.reset(new Acquisition(mAcquisitionData, this));

	connect(mAquisition.get(), SIGNAL(started()), this, SLOT(acquisitionStarted()));
	connect(mAquisition.get(), SIGNAL(stopped()), this, SLOT(acquisitionStopped()));
	connect(mAquisition.get(), SIGNAL(cancelled()), this, SLOT(acquisitionStopped()));

//	TrackedCenterlineWidget* trackedCenterLine = new TrackedCenterlineWidget(mAcquisitionData, this);

	mRecordSessionWidget.reset(new RecordSessionWidget(mAquisition, this, "Bronchoscope path"));

	mVerticalLayout->addWidget(new DataSelectWidget(this, mSelectMeshWidget));
//	mVerticalLayout->addWidget(trackedCenterLine);
	mVerticalLayout->addWidget(mRecordSessionWidget.get());
	mVerticalLayout->addWidget(mRegisterButton);

	mVerticalLayout->addStretch();
}


QString BronchoscopyRegistrationWidget::defaultWhatsThis() const
{
	return QString();
}

void BronchoscopyRegistrationWidget::registerSlot()
{
	Transform3D old_rMpr = *toolManager()->get_rMpr();//input?

	vtkPolyDataPtr centerline = mSelectMeshWidget->getMesh()->getVtkPolyData();//input


	ToolPtr tool = toolManager()->getDominantTool();
	RecordSessionPtr session = mAquisition->getLatestSession();
	TimedTransformMap trackerRecordedData = RecordSession::getToolHistory(tool, session);//input

	BronchoscopyRegistration reg;
	Transform3D new_rMpr = Transform3D(reg.runBronchoscopyRegistration(centerline,trackerRecordedData));



//	Transform3D new_rMpr = old_rMpr;//output
	mManager->applyPatientRegistration(new_rMpr, "Bronchoscopy centerline to tracking data");



	ToolRep3DPtr activeRep3D = getActiveToolRepIn3DView();
	activeRep3D->getTracer()->clear();
}

void BronchoscopyRegistrationWidget::acquisitionStarted()
{
	ToolRep3DPtr activeRep3D = getActiveToolRepIn3DView();
	if (!activeRep3D)
		return;

	activeRep3D->getTracer()->start();
}
void BronchoscopyRegistrationWidget::acquisitionStopped()
{
	ToolRep3DPtr activeRep3D = getActiveToolRepIn3DView();
	if (!activeRep3D)
		return;

	activeRep3D->getTracer()->stop();
}

ToolRep3DPtr BronchoscopyRegistrationWidget::getActiveToolRepIn3DView()
{
	ViewWidgetQPtr view = viewManager()->get3DView();
	ToolRep3DPtr retval = RepManager::findFirstRep<ToolRep3D>(view->getReps(),
																   toolManager()->getDominantTool());
	return retval;
}

} //namespace cx
