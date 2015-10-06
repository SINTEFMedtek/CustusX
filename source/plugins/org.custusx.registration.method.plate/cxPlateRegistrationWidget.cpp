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

#include "cxPlateRegistrationWidget.h"

#include <QPushButton>
#include <QLabel>
#include "cxTypeConversions.h"
#include "cxTrackingService.h"
#include "cxLogger.h"
#include "cxViewService.h"
#include "cxRegistrationService.h"
#include "cxPatientModelService.h"
#include "cxTrackingService.h"
#include "cxLandmark.h"
#include "cxViewGroupData.h"

namespace cx
{
PlateRegistrationWidget::PlateRegistrationWidget(RegServicesPtr services, QWidget* parent) :
	RegistrationBaseWidget(services, parent, "org_custusx_registration_method_plate_reference_landmarks", "Plate Registration Reference landmarks"),
	mPlateRegistrationButton(new QPushButton("Load registration points", this)),
	mReferenceToolInfoLabel(new QLabel("", this))
{
	connect(mPlateRegistrationButton, SIGNAL(clicked()), this, SLOT(plateRegistrationSlot()));
	connect(mServices->tracking().get(), &TrackingService::stateChanged, this, &PlateRegistrationWidget::internalUpdate);

	QVBoxLayout* toptopLayout = new QVBoxLayout(this);
	toptopLayout->addWidget(mReferenceToolInfoLabel);
	toptopLayout->addWidget(mPlateRegistrationButton);
	toptopLayout->addStretch();

	this->internalUpdate();
}

PlateRegistrationWidget::~PlateRegistrationWidget()
{

}

void PlateRegistrationWidget::showEvent(QShowEvent* event)
{
  BaseWidget::showEvent(event);
  connect(mServices->patient()->getPatientLandmarks().get(), &Landmarks::landmarkAdded,
		  this, &PlateRegistrationWidget::landmarkUpdatedSlot);
  connect(mServices->patient()->getPatientLandmarks().get(), &Landmarks::landmarkRemoved,
		  this, &PlateRegistrationWidget::landmarkUpdatedSlot);

  mServices->view()->getGroup(0)->setRegistrationMode(rsPATIENT_REGISTRATED);
}

void PlateRegistrationWidget::hideEvent(QHideEvent* event)
{
  BaseWidget::hideEvent(event);
  disconnect(mServices->patient()->getPatientLandmarks().get(), &Landmarks::landmarkAdded,
			 this, &PlateRegistrationWidget::landmarkUpdatedSlot);
  disconnect(mServices->patient()->getPatientLandmarks().get(), &Landmarks::landmarkRemoved,
			 this, &PlateRegistrationWidget::landmarkUpdatedSlot);

  mServices->view()->getGroup(0)->setRegistrationMode(rsNOT_REGISTRATED);
}

void PlateRegistrationWidget::landmarkUpdatedSlot()
{
	mServices->registration()->doFastRegistration_Translation();
}

void PlateRegistrationWidget::plateRegistrationSlot()
{
	mServices->patient()->getPatientLandmarks()->clear();

  ToolPtr refTool = mServices->tracking()->getReferenceTool();
  if(!refTool)//cannot register without a reference tool
  {
	reportDebug("No refTool");
    return;
  }
  std::map<int, Vector3D> referencePoints = refTool->getReferencePoints();
  if(referencePoints.empty()) //cannot register without at least 1 reference point
  {
	reportDebug("No referenceppoints in reftool "+refTool->getName());
    return;
  }

  std::map<int, Vector3D>::iterator it = referencePoints.begin();
  for(; it != referencePoints.end(); ++it)
  {
	QString uid = mServices->patient()->addLandmark();
	mServices->patient()->setLandmarkName(uid, qstring_cast(it->first));
	mServices->patient()->getPatientLandmarks()->setLandmark(Landmark(uid, it->second));
  }

  // set all landmarks as not active as default
  LandmarkPropertyMap map = mServices->patient()->getLandmarkProperties();
  LandmarkPropertyMap::iterator landmarkIt = map.begin();
  for(; landmarkIt != map.end(); ++landmarkIt)
  {
	mServices->patient()->setLandmarkActive(landmarkIt->first, false);
  }

  //we don't want the user to load the landmarks twice, it will result in alot of global landmarks...
  mPlateRegistrationButton->setDisabled(true);
}

void PlateRegistrationWidget::internalUpdate()
{
  ToolPtr refTool = mServices->tracking()->getReferenceTool();

  QString labelText = "";
  if(!refTool || refTool->getReferencePoints().size()<1)
  {
    mPlateRegistrationButton->setDisabled(true);

    labelText.append("Configure the tracker to have <br>a reference frame that has at least <br>one reference point.");
  }else
  {
    mPlateRegistrationButton->setEnabled(true);

    labelText = "<b>Reference tool selected:</b> <br>";
    labelText.append("Tool name: <i>"+refTool->getName()+"</i><br>");
    labelText.append("Number of defined reference points: <i>"+qstring_cast(refTool->getReferencePoints().size())+"</i>");
  }

  mReferenceToolInfoLabel->setText(labelText);
}

}//namespace cx
