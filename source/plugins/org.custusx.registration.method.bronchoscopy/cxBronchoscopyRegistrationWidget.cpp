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

#include "cxBronchoscopyRegistrationWidget.h"
#include <vtkPolyData.h>
#include "cxTransform3D.h"
#include "cxDataSelectWidget.h"
#include "cxTrackingService.h"
#include "cxMesh.h"
#include "cxAcquisitionData.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxTrackedCenterlineWidget.h"
#include "cxRecordSessionWidget.h"
#include "cxRecordSession.h"
#include "cxRepManager.h"
#include "cxView.h"
#include "cxToolRep3D.h"
#include "cxToolTracer.h"
#include "cxBronchoscopyRegistration.h"
#include "cxReporter.h"
#include "cxTypeConversions.h"
#include "cxThresholdPreview.h"
#include "cxPatientModelService.h"
#include "cxRegistrationService.h"
#include "cxVisualizationService.h"
#include "cxStringDataAdapterXml.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxTrackingService.h"

#include "cxLegacySingletons.h"


namespace cx
{
BronchoscopyRegistrationWidget::BronchoscopyRegistrationWidget(regServices services, QWidget* parent) :
	RegistrationBaseWidget(services, parent, "BronchoscopyRegistrationWidget",
						   "Bronchoscopy Registration"), mVerticalLayout(new QVBoxLayout(this))
{
	mSelectMeshWidget = SelectMeshStringDataAdapter::New(services.patientModelService);
	mSelectMeshWidget->setValueName("Centerline: ");

	AcquisitionDataPtr mAcquisitionData;
	mAcquisitionData.reset(new AcquisitionData());

	QDomElement sessionsNode = mServices.patientModelService->getCurrentWorkingElement("bronchoscopySessions");
	mAcquisitionData->parseXml(sessionsNode);

	this->initSessionSelector(mAcquisitionData);

	mRegisterButton = new QPushButton("Register");
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));
	mRegisterButton->setToolTip(this->defaultWhatsThis());


	mAcquisition.reset(new Acquisition(mAcquisitionData, this));

	connect(mAcquisition.get(), SIGNAL(started()), this, SLOT(acquisitionStarted()));
		connect(mAcquisition.get(), SIGNAL(acquisitionStopped()), this, SLOT(acquisitionStopped()), Qt::QueuedConnection);
	connect(mAcquisition.get(), SIGNAL(cancelled()), this, SLOT(acquisitionStopped()));

//    mTrackedCenterLine = new TrackedCenterlineWidget(mAcquisitionData, this);

	mRecordSessionWidget.reset(new RecordSessionWidget(mAcquisition, this, "Bronchoscope path"));

	mVerticalLayout->addWidget(new DataSelectWidget(services.visualizationService, services.patientModelService, this, mSelectMeshWidget));
//    mVerticalLayout->addWidget(mTrackedCenterLine);
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mSessionSelector));
    mVerticalLayout->addWidget(mRecordSessionWidget.get());
	mVerticalLayout->addWidget(mRegisterButton);

	mVerticalLayout->addStretch();

    boost::shared_ptr<WidgetObscuredListener> mObscuredListener;

    mObscuredListener.reset(new WidgetObscuredListener(this));
    connect(mObscuredListener.get(), SIGNAL(obscured(bool)), this, SLOT(obscuredSlot(bool)));
}

void BronchoscopyRegistrationWidget::initSessionSelector(AcquisitionDataPtr acquisitionData)
{
	std::vector<RecordSessionPtr> sessions = acquisitionData->getRecordSessions();
	QStringList sessionUids;

	std::vector<RecordSessionPtr>::iterator it = sessions.begin();
	QString defaultValue;
	if(it != sessions.end())
		defaultValue = (*it)->getUid();

	for(; it != sessions.end(); ++it)
	{
		QString uid = (*it)->getUid();
		sessionUids << uid;
		if(defaultValue < uid)
			defaultValue = uid;
	}

	mSessionSelector = StringDataAdapterXml::initialize("bronchoscopy_session", "Bronchoscope path", "Select bronchoscope path for registration", defaultValue, sessionUids, QDomNode());
}

QString BronchoscopyRegistrationWidget::defaultWhatsThis() const
{
	return QString();
}

void BronchoscopyRegistrationWidget::registerSlot()
{
	Transform3D old_rMpr = mServices.patientModelService->get_rMpr();//input to registrationAlgorithm
    //std::cout << "rMpr: " << std::endl;
    //std::cout << old_rMpr << std::endl;


    if(!mSelectMeshWidget->getMesh())
    {
        reportError("No centerline");
        return;
    }
	vtkPolyDataPtr centerline = mSelectMeshWidget->getMesh()->getVtkPolyData();//input
    Transform3D rMd = mSelectMeshWidget->getMesh()->get_rMd();

    if(!mTool)
    {
        reportError("No tool");
    }
    std::cout << "Tool name: " << mTool->getName() << std::endl;

	RecordSessionPtr session;
	QString sessionUid = mSessionSelector->getValue();
	if(!sessionUid.isEmpty())
		session = mAcquisition->getPluginData()->getRecordSession(sessionUid);
//	else
//		session = mAcquisition->getLatestSession();

    if(!session)
        reportError("No session");

    TimedTransformMap trackerRecordedData_prMt = RecordSession::getToolHistory_prMt(mTool, session);//input
//    TimedTransformMap trackerRecordedData = mTrackedCenterLine->getRecording();

    if(trackerRecordedData_prMt.size() == 0)
    {
        reportError("No positions");
        return;
    }

	BronchoscopyRegistration reg;
    Transform3D new_rMpr = Transform3D(reg.runBronchoscopyRegistration(centerline,trackerRecordedData_prMt,old_rMpr,rMd));

    new_rMpr = new_rMpr*old_rMpr;//output
	mServices.registrationService->applyPatientRegistration(new_rMpr, "Bronchoscopy centerline to tracking data");

    Eigen::Matrix4d display_rMpr = Eigen::Matrix4d::Identity();
            display_rMpr = new_rMpr*display_rMpr;
    std::cout << "New prMt: " << std::endl;
        for (int i = 0; i < 4; i++)
            std::cout << display_rMpr.row(i) << std::endl;

    ToolRep3DPtr activeRep3D = getToolRepIn3DView(mTool);
    activeRep3D->getTracer()->clear();

    QColor colorGreen = QColor(0, 255, 0, 255);
    activeRep3D->getTracer()->setColor(colorGreen);
    activeRep3D->getTracer()->addManyPositions(trackerRecordedData_prMt);


}

void BronchoscopyRegistrationWidget::acquisitionStarted()
{
    std::cout << "acquisitionStarted" << std::endl;

	mTool = mServices.trackingService->getActiveTool();
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

	mSessionSelector->setValue(mAcquisition->getLatestSession()->getUid());

	this->saveSessions();
}

void BronchoscopyRegistrationWidget::saveSessions()
{
//	RecordSessionPtr session = mAquisition->getLatestSession();
	QDomElement sessionsNode = mServices.patientModelService->getCurrentWorkingElement("bronchoscopySessions");
	mAcquisition->getPluginData()->addXml(sessionsNode);
}

ToolRep3DPtr BronchoscopyRegistrationWidget::getToolRepIn3DView(ToolPtr tool)
{
	ViewPtr view = mServices.visualizationService->get3DView();
    ToolRep3DPtr retval = RepManager::findFirstRep<ToolRep3D>(view->getReps(),tool);
	return retval;
}

void BronchoscopyRegistrationWidget::obscuredSlot(bool obscured)
{
    //std::cout << "Checking slot" << std::endl;
    if (!obscured)
        return;

    ToolRep3DPtr activeRep3D = this->getToolRepIn3DView(mTool);
    if (!activeRep3D)
        return;
    //std::cout << "Slot is cleared" << std::endl;
    activeRep3D->getTracer()->clear();
}

} //namespace cx
