/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxBronchoscopyRegistrationWidget.h"
#include <vtkPolyData.h>
#include "cxTransform3D.h"
#include "cxDataSelectWidget.h"
#include "cxTrackingService.h"
#include "cxMesh.h"
#include "cxSelectDataStringProperty.h"
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
#include "cxRecordTrackingWidget.h"


namespace cx
{
BronchoscopyRegistrationWidget::BronchoscopyRegistrationWidget(RegServicesPtr services, QWidget* parent) :
	RegistrationBaseWidget(services, parent, "org_custusx_registration_method_bronchoscopy_widget",
												 "Bronchoscopy Registration"),
	mBronchoscopyRegistration(new BronchoscopyRegistration()),
	mServices(services),
	mRecordTrackingWidget(NULL)
{
	mVerticalLayout = new QVBoxLayout(this);
}

void BronchoscopyRegistrationWidget::prePaintEvent()
{
	if (!mRecordTrackingWidget)
	{
		this->setup();
	}
}

void BronchoscopyRegistrationWidget::setup()
{
	mOptions = profile()->getXmlSettings().descend("bronchoscopyregistrationwidget");

	mSelectMeshWidget = StringPropertySelectMesh::New(mServices->patient());
	mSelectMeshWidget->setValueName("Centerline: ");

	//this->initializeTrackingService();

	connect(mServices->patient().get(),&PatientModelService::patientChanged,this,&BronchoscopyRegistrationWidget::clearDataOnNewPatient);

	mProcessCenterlineButton = new QPushButton("Process centerline");
	connect(mProcessCenterlineButton, SIGNAL(clicked()), this, SLOT(processCenterlineSlot()));
	mProcessCenterlineButton->setToolTip(this->defaultWhatsThis());

	//	mBronchoscopeRegistrationPtr = BronchoscopeRegistrationPtr(new BronchoscopePositionProjection());
	//	mProjectionCenterlinePtr->createMaxDistanceToCenterlineOption(mOptions.getElement());

	mRegisterButton = new QPushButton("Register");
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));
	mRegisterButton->setToolTip(this->defaultWhatsThis());

	mRecordTrackingWidget = new RecordTrackingWidget(mOptions.descend("recordTracker"),
																									 mServices->acquisition(), mServices,
																									 "bronc_path",
																									 this);
	mRecordTrackingWidget->getSessionSelector()->setHelp("Select bronchoscope path for registration");
	mRecordTrackingWidget->getSessionSelector()->setDisplayName("Bronchoscope path");

	mVerticalLayout->setMargin(0);
	mVerticalLayout->addWidget(new DataSelectWidget(mServices->view(), mServices->patient(), this, mSelectMeshWidget));

	this->selectSubsetOfBranches(mOptions.getElement());
	this->createMaxNumberOfGenerations(mOptions.getElement());
	this->useLocalRegistration(mOptions.getElement());
	this->createMaxLocalRegistrationDistance(mOptions.getElement());

	//	PropertyPtr maxLocalRegistrationDistance = mProjectionCenterlinePtr->getMaxLocalRegistrationDistanceOption();

	mVerticalLayout->addWidget(new CheckBoxWidget(this, mUseSubsetOfGenerations));
	mVerticalLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), this, mMaxNumberOfGenerations));
	mVerticalLayout->addWidget(mProcessCenterlineButton);
	mVerticalLayout->addWidget(mRecordTrackingWidget);
	mVerticalLayout->addWidget(new CheckBoxWidget(this, mUseLocalRegistration));
	mVerticalLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), this, mMaxLocalRegistrationDistance));
	mVerticalLayout->addWidget(mRegisterButton);

	mVerticalLayout->addStretch();
}

QString BronchoscopyRegistrationWidget::defaultWhatsThis() const
{
	return QString();
}

void BronchoscopyRegistrationWidget::initializeTrackingService()
{
	if(mServices->tracking()->getState() < Tool::tsCONFIGURED)
		mServices->tracking()->setState(Tool::tsCONFIGURED);
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
		std::cout << "name: " << name << std::endl;
		std::cout << "uid: " << uid << std::endl;
		mMesh = mServices->patient()->createSpecificData<Mesh>(uid, name);
	}
	mMesh->setVtkPolyData(processedCenterline);
	mMesh->setColor(QColor(0, 0, 255, 255));
	mServices->patient()->insertData(mMesh);
	mServices->view()->autoShowData(mMesh);
}

void BronchoscopyRegistrationWidget::registerSlot()
{
	if(!mBronchoscopyRegistration->isCenterlineProcessed())
	{
		reportError("Centerline not processed");
		return;
	}

	Transform3D old_rMpr = mServices->patient()->get_rMpr();//input to registrationAlgorithm
	//std::cout << "rMpr: " << std::endl;
	//std::cout << old_rMpr << std::endl;

	TimedTransformMap trackerRecordedData_prMt = mRecordTrackingWidget->getRecordedTrackerData_prMt();

	if(trackerRecordedData_prMt.empty())
	{
		reportError("No positions");
		return;
	}

	Transform3D new_rMpr;

	if(mUseLocalRegistration->getValue()){
		std::cout << "Running local registration with max distance " << mMaxLocalRegistrationDistance->getValue() << " mm." << std::endl;
		new_rMpr = Transform3D(mBronchoscopyRegistration->runBronchoscopyRegistration(trackerRecordedData_prMt,old_rMpr,mMaxLocalRegistrationDistance->getValue()));
	}
	else{
		std::cout << "Running global registration." << std::endl;
		new_rMpr = Transform3D(mBronchoscopyRegistration->runBronchoscopyRegistration(trackerRecordedData_prMt,old_rMpr,0));
	}

	new_rMpr = new_rMpr*old_rMpr;//output
	mServices->registration()->addPatientRegistration(new_rMpr, "Bronchoscopy centerline to tracking data");

	Eigen::Matrix4d display_rMpr = Eigen::Matrix4d::Identity();
	display_rMpr = new_rMpr*display_rMpr;
	std::cout << "New prMt: " << std::endl;
	for (int i = 0; i < 4; i++)
		std::cout << display_rMpr.row(i) << std::endl;

	//	mRecordTrackingWidget->showSelectedRecordingInView();

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

void BronchoscopyRegistrationWidget::createMaxLocalRegistrationDistance(QDomElement root)
{
	mMaxLocalRegistrationDistance = DoubleProperty::initialize("Max local registration distance (mm)", "",
																														 "Set max distance for local registration in mm", 30, DoubleRange(1, 200, 1), 0,
																														 root);
	mMaxLocalRegistrationDistance->setGuiRepresentation(DoubleProperty::grSLIDER);
}

void BronchoscopyRegistrationWidget::clearDataOnNewPatient()
{
	mMesh.reset();
}
} //namespace cx
