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
#include "cxTrackedCenterlineWidget.h"
#include "cxRecordSessionWidget.h"
#include "cxRecordSession.h"
#include "cxRepManager.h"
#include "cxViewManager.h"
#include "sscView.h"
#include "sscToolRep3D.h"
#include "sscToolTracer.h"
#include "cxBronchoscopyRegistration.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"


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
		connect(mAquisition.get(), SIGNAL(acquisitionStopped()), this, SLOT(acquisitionStopped()), Qt::QueuedConnection);
    connect(mAquisition.get(), SIGNAL(cancelled()), this, SLOT(acquisitionStopped()));

//    mTrackedCenterLine = new TrackedCenterlineWidget(mAcquisitionData, this);

    mRecordSessionWidget.reset(new RecordSessionWidget(mAquisition, this, "Bronchoscope path"));

	mVerticalLayout->addWidget(new DataSelectWidget(this, mSelectMeshWidget));
//    mVerticalLayout->addWidget(mTrackedCenterLine);
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
	Transform3D old_rMpr = toolManager()->get_rMpr();//input?
    std::cout << "rMpr: " << std::endl;
    std::cout << old_rMpr << std::endl;

    if(!mSelectMeshWidget->getMesh())
    {
        messageManager()->sendError("No centerline");
        return;
    }
	vtkPolyDataPtr centerline = mSelectMeshWidget->getMesh()->getVtkPolyData();//input

    if(!mTool)
    {
        messageManager()->sendError("No tool");
    }
    std::cout << "Tool name: " << mTool->getName() << std::endl;
	RecordSessionPtr session = mAquisition->getLatestSession();
    if(!session)
        messageManager()->sendError("No session");

    TimedTransformMap trackerRecordedData_prMt = RecordSession::getToolHistory_prMt(mTool, session);//input
//    TimedTransformMap trackerRecordedData = mTrackedCenterLine->getRecording();

    if(trackerRecordedData_prMt.size() == 0)
    {
        messageManager()->sendError("No positions");
        return;
    }

	BronchoscopyRegistration reg;
    Transform3D new_rMpr = Transform3D(reg.runBronchoscopyRegistration(centerline,trackerRecordedData_prMt,old_rMpr));

    new_rMpr = new_rMpr*old_rMpr;//output
	mManager->applyPatientRegistration(new_rMpr, "Bronchoscopy centerline to tracking data");

    ToolRep3DPtr activeRep3D = getToolRepIn3DView(mTool);
	activeRep3D->getTracer()->clear();
}

void BronchoscopyRegistrationWidget::acquisitionStarted()
{
    std::cout << "acquisitionStarted" << std::endl;

    mTool = toolManager()->getDominantTool();
    ToolRep3DPtr activeRep3D = this->getToolRepIn3DView(mTool);
	if (!activeRep3D)
		return;

	activeRep3D->getTracer()->start();
}
void BronchoscopyRegistrationWidget::acquisitionStopped()
{
    std::cout << "acquisitionStopped" << std::endl;
    ToolRep3DPtr activeRep3D = this->getToolRepIn3DView(mTool);
	if (!activeRep3D)
		return;

	activeRep3D->getTracer()->stop();
}

ToolRep3DPtr BronchoscopyRegistrationWidget::getToolRepIn3DView(ToolPtr tool)
{
	ViewWidgetQPtr view = viewManager()->get3DView();
    ToolRep3DPtr retval = RepManager::findFirstRep<ToolRep3D>(view->getReps(),tool);
	return retval;
}

} //namespace cx
