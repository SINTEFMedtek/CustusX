/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxMeshTextureWidget.h"
#include "cxMesh.h"


namespace cx
{

MeshTextureWidget::MeshTextureWidget(SelectDataStringPropertyBasePtr meshSelector, PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget* parent)
	: BaseWidget(parent, "mesh_texture_widget", "Texture"),
	  mPatientModelService(patientModelService),
	  mViewService(viewService),
	  mMeshSelector(meshSelector)
{
	QVBoxLayout* toptopLayout = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout;
	gridLayout->setMargin(0);
	toptopLayout->addLayout(gridLayout);
	mOptionsWidget = new OptionsWidget(mViewService, mPatientModelService,this);
	toptopLayout->addWidget(mOptionsWidget);
	toptopLayout->addStretch();

	this->clearUI();
	connect(mMeshSelector.get(), &Property::changed, this, &MeshTextureWidget::meshSelectedSlot);
	this->setModified();
}

MeshTextureWidget::~MeshTextureWidget()
{

}

void MeshTextureWidget::prePaintEvent()
{
	this->setupUI();
}

void MeshTextureWidget::meshSelectedSlot()
{
	if (mMesh == mMeshSelector->getData())
		return;

	mMesh = boost::dynamic_pointer_cast<Mesh>(mMeshSelector->getData());

	this->setupUI();
	mOptionsWidget->rebuild();
}

void MeshTextureWidget::clearUI()
{
	this->setModified();
}

void MeshTextureWidget::setupUI()
{
	if (!mMesh)
		return;

	std::vector<PropertyPtr> properties = mMesh->getTextureData().getProperties();
	mOptionsWidget->setOptions(mMesh->getUid(), properties, true);

}

}//end namespace cx
