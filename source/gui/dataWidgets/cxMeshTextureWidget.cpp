/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2016, SINTEF Department of Medical Technology
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
    this->clearUI();
}

void MeshTextureWidget::clearUI()
{
    this->setModified();
}

void MeshTextureWidget::setupUI()
{
    if (!mMesh)
        return;

    std::vector<PropertyPtr> properties = mMesh->getTextureData().mProperties;
    mOptionsWidget->setOptions("mesh_texture_properties", properties, true);

}

}//end namespace cx
