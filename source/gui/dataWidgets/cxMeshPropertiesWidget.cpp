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
#include "cxMeshPropertiesWidget.h"

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

namespace cx
{

MeshPropertiesWidget::MeshPropertiesWidget(SelectDataStringPropertyBasePtr meshSelector,
							   PatientModelServicePtr patientModelService,
							   ViewServicePtr viewService,
							   QWidget* parent) :
	BaseWidget(parent, "mesh_glyphs_widget", "Glyphs"),
	mPatientModelService(patientModelService),
	mViewService(viewService),
	mMeshSelector(meshSelector)
{
	connect(mMeshSelector.get(), &Property::changed, this, &MeshPropertiesWidget::meshSelectedSlot);
	this->addWidgets();
	this->meshSelectedSlot();
}

MeshPropertiesWidget::~MeshPropertiesWidget()
{
}

void MeshPropertiesWidget::setColorSlot()
{
  if(!mMesh)
	return;
  // Implement like TransferFunctionColorWidget::setColorSlot()
  // to prevent crash problems
  QTimer::singleShot(1, this, SLOT(setColorSlotDelayed()));
}

void MeshPropertiesWidget::setColorSlotDelayed()
{
	if(!mMesh)
	  return;
	mMesh->setColor(mColorAdapter->getValue());
}

void MeshPropertiesWidget::meshSelectedSlot()
{
	if (mMesh == mMeshSelector->getData())
		return;

	if(mMesh)
	{
		mMesh->setVisSize((double) mVisSizeWidget->getValue());
		disconnect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));
	}

	mMesh = boost::dynamic_pointer_cast<Mesh>(mMeshSelector->getData());

	if (!mMesh)
	{
		return;
	}

	mVisSizeWidget->setValue(mMesh->getVisSize());
	mColorAdapter->setValue(mMesh->getColor());

	connect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));
}

void MeshPropertiesWidget::meshChangedSlot()
{
	if(!mMesh)
		return;
	mBackfaceCullingCheckBox->setChecked(mMesh->getBackfaceCulling());
	mFrontfaceCullingCheckBox->setChecked(mMesh->getFrontfaceCulling());
	mColorAdapter->setValue(mMesh->getColor());
	mMesh->setVisSize((double) mVisSizeWidget->getValue());
}

void MeshPropertiesWidget::addWidgets()
{
	QVBoxLayout* toptopLayout = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout;
	gridLayout->setMargin(0);
	toptopLayout->addLayout(gridLayout);

	mColorAdapter = ColorProperty::initialize("Color", "", "Mesh color", QColor("red"));//, options.getElement());
	connect(mColorAdapter.get(), SIGNAL(changed()), this, SLOT(setColorSlot()));

	mVisSizeWidget= DoubleProperty::initialize("visSize", "Point size", "Visualized size of points, glyphs etc.",1, DoubleRange(1, 20, 1), 0);
	mVisSizeWidget->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	connect(mVisSizeWidget.get(), &Property::changed, this, &MeshPropertiesWidget::meshChangedSlot);

	int row = 1;

	mBackfaceCullingCheckBox = new QCheckBox("Backface culling");
	mBackfaceCullingCheckBox->setToolTip("Set backface culling on. This makes transparent meshes work, but only draws outside mesh walls (eg. navigating inside meshes will not work).");
	gridLayout->addWidget(mBackfaceCullingCheckBox, row++, 0);

	mFrontfaceCullingCheckBox = new QCheckBox("Frontface culling");
	mFrontfaceCullingCheckBox->setToolTip("Set frontface culling on. Can be used to make transparent meshes work from inside the meshes.");
	gridLayout->addWidget(mFrontfaceCullingCheckBox, row++, 0);

	gridLayout->addWidget(sscCreateDataWidget(this, mColorAdapter, gridLayout, row++));
	gridLayout->addWidget(createDataWidget(mViewService, mPatientModelService, this, mVisSizeWidget),row++,0);

	toptopLayout->addStretch();
}



}//end namespace cx
