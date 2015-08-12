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
#include "cxSelectDataStringProperty.h"
#include "cxTrackedCenterlineWidget.h"
#include "cxRecordSessionWidget.h"
#include "cxRecordSession.h"
#include "cxView.h"
#include "cxToolRep3D.h"
#include "cxToolTracer.h"
#include "cxBronchoscopyRegistration.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxStringProperty.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxTrackingService.h"
#include "cxDoubleProperty.h"
#include "cxProfile.h"
#include "cxHelperWidgets.h"
#include "cxBoolProperty.h"
#include "cxCheckBoxWidget.h"
#include "cxRepContainer.h"
#include "cxWidgetObscuredListener.h"
#include "cxViewGroupData.h"
#include "cxStringPropertySelectTool.h"
#include "cxHelperWidgets.h"
#include "cxAcquisitionService.h"
#include "cxRegServices.h"


namespace cx
{
BronchoscopyRegistrationWidget::BronchoscopyRegistrationWidget(RegServices services, QWidget* parent) :
	RegistrationBaseWidget(services, parent, "org_custusx_registration_method_bronchoscopy_widget",
						   "Bronchoscopy Registration"),
	mBronchoscopyRegistration(new BronchoscopyRegistration()),
	mServices(services)
{
	mVerticalLayout = new QVBoxLayout(this);
	mOptions = profile()->getXmlSettings().descend("bronchoscopyregistrationwidget");

	mSelectMeshWidget = StringPropertySelectMesh::New(mServices.patientModelService);
	mSelectMeshWidget->setValueName("Centerline: ");

	mSelectToolWidget = StringPropertySelectTool::New(services.getToolManager());
	//this->initializeTrackingService();

	mProcessCenterlineButton = new QPushButton("Process centerline");
	connect(mProcessCenterlineButton, SIGNAL(clicked()), this, SLOT(processCenterlineSlot()));
	mProcessCenterlineButton->setToolTip(this->defaultWhatsThis());

	mRegisterButton = new QPushButton("Register");
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));
	mRegisterButton->setToolTip(this->defaultWhatsThis());

	this->initSessionSelector();

//    mTrackedCenterLine = new TrackedCenterlineWidget(mAcquisitionData, this);

	mRecordSessionWidget.reset(new RecordSessionWidget(services.acquisitionService, this, "Bronchoscope path", false));

	mVerticalLayout->setMargin(0);
	mVerticalLayout->addWidget(new DataSelectWidget(mServices.visualizationService, mServices.patientModelService, this, mSelectMeshWidget));
//    mVerticalLayout->addWidget(mTrackedCenterLine);

	this->selectSubsetOfBranches(mOptions.getElement());
	this->createMaxNumberOfGenerations(mOptions.getElement());
	this->useLocalRegistration(mOptions.getElement());

//	QHBoxLayout* activeToolLayout = new QHBoxLayout;
//	activeToolLayout->addWidget(new QLabel("Name:", this));
//	mToolNameLabel = new QLabel(this);
//	activeToolLayout->addWidget(mToolNameLabel);
//	mActiveToolVisibleLabel = new QLabel("Visible: NA");
//	activeToolLayout->addWidget(mActiveToolVisibleLabel);
//	activeGroupLayout->addLayout(activeToolLayout);

	mVerticalLayout->addWidget(new CheckBoxWidget(this, mUseSubsetOfGenerations));
	mVerticalLayout->addWidget(createDataWidget(mServices.visualizationService, mServices.patientModelService, this, mMaxNumberOfGenerations));
	mVerticalLayout->addWidget(mProcessCenterlineButton);
	mVerticalLayout->addWidget(sscCreateDataWidget(this, mSelectToolWidget));
	mVerticalLayout->addWidget(mRecordSessionWidget.get());
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mSessionSelector));
	mVerticalLayout->addWidget(new CheckBoxWidget(this, mUseLocalRegistration));
	mVerticalLayout->addWidget(mRegisterButton);

	mVerticalLayout->addStretch();

	mObscuredListener.reset(new WidgetObscuredListener(this));
	connect(mObscuredListener.get(), SIGNAL(obscured(bool)), this, SLOT(obscuredSlot(bool)));

}

void BronchoscopyRegistrationWidget::initSessionSelector()
{
	QStringList sessionUids = getSessionList();
	QString defaultValue;
	if(!sessionUids.isEmpty())
		defaultValue = sessionUids.last();
	mSessionSelector = StringProperty::initialize("bronchoscopy_session", "Bronchoscope path", "Select bronchoscope path for registration", defaultValue, sessionUids, QDomNode());

	//TODO: Let mSessionSelector display displaynames instead of uids (StringDataAdapterXml::setDisplayNames)
}

QStringList BronchoscopyRegistrationWidget::getSessionList()
{
	std::vector<RecordSessionPtr> sessions = mServices.acquisitionService->getSessions();
	std::vector<RecordSessionPtr>::iterator it = sessions.begin();
	QStringList sessionUids;

	for(; it != sessions.end(); ++it)
	{
		QString uid = (*it)->getUid();
		sessionUids << uid;
	}

	sessionUids.sort();
	return sessionUids;
}

QString BronchoscopyRegistrationWidget::defaultWhatsThis() const
{
	return QString();
}

void BronchoscopyRegistrationWidget::initializeTrackingService()
{
	if(mServices.trackingService->getState() < Tool::tsCONFIGURED)
		mServices.trackingService->setState(Tool::tsCONFIGURED);
}

void BronchoscopyRegistrationWidget::processCenterlineSlot()
{
	this->initializeTrackingService();

	if(!mSelectMeshWidget->getMesh())
	{
		reportError("No centerline");
		return;
	}
	vtkPolyDataPtr centerline = mSelectMeshWidget->getMesh()->getVtkPolyData();//input
	Transform3D rMd = mSelectMeshWidget->getMesh()->get_rMd();
	vtkPolyDataPtr processedCenterline;
	if (mUseSubsetOfGenerations->getValue())
		processedCenterline = mBronchoscopyRegistration->processCenterline(centerline, rMd, mMaxNumberOfGenerations->getValue());
	else
		processedCenterline = mBronchoscopyRegistration->processCenterline(centerline, rMd);

	if (!mMesh)
	{
		QString uid = mSelectMeshWidget->getMesh()->getUid() + "_cl%1";
		QString name = mSelectMeshWidget->getMesh()->getName()+" cl_processed%1";
		mMesh = mServices.patientModelService->createSpecificData<Mesh>(uid, name);
	}
	mMesh->setVtkPolyData(processedCenterline);
	mMesh->setColor(QColor(0, 0, 255, 255));
	mServices.patientModelService->insertData(mMesh);
	mServices.visualizationService->autoShowData(mMesh);
}

void BronchoscopyRegistrationWidget::registerSlot()
{
	if(!mBronchoscopyRegistration->isCenterlineProcessed())
	{
		reportError("Centerline not processed");
		return;
	}

	Transform3D old_rMpr = mServices.patientModelService->get_rMpr();//input to registrationAlgorithm
    //std::cout << "rMpr: " << std::endl;
    //std::cout << old_rMpr << std::endl;

	mTool = mSelectToolWidget->getTool();
    if(!mTool)
        mTool = mServices.trackingService->getActiveTool();

    if(!mTool)
    {
        reportError("No tool");
		return;
    }
    std::cout << "Tool name: " << mTool->getName() << std::endl;

	RecordSessionPtr session;
	QString sessionUid = mSessionSelector->getValue();
	if(!sessionUid.isEmpty())
		session = mServices.acquisitionService->getSession(sessionUid);
//	else
//		session = mAcquisition->getLatestSession();

    if(!session)
        reportError("No session");

    TimedTransformMap trackerRecordedData_prMt = RecordSession::getToolHistory_prMt(mTool, session);//input
//    TimedTransformMap trackerRecordedData = mTrackedCenterLine->getRecording();

	if(trackerRecordedData_prMt.empty())
	{
        reportError("No positions");
        return;
    }

	Transform3D new_rMpr;
	double maxDistanceForLocalRegistration = 30; //mm
	if(mUseLocalRegistration->getValue())
		new_rMpr = Transform3D(mBronchoscopyRegistration->runBronchoscopyRegistration(trackerRecordedData_prMt,old_rMpr,maxDistanceForLocalRegistration));
	else
		new_rMpr = Transform3D(mBronchoscopyRegistration->runBronchoscopyRegistration(trackerRecordedData_prMt,old_rMpr,0));

    new_rMpr = new_rMpr*old_rMpr;//output
	mServices.registrationService->applyPatientRegistration(new_rMpr, "Bronchoscopy centerline to tracking data");

    Eigen::Matrix4d display_rMpr = Eigen::Matrix4d::Identity();
            display_rMpr = new_rMpr*display_rMpr;
    std::cout << "New prMt: " << std::endl;
        for (int i = 0; i < 4; i++)
            std::cout << display_rMpr.row(i) << std::endl;

    ToolRep3DPtr activeRep3D = getToolRepIn3DView(mTool);
	if(activeRep3D && activeRep3D->getTracer())
        activeRep3D->getTracer()->clear();

	QColor colorGreen = QColor(0, 255, 0, 255);
	activeRep3D->getTracer()->setColor(colorGreen);
    activeRep3D->getTracer()->addManyPositions(trackerRecordedData_prMt);


}

void BronchoscopyRegistrationWidget::acquisitionStarted()
{
    std::cout << "acquisitionStarted" << std::endl;

	mTool = mSelectToolWidget->getTool();
	if(!mTool)
		mTool = mServices.trackingService->getActiveTool();

    ToolRep3DPtr activeRep3D = this->getToolRepIn3DView(mTool);
	if (!activeRep3D)
		return;

	activeRep3D->getTracer()->start();
}
void BronchoscopyRegistrationWidget::acquisitionStopped()
{
    std::cout << "acquisitionStopped" << std::endl;

	this->acquisitionCancelled();
	QString newUid = mServices.acquisitionService->getLatestSession()->getUid();
	QStringList range = mSessionSelector->getValueRange();
	range << newUid;
	mSessionSelector->setValueRange(range);
	mSessionSelector->setValue(newUid);

    mServices.patientModelService->autoSave();

//	this->saveSessions();
}

void BronchoscopyRegistrationWidget::acquisitionCancelled()
{
	ToolRep3DPtr activeRep3D = this->getToolRepIn3DView(mTool);
	if (!activeRep3D)
		return;

	activeRep3D->getTracer()->stop();
}

void BronchoscopyRegistrationWidget::recordedSessionsChanged()
{
	QStringList sessionUids = getSessionList();
	mSessionSelector->setValueRange(sessionUids);
	if(mSessionSelector->getValue().isEmpty() && !sessionUids.isEmpty())
		mSessionSelector->setValue(sessionUids.last());
}

ToolRep3DPtr BronchoscopyRegistrationWidget::getToolRepIn3DView(ToolPtr tool)
{
	RepContainerPtr repContainer = mServices.visualizationService->get3DReps(0, 0);
	if (repContainer)
		return repContainer->findFirst<ToolRep3D>(tool);
	else
		return ToolRep3DPtr();
}

void BronchoscopyRegistrationWidget::obscuredSlot(bool obscured)
{
//	std::cout << "obscuredSlot: " << obscured << std::endl;

    if (!obscured)
	{
		connect(mServices.acquisitionService.get(), &AcquisitionService::started, this, &BronchoscopyRegistrationWidget::acquisitionStarted);
		connect(mServices.acquisitionService.get(), &AcquisitionService::acquisitionStopped, this, &BronchoscopyRegistrationWidget::acquisitionStopped, Qt::QueuedConnection);
		connect(mServices.acquisitionService.get(), &AcquisitionService::cancelled, this, &BronchoscopyRegistrationWidget::acquisitionCancelled);
		connect(mServices.acquisitionService.get(), &AcquisitionService::recordedSessionsChanged, this, &BronchoscopyRegistrationWidget::recordedSessionsChanged);
        return;
	}

	disconnect(mServices.acquisitionService.get(), &AcquisitionService::started, this, &BronchoscopyRegistrationWidget::acquisitionStarted);
	disconnect(mServices.acquisitionService.get(), &AcquisitionService::acquisitionStopped, this, &BronchoscopyRegistrationWidget::acquisitionStopped);
	disconnect(mServices.acquisitionService.get(), &AcquisitionService::cancelled, this, &BronchoscopyRegistrationWidget::acquisitionCancelled);
	disconnect(mServices.acquisitionService.get(), &AcquisitionService::recordedSessionsChanged, this, &BronchoscopyRegistrationWidget::recordedSessionsChanged);

    ToolRep3DPtr activeRep3D = this->getToolRepIn3DView(mTool);
    if (!activeRep3D)
        return;
    //std::cout << "Slot is cleared" << std::endl;
	activeRep3D->getTracer()->clear();
}

void BronchoscopyRegistrationWidget::createMaxNumberOfGenerations(QDomElement root)
{
	mMaxNumberOfGenerations = DoubleProperty::initialize("Max number of generations in centerline", "",
	"Set max number of generations centerline", 4, DoubleRange(0, 10, 1), 0,
					root);
	mMaxNumberOfGenerations->setGuiRepresentation(DoublePropertyBase::grSLIDER);
}

void BronchoscopyRegistrationWidget::selectSubsetOfBranches(QDomElement root)
{
	mUseSubsetOfGenerations = BoolProperty::initialize("Select branch generations to be used in registration", "",
																			"Select branch generations to be used in registration", false,
																				root);
}

void BronchoscopyRegistrationWidget::useLocalRegistration(QDomElement root)
{
	mUseLocalRegistration = BoolProperty::initialize("Use local registration", "",
																			"Use local registration", false,
																				root);
}

} //namespace cx
