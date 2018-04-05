/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
