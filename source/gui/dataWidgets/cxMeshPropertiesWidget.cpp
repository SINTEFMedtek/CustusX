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
#include "cxBoolProperty.h"

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
	BaseWidget(parent, "mesh_properties_widget", "Properties"),
	mPatientModelService(patientModelService),
	mViewService(viewService),
	mMeshSelector(meshSelector)
{
	mLayout = new QVBoxLayout(this);

	connect(mMeshSelector.get(), &Property::changed, this, &MeshPropertiesWidget::meshSelectedSlot);
	this->setModified();
}

MeshPropertiesWidget::~MeshPropertiesWidget()
{
}

void MeshPropertiesWidget::prePaintEvent()
{
	this->setupUI();
	this->updateFrontend();
}

void MeshPropertiesWidget::updateFrontend()
{
	MeshPropertyData data;
	if (mMesh)
		data = mMesh->getProperties();

	mVisSize->setValue(data.mVisSize);
	mColor->setValue(data.mColor);
	mBackfaceCulling->setValue(data.mBackfaceCulling);
	mFrontfaceCulling->setValue(data.mFrontfaceCulling);
}

void MeshPropertiesWidget::onGuiChanged()
{
	if (!mMesh)
		return;

	MeshPropertyData data = mMesh->getProperties();

	data.mBackfaceCulling = mBackfaceCulling->getValue();
	data.mFrontfaceCulling = mFrontfaceCulling->getValue();
	data.mVisSize = mVisSize->getValue();
	data.mColor = mColor->getValue();

	mMesh->setProperties(data);
}

template <class T, class SOURCE_SIGNAL, class TARGET, class TARGET_SIGNAL>
bool cxqreconnect(boost::shared_ptr<T> old_source, boost::shared_ptr<T> new_source,
								  SOURCE_SIGNAL source_signal,
								  TARGET* target,
								  TARGET_SIGNAL target_signal)
{
	if (old_source == new_source)
		return false;

	if(old_source)
	{
		target->disconnect(old_source.get(), source_signal, target, target_signal);
	}
	if(new_source)
	{
		target->connect(new_source.get(), source_signal, target, target_signal);
	}

	return true;
}

template<class T>
class cxqConnector
{
public:
	boost::shared_ptr<T> object;

	template <class SOURCE_SIGNAL, class TARGET, class TARGET_SIGNAL>
	bool reconnect(boost::shared_ptr<T> new_source,
									  SOURCE_SIGNAL source_signal,
									  TARGET* target,
									  TARGET_SIGNAL target_signal)
	{
		if (!cxqreconnect(object,  new_source, source_signal, target, target_signal))
			return false;
		object = new_source;
		return true;
	}
};

void MeshPropertiesWidget::meshSelectedSlot()
{
	if (mMesh == mMeshSelector->getData())
		return;

	MeshPtr newmesh = boost::dynamic_pointer_cast<Mesh>(mMeshSelector->getData());

	cxqreconnect(mMesh,  newmesh, &Mesh::meshChanged, this, &MeshPropertiesWidget::setModified);
	mMesh = newmesh;
	this->setModified();
}

void MeshPropertiesWidget::setupUI()
{
	if (mLayout->count()) // already created
		return;

	mPropertiesLayout = new QGridLayout;
	mPropertiesLayout->setMargin(0);
	mLayout->addLayout(mPropertiesLayout);

	//-------------------------------------------------------------------------
	mColor = ColorProperty::initialize("Color", "", "Mesh color", QColor("red"));//, options.getElement());
	this->addProperty(mColor);
	//-------------------------------------------------------------------------
	mVisSize = DoubleProperty::initialize("visSize", "Point size", "Visualized size of points, glyphs etc.",1, DoubleRange(1, 20, 1), 0);
	mVisSize->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	this->addProperty(mVisSize);
	//-------------------------------------------------------------------------
	mBackfaceCulling = BoolProperty::initialize("Backface culling", "",
									   "Set backface culling on. This makes transparent meshes work, "
									   "but only draws outside mesh walls "
									   "(eg. navigating inside meshes will not work).",
									   true);
	this->addProperty(mBackfaceCulling);
	//-------------------------------------------------------------------------
	mFrontfaceCulling = BoolProperty::initialize("Frontface culling", "",
									   "Set frontface culling on. Can be used to make transparent "
									   "meshes work from inside the meshes.",
									   true);
	this->addProperty(mFrontfaceCulling);
	//-------------------------------------------------------------------------

	mLayout->addStretch();
}

void MeshPropertiesWidget::addProperty(PropertyPtr property)
{
	mProperties.push_back(property);
	connect(property.get(), &Property::changed, this, &MeshPropertiesWidget::onGuiChanged);
	createDataWidget(mViewService, mPatientModelService, this, property, mPropertiesLayout, mProperties.size()-1);
}

}//end namespace cx
