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

#include "cxBronchoscopyNavigationWidget.h"
#include <QLabel>
#include <QVBoxLayout>
#include "cxMesh.h"
#include "cxDataSelectWidget.h"
#include "cxReporter.h"
#include <vtkPolyData.h>
#include "cxTrackingSystemBronchoscopyService.h"
#include "cxTrackingServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxViewServiceProxy.h"
#include "cxBronchoscopePositionProjection.h"
#include "cxDoubleProperty.h"
#include "cxDataLocations.h"
#include "cxBoolProperty.h"
#include "cxCheckBoxWidget.h"
#include "cxProfile.h"
#include "cxHelperWidgets.h"
#include "cxVisServices.h"
#include "cxVirtualCameraRotationWidget.h"
#include "cxStringPropertySelectTool.h"

namespace cx
{

BronchoscopyNavigationWidget::BronchoscopyNavigationWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "bronchoscopy_navigation_widget", "Bronchoscopy Navigation"),
	mVerticalLayout(new QVBoxLayout(this))
{
	mIsCenerlineProcessed = false;

	mOptions = profile()->getXmlSettings().descend("bronchoscopynavigationwidget");

	mPatientModelService = services->patient();
	mViewService = services->view();
	mTrackingService = services->tracking();


	this->setObjectName("BronchoscopyNavigationWidget");
	this->setWindowTitle("BronchoscopyNavigation");
	this->setWhatsThis(this->defaultWhatsThis());
	
	mToolSelector = StringPropertySelectTool::New(services->tracking());

	mSelectMeshWidget = StringPropertySelectMesh::New(mPatientModelService);
	mSelectMeshWidget->setValueName("Centerline: ");

	mProjectionCenterlinePtr = BronchoscopePositionProjectionPtr(new BronchoscopePositionProjection());
	mProjectionCenterlinePtr->createMaxDistanceToCenterlineOption(mOptions.getElement());
	mProjectionCenterlinePtr->createMaxSearchDistanceOption(mOptions.getElement());
	mProjectionCenterlinePtr->createAlphaOption(mOptions.getElement());

	mProcessCenterlineButton = new QPushButton("Process centerline");
	connect(mProcessCenterlineButton, SIGNAL(clicked()), this, SLOT(processCenterlineSlot()));
	mProcessCenterlineButton->setToolTip(this->defaultWhatsThis());

	mEnableButton = new QPushButton("Enable", this);
	connect(mEnableButton, SIGNAL(clicked()), this, SLOT(enableSlot()));
	mEnableButton->setToolTip(this->defaultWhatsThis());

	mDisableButton = new QPushButton("Disable", this);
	connect(mDisableButton, SIGNAL(clicked()), this, SLOT(disableSlot()));
	mDisableButton->setToolTip(this->defaultWhatsThis());

	mAdvancedOption = new QCheckBox("Use advanced centerline projection", this);
	connect(mAdvancedOption, SIGNAL(clicked()), this, SLOT(showAdvancedOptionsSlot()));


	PropertyPtr maxDistanceToCenterline = mProjectionCenterlinePtr->getMaxDistanceToCenterlineOption();
	PropertyPtr maxSearchDistance = mProjectionCenterlinePtr->getMaxSearchDistanceOption();
	PropertyPtr alpha = mProjectionCenterlinePtr->getAlphaOption();

	mMaxSearchDistanceWidget = createDataWidget(mViewService, mPatientModelService, this, maxSearchDistance);
	mAlphaWidget = createDataWidget(mViewService, mPatientModelService, this, alpha);

	mVerticalLayout->addWidget(sscCreateDataWidget(this, mToolSelector));
	mVerticalLayout->addWidget(new DataSelectWidget(mViewService, mPatientModelService, this, mSelectMeshWidget));
	mVerticalLayout->addWidget(mProcessCenterlineButton);
	mVerticalLayout->addWidget(createDataWidget(mViewService, mPatientModelService, this, maxDistanceToCenterline));
	mVerticalLayout->addWidget(mAdvancedOption);

	mVerticalLayout->addWidget(mMaxSearchDistanceWidget);
	mVerticalLayout->addWidget(mAlphaWidget);
	mVerticalLayout->addWidget(mEnableButton);
	mVerticalLayout->addWidget(mDisableButton);
	mVerticalLayout->addWidget(this->createHorizontalLine());
	mVerticalLayout->addWidget(new VirtualCameraRotationWidget(services, mToolSelector, this));
	mVerticalLayout->addStretch();
	
	mEnableButton->setEnabled(false);
	mDisableButton->setEnabled(false);
	this->showAdvancedOptionsSlot();

}

BronchoscopyNavigationWidget::~BronchoscopyNavigationWidget()
{
}

void BronchoscopyNavigationWidget::processCenterlineSlot()
{
	if(!mSelectMeshWidget->getMesh())
	{
		reportError("No centerline");
		return;
	}
	vtkPolyDataPtr centerline = mSelectMeshWidget->getMesh()->getVtkPolyData();//input
	Transform3D rMd = mSelectMeshWidget->getMesh()->get_rMd();
	Transform3D rMpr = mPatientModelService->get_rMpr();
	Transform3D prMd = rMpr.inverse()*rMd;

	mProjectionCenterlinePtr->processCenterline(centerline, rMd, rMpr);
	mIsCenerlineProcessed = true;
	mEnableButton->setEnabled(true);
}

void BronchoscopyNavigationWidget::enableSlot()
{
	if(!mIsCenerlineProcessed)
	{
		reportError("Centerline not processed");
		return;
	}
	
	if(!mToolSelector->getTool())
		{
			reportError("Tool not selected");
			return;
		}
		

	mProjectionCenterlinePtr->setAdvancedCenterlineOption(mAdvancedOption->isChecked());
	if (!mTrackingSystem)
	{
//		std::vector<TrackingSystemServicePtr> trackingSystems = mTrackingService->getTrackingSystems();
//		for (int i=0; i<trackingSystems.size(); i++ )
//			if(trackingSystems[i]->getUid() == "org.custusx.bronchoscopynavigation")
//				mTrackingSystem = trackingSystems[i];
//		if(!mTrackingService)
//			CX_LOG_WARNING() << "Did not find bronchoscopy navigation tracking system.";

		mTrackingSystem = TrackingSystemBronchoscopyServicePtr(new TrackingSystemBronchoscopyService(mTrackingService, mProjectionCenterlinePtr, mToolSelector->getTool()));
		mTrackingService->unInstallTrackingSystem(mTrackingSystem->getBase());
		mTrackingService->installTrackingSystem(mTrackingSystem);
	}
	mEnableButton->setEnabled(false);
	mDisableButton->setEnabled(true);
	
	std::cout << "BronchoscopyNavigation started. Position locked to centerline." << std::endl;
}

void BronchoscopyNavigationWidget::disableSlot()
{
	if (mTrackingSystem)
	{
		mTrackingService->unInstallTrackingSystem(mTrackingSystem);
		mTrackingService->installTrackingSystem(mTrackingSystem->getBase());
		mTrackingSystem.reset();
	}

	std::cout << "BronchoscopyNavigation stopped." << std::endl;
	mEnableButton->setEnabled(true);
	mDisableButton->setEnabled(false);

}


void BronchoscopyNavigationWidget::showAdvancedOptionsSlot()
{
	if(mAdvancedOption->isChecked())
	{
		mMaxSearchDistanceWidget->show();
		mAlphaWidget->show();
	}
	else{
		mMaxSearchDistanceWidget->hide();
		mAlphaWidget->hide();
	}
}

QString BronchoscopyNavigationWidget::defaultWhatsThis() const
{
	return	"<html>"
					"<h3>BronchoscopyNavigation plugin.</h3>"
					"<p>Locks tool position to CT centerline.</p>"
					"</html>";
}


} /* namespace cx */
