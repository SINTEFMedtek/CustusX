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
#include "cxToolManager.h"
#include "cxReporter.h"
#include "cxVisualizationService.h"
#include "cxRegistrationService.h"
#include "cxPatientModelService.h"

#include "cxLegacySingletons.h"

namespace cx
{
PlateRegistrationWidget::PlateRegistrationWidget(regServices services, QWidget* parent) :
	RegistrationBaseWidget(services, parent, "PlateRegistrationWidget", "Plate Registration"),
	mPlateRegistrationButton(new QPushButton("Load registration points", this)),
	mReferenceToolInfoLabel(new QLabel("", this))
{
	connect(mPlateRegistrationButton, SIGNAL(clicked()), this, SLOT(plateRegistrationSlot()));
	connect(toolManager(), &ToolManager::stateChanged, this, &PlateRegistrationWidget::internalUpdate);

	QVBoxLayout* toptopLayout = new QVBoxLayout(this);
	toptopLayout->addWidget(mReferenceToolInfoLabel);
	toptopLayout->addWidget(mPlateRegistrationButton);
	toptopLayout->addStretch();

	this->internalUpdate();
}

PlateRegistrationWidget::~PlateRegistrationWidget()
{

}

QString PlateRegistrationWidget::defaultWhatsThis() const
{
	return "<html>"
		   "<h3>Plate registration.</h3>"
		   "<p>Internally register the reference plates reference points as landmarks.</p>"
		   "<p><i>Click the button to load landmarks.</i></p>"
		   "</html>";
}

void PlateRegistrationWidget::showEvent(QShowEvent* event)
{
  BaseWidget::showEvent(event);
  connect(mServices.patientModelService->getPatientLandmarks().get(), &Landmarks::landmarkAdded,
		  this, &PlateRegistrationWidget::landmarkUpdatedSlot);
  connect(mServices.patientModelService->getPatientLandmarks().get(), &Landmarks::landmarkRemoved,
		  this, &PlateRegistrationWidget::landmarkUpdatedSlot);

  mServices.visualizationService->setRegistrationMode(rsPATIENT_REGISTRATED);
}

void PlateRegistrationWidget::hideEvent(QHideEvent* event)
{
  BaseWidget::hideEvent(event);
  disconnect(mServices.patientModelService->getPatientLandmarks().get(), &Landmarks::landmarkAdded,
			 this, &PlateRegistrationWidget::landmarkUpdatedSlot);
  disconnect(mServices.patientModelService->getPatientLandmarks().get(), &Landmarks::landmarkRemoved,
			 this, &PlateRegistrationWidget::landmarkUpdatedSlot);

  mServices.visualizationService->setRegistrationMode(rsNOT_REGISTRATED);
}

void PlateRegistrationWidget::landmarkUpdatedSlot()
{
	mServices.registrationService->doFastRegistration_Translation();
}

void PlateRegistrationWidget::plateRegistrationSlot()
{
	mServices.patientModelService->getPatientLandmarks()->clear();

  ToolPtr refTool = toolManager()->getReferenceTool();
  if(!refTool)//cannot register without a reference tool
  {
    reporter()->sendDebug("No refTool");
    return;
  }
  std::map<int, Vector3D> referencePoints = refTool->getReferencePoints();
  if(referencePoints.empty()) //cannot register without at least 1 reference point
  {
    reporter()->sendDebug("No referenceppoints in reftool "+refTool->getName());
    return;
  }

  std::map<int, Vector3D>::iterator it = referencePoints.begin();
  for(; it != referencePoints.end(); ++it)
  {
	QString uid = mServices.patientModelService->addLandmark();
	mServices.patientModelService->setLandmarkName(uid, qstring_cast(it->first));
	mServices.patientModelService->getPatientLandmarks()->setLandmark(Landmark(uid, it->second));
  }

  // set all landmarks as not active as default
  LandmarkPropertyMap map = mServices.patientModelService->getLandmarkProperties();
  LandmarkPropertyMap::iterator landmarkIt = map.begin();
  for(; landmarkIt != map.end(); ++landmarkIt)
  {
	mServices.patientModelService->setLandmarkActive(landmarkIt->first, false);
  }

  //we don't want the user to load the landmarks twice, it will result in alot of global landmarks...
  mPlateRegistrationButton->setDisabled(true);
}

void PlateRegistrationWidget::internalUpdate()
{
  ToolPtr refTool = toolManager()->getReferenceTool();

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
