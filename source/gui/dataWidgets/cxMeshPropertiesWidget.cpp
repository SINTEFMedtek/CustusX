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
