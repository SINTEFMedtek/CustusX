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
#include "cxMeshInfoWidget.h"

#include <QVBoxLayout>
#include "cxImage.h"

#include "cxDoubleProperty.h"
#include "cxRegistrationTransform.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxLabeledLineEditWidget.h"
#include "cxMeshHelpers.h"
#include "cxHelperWidgets.h"
#include "cxColorProperty.h"
#include "cxDataLocations.h"
#include "cxDataInterface.h"
#include "cxDataSelectWidget.h"
#include "cxSelectDataStringProperty.h"

#include "cxPatientModelService.h"
#include "cxLogger.h"
#include "cxProfile.h"
#include "cxVisServices.h"
#include "cxSelectClippersForDataWidget.h"
#include "cxMeshGlyphsWidget.h"
#include "cxMeshPropertiesWidget.h"
#include "cxMeshTextureWidget.h"

namespace cx
{

ActiveMeshPropertiesWidget::ActiveMeshPropertiesWidget(VisServicesPtr services, QWidget *parent) :
		TabbedWidget(parent, "mesh_info_widget", "Mesh Properties")
{
	this->setToolTip("Mesh properties");

	StringPropertyActiveDataPtr activeMeshProperty = StringPropertyActiveData::New(services->patient(), "mesh");
	activeMeshProperty->setValueName("Active Mesh");

	this->insertWidgetAtTop(new DataSelectWidget(services->view(), services->patient(), this, activeMeshProperty));

	this->addTab(new MeshInfoWidget(activeMeshProperty, services->patient(), services->view(), this), "Info");
	this->addTab(new MeshPropertiesWidget(activeMeshProperty, services->patient(), services->view(), this), "Properties");
	this->addTab(new MeshGlyphsWidget(activeMeshProperty, services->patient(), services->view(), this), "Glyph");
	this->addTab(new SelectClippersForMeshWidget(services, this), "Clip");
    this->addTab(new MeshTextureWidget(/*activeMeshProperty, services->patient(), services->view(), */this), "Texture");
}

//---------------------------------------------------------
//---------------------------------------------------------

MeshInfoWidget::MeshInfoWidget(SelectDataStringPropertyBasePtr meshSelector,
							   PatientModelServicePtr patientModelService,
							   ViewServicePtr viewService,
							   QWidget* parent) :
	InfoWidget(parent, "MeshCoreInfoWidget", "Mesh Properties"),
	mPatientModelService(patientModelService),
	mViewService(viewService),
	mMeshSelector(meshSelector)
{
	connect(mMeshSelector.get(), &Property::changed, this, &MeshInfoWidget::meshSelectedSlot);
	this->addWidgets();
	this->meshSelectedSlot();
}

MeshInfoWidget::~MeshInfoWidget()
{
}

void MeshInfoWidget::meshSelectedSlot()
{
	if (mMesh == mMeshSelector->getData())
	return;

	if(mMesh)
	{
		disconnect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));
    }

	mMesh = boost::dynamic_pointer_cast<Mesh>(mMeshSelector->getData());

	if (!mMesh)
	{
        mParentFrameAdapter->setData(mMesh);
        mNameAdapter->setData(mMesh);
		mUidAdapter->setData(mMesh);
		return;
	}

    connect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));

    mParentFrameAdapter->setData(mMesh);
	mNameAdapter->setData(mMesh);
	mUidAdapter->setData(mMesh);

	std::map<std::string, std::string> info = getDisplayFriendlyInfo(mMesh);
	this->populateTableWidget(info);
}

void MeshInfoWidget::importTransformSlot()
{
  if(!mMesh)
    return;
  DataPtr parent = mPatientModelService->getData(mMesh->getParentSpace());
  if (!parent)
    return;
  mMesh->get_rMd_History()->setRegistration(parent->get_rMd());
  report("Assigned rMd from volume [" + parent->getName() + "] to surface [" + mMesh->getName() + "]");
}
  
void MeshInfoWidget::meshChangedSlot()
{
	if(!mMesh)
		return;
}

void MeshInfoWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void MeshInfoWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}

void MeshInfoWidget::addWidgets()
{
	MeshPtr mesh = boost::dynamic_pointer_cast<Mesh>(mMeshSelector->getData());

	QPushButton* importTransformButton = new QPushButton("Import Transform from Parent", this);
	importTransformButton->setToolTip("Replace data transform with that of the parent data.");
	connect(importTransformButton, SIGNAL(clicked()), this, SLOT(importTransformSlot()));

	mUidAdapter = StringPropertyDataUidEditable::New();
	mNameAdapter = StringPropertyDataNameEditable::New();
    mParentFrameAdapter = StringPropertyParentFrame::New(mPatientModelService);

	int row = 1;

	new LabeledLineEditWidget(this, mUidAdapter, gridLayout, row++);
	new LabeledLineEditWidget(this, mNameAdapter, gridLayout, row++);
	new LabeledComboBoxWidget(this, mParentFrameAdapter, gridLayout, row++);
	gridLayout->addWidget(mTableWidget, row++, 0, 1, 2);
	gridLayout->addWidget(importTransformButton, row++, 0, 1, 2);

	this->addStretch();
}



}//end namespace cx
