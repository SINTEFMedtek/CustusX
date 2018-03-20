/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxMeshPropertiesWidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include "cxHelperWidgets.h"
#include "cxSelectDataStringProperty.h"
#include "cxReplacableContentWidget.h"

namespace cx
{

MeshPropertiesWidget::MeshPropertiesWidget(SelectDataStringPropertyBasePtr meshSelector,
							   PatientModelServicePtr patientModelService,
							   ViewServicePtr viewService,
							   QWidget* parent) :
	BaseWidget(parent, "mesh_properties_widget", "Properties"),
	mPatientModelService(patientModelService),
	mViewService(viewService),
	mMeshSelector(meshSelector)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);

	mPropertiesWidget = new ReplacableContentWidget(this);
	layout->addWidget(mPropertiesWidget);
	layout->addStretch();

	this->clearUI();

	connect(mMeshSelector.get(), &Property::changed, this, &MeshPropertiesWidget::meshSelectedSlot);
	this->setModified();
}

MeshPropertiesWidget::~MeshPropertiesWidget()
{
}

void MeshPropertiesWidget::prePaintEvent()
{
	this->setupUI();
}

void MeshPropertiesWidget::meshSelectedSlot()
{
	if (mMesh == mMeshSelector->getData())
		return;

	// clear gui, ready for next modified
	this->clearUI();

	mMesh = boost::dynamic_pointer_cast<Mesh>(mMeshSelector->getData());
	this->setModified();
}

void MeshPropertiesWidget::clearUI()
{
	mPropertiesWidget->setWidgetDeleteOld(new QLabel("no\nmesh\nselected"));
	this->setModified();
}

void MeshPropertiesWidget::setupUI()
{
	if (!mMesh)
		return;

	QWidget* widget = new QWidget;
	QGridLayout* layout = new QGridLayout(widget);
	layout->setMargin(0);
	mPropertiesWidget->setWidgetDeleteOld(widget);

	std::vector<PropertyPtr> properties = mMesh->getProperties().mProperties;
	for (unsigned i=0; i<properties.size(); ++i)
	{
		createDataWidget(mViewService, mPatientModelService, this, properties[i], layout, i);
	}
}

}//end namespace cx
