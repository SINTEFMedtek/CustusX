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
#include "cxMeshGlyphsWidget.h"

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

MeshGlyphsWidget::MeshGlyphsWidget(SelectDataStringPropertyBasePtr meshSelector,
							   PatientModelServicePtr patientModelService,
							   ViewServicePtr viewService,
							   QWidget* parent) :
	BaseWidget(parent, "mesh_glyphs_widget", "Glyphs"),
	mPatientModelService(patientModelService),
	mViewService(viewService),
	mMeshSelector(meshSelector)
{
	connect(mMeshSelector.get(), &Property::changed, this, &MeshGlyphsWidget::meshSelectedSlot);
	this->addWidgets();
	this->meshSelectedSlot();
}

MeshGlyphsWidget::~MeshGlyphsWidget()
{
}

void MeshGlyphsWidget::meshSelectedSlot()
{
	if (mMesh == mMeshSelector->getData())
		return;

	if(mMesh)
	{
		disconnect(mGlyphVisualizationCheckBox, SIGNAL(toggled(bool)), mMesh.get(), SLOT(setShowGlyph(bool)));
		disconnect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));
	}

	mMesh = boost::dynamic_pointer_cast<Mesh>(mMeshSelector->getData());

	mGlyphOrientationArrayAdapter->setData(mMesh);
	mGlyphColorArrayAdapter->setData(mMesh);
	mGlyphColorLUTAdapter->setData(mMesh);

	if (!mMesh)
	{
		return;
	}

	mGlyphVisualizationCheckBox->setChecked(mMesh->showGlyph());
	mGlyphVisualizationCheckBox->setEnabled(mMesh->hasGlyph());

	connect(mGlyphVisualizationCheckBox, SIGNAL(toggled(bool)), mMesh.get(), SLOT(setShowGlyph(bool)));

	connect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));
}

void MeshGlyphsWidget::meshChangedSlot()
{
	if(!mMesh)
		return;
	mGlyphVisualizationCheckBox->setChecked(mMesh->showGlyph());
	mGlyphVisualizationCheckBox->setEnabled(mMesh->hasGlyph());
}

void MeshGlyphsWidget::addWidgets()
{
	QVBoxLayout* toptopLayout = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout;
	gridLayout->setMargin(0);
	toptopLayout->addLayout(gridLayout);

	mGlyphOrientationArrayAdapter = StringPropertyGlyphOrientationArray::New(mPatientModelService);
	mGlyphColorArrayAdapter = StringPropertyGlyphColorArray::New(mPatientModelService);
	mGlyphColorLUTAdapter = StringPropertyGlyphLUT::New(mPatientModelService);

	int row = 1;
	mGlyphVisualizationCheckBox = new QCheckBox("Enable glyph visualization");
	mGlyphVisualizationCheckBox->setToolTip("Enable glyph visualization");
	gridLayout->addWidget(mGlyphVisualizationCheckBox, row++,0);
	new LabeledComboBoxWidget(this, mGlyphOrientationArrayAdapter,gridLayout, row++);
	new LabeledComboBoxWidget(this, mGlyphColorArrayAdapter,gridLayout, row++);
	new LabeledComboBoxWidget(this, mGlyphColorLUTAdapter,gridLayout, row++);
	toptopLayout->addStretch();
}



}//end namespace cx
