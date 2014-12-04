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
#include "cxVisualizationServiceProxy.h"
#include "cxBronchoscopePositionProjection.h"
#include "cxDataAdapterHelper.h"
#include "cxDoubleDataAdapterXml.h"
#include "cxDataLocations.h"
#include "cxBoolDataAdapterXml.h"
#include "cxCheckBoxWidget.h"

namespace cx
{

BronchoscopyNavigationWidget::BronchoscopyNavigationWidget(ctkPluginContext *context, QWidget* parent) :
    QWidget(parent),
    mVerticalLayout(new QVBoxLayout(this))
{
	mOptions = XmlOptionFile(DataLocations::getXmlSettingsFile()).descend("bronchoscopynavigationwidget");

	mPatientModelService = PatientModelServicePtr(new PatientModelServiceProxy(context));
	mVisualizationService = VisualizationServicePtr(new VisualizationServiceProxy(context));
	mTrackingService = TrackingServiceProxy::create(context);


	this->setObjectName("BronchoscopyNavigationWidget");
	this->setWindowTitle("BronchoscopyNavigation");
    this->setWhatsThis(this->defaultWhatsThis());

	mSelectMeshWidget = SelectMeshStringDataAdapter::New(mPatientModelService);
	mSelectMeshWidget->setValueName("Centerline: ");

	//mSelectMaxDistanceWidget =
	mProjectionCenterlinePtr = BronchoscopePositionProjectionPtr(new BronchoscopePositionProjection());
	mProjectionCenterlinePtr->createMaxDistanceToCenterlineOption(mOptions.getElement());

	mProcessCenterlineButton = new QPushButton("Process centerline");
	connect(mProcessCenterlineButton, SIGNAL(clicked()), this, SLOT(processCenterlineSlot()));
	mProcessCenterlineButton->setToolTip(this->defaultWhatsThis());

	mEnableButton = new QPushButton("Enable", this);
	connect(mEnableButton, SIGNAL(clicked()), this, SLOT(enableSlot()));
	mEnableButton->setToolTip(this->defaultWhatsThis());

	mDisableButton = new QPushButton("Disable", this);
	connect(mDisableButton, SIGNAL(clicked()), this, SLOT(disableSlot()));
	mDisableButton->setToolTip(this->defaultWhatsThis());

	this->useAdvancedCenterlineProjection(mOptions.getElement());


	DataAdapterPtr maxDistanceToCenterline = mProjectionCenterlinePtr->getMaxDistanceToCenterlineOption();

	mVerticalLayout->addWidget(new DataSelectWidget(mVisualizationService, mPatientModelService, this, mSelectMeshWidget));
	mVerticalLayout->addWidget(mProcessCenterlineButton);
	mVerticalLayout->addWidget(new CheckBoxWidget(this, mUseAdvancedCenterlineProjection));
	mVerticalLayout->addWidget(createDataWidget(mVisualizationService, mPatientModelService, this, maxDistanceToCenterline));
	mVerticalLayout->addWidget(mEnableButton);
	mVerticalLayout->addWidget(mDisableButton);
	mVerticalLayout->addStretch();
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

	mProjectionCenterlinePtr->processCenterline(centerline, rMd);
}

void BronchoscopyNavigationWidget::enableSlot()
{
	std::cout << "BronchoscopyNavigation started. Position locked to centerline." << std::endl;
//	mTool = toolManager()->getDominantTool();

	if(!mSelectMeshWidget->getMesh())
	{
		reportError("No centerline");
		return;
	}
	vtkPolyDataPtr centerline = mSelectMeshWidget->getMesh()->getVtkPolyData();//input
	Transform3D rMd = mSelectMeshWidget->getMesh()->get_rMd();
    Transform3D rMpr = mPatientModelService->get_rMpr();
    Transform3D prMd = rMpr.inverse()*rMd;

	mProjectionCenterlinePtr->setCenterline(centerline, prMd, mUseAdvancedCenterlineProjection->getValue());
	if (!mTrackingSystem)
	{
		mTrackingSystem = TrackingSystemBronchoscopyServicePtr(new TrackingSystemBronchoscopyService(mTrackingService, mProjectionCenterlinePtr));
		mTrackingService->unInstallTrackingSystem(mTrackingSystem->getBase());
		mTrackingService->installTrackingSystem(mTrackingSystem);
	}


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

}

QString BronchoscopyNavigationWidget::defaultWhatsThis() const
{
  return "<html>"
	  "<h3>BronchoscopyNavigation plugin.</h3>"
	  "<p>Locks tool position to CT centerline.</p>"
      "</html>";
}

void BronchoscopyNavigationWidget::useAdvancedCenterlineProjection(QDomElement root)
{
	mUseAdvancedCenterlineProjection = BoolDataAdapterXml::initialize("Use advanced centerline projection", "",
																			"Use advanced centerline projection: Avoiding tool position to jump between adjacent branches.", false,
																				root);
}



} /* namespace cx */
