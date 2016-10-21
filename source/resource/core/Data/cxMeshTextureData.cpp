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


#include "cxMeshTextureData.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"

namespace cx
{
MeshTextureData::MeshTextureData(PatientModelServicePtr patientModelService)
	: mPatientModelService(patientModelService)
	, mCylinderText("Cylinder")
	, mPlaneText("Plane")
	, mSphereText("Sphere")
{
	this->initialize();
}

void MeshTextureData::addXml(QDomNode &dataNode)
{
	for (unsigned i=0; i<mProperties.size(); ++i)
	{
		XmlOptionItem item(mProperties[i]->getUid(), dataNode.toElement());
		item.writeVariant(mProperties[i]->getValueAsVariant());
	}
}

void MeshTextureData::parseXml(QDomNode &dataNode)
{
	for (unsigned i=0; i<mProperties.size(); ++i)
	{
		XmlOptionItem item(mProperties[i]->getUid(), dataNode.toElement());
		QVariant orgval = mProperties[i]->getValueAsVariant();
		mProperties[i]->setValueFromVariant(item.readVariant(orgval));
	}
}

void MeshTextureData::addProperty(PropertyPtr property)
{
	mProperties.push_back(property);
	connect(property.get(), &Property::changed, this, &MeshTextureData::changed);
}

void MeshTextureData::initialize()
{
	mTextureImage = StringPropertySelectImage::New(mPatientModelService);
	mTextureImage->setValueName("Texture image");
	mTextureImage->setHelp("Select an imported 2D image to use as texture.");
	mTextureImage->setOnly2DImagesFilter(true);
	this->addProperty(mTextureImage);

	mTextureShape = StringProperty::initialize("texture_type", "Texture shape",
											   "The texture is applied in predefined geometric shapes. Select the shape which gives the best fit to your mesh.",
											   "Cylinder",
											   QStringList()
											   << this->getCylinderText()
											   << this->getPlaneText()
											   << this->getSphereText());
	this->addProperty(mTextureShape);

	mScaleX = DoubleProperty::initialize("texture_scale_X", "Scale X", "How many times to draw the image along the X axis.", 1, DoubleRange(1,100000,1), 0);
	this->addProperty(mScaleX);

	mScaleY = DoubleProperty::initialize("texture_scale_Y", "Scale Y", "How many times to draw the image along the Y axis.", 1, DoubleRange(1,100000,1), 0);
	this->addProperty(mScaleY);

	mPositionX = DoubleProperty::initialize("texture_position_X", "Position X", "Where to start drawing the image along the X axis.", 0, DoubleRange(-100000,100000,0.01), 2);
	this->addProperty(mPositionX);

	mPositionY = DoubleProperty::initialize("texture_position_Y", "Position Y", "Where to start drawing the image along the Y axis.", 0, DoubleRange(-100000,100000,0.01), 2);
	this->addProperty(mPositionY);

	mRepeat = BoolProperty::initialize("repeat", "Repeat", "Repeat texture over surface", true);
	this->addProperty(mRepeat);
	mEdgeClamp = BoolProperty::initialize("edge_clamp", "Edge Clamp", "Clamp texture at edges when repeat is off", false);
	this->addProperty(mEdgeClamp);
}

QString MeshTextureData::getSphereText() const
{
	return mSphereText;
}

QString MeshTextureData::getPlaneText() const
{
	return mPlaneText;
}

QString MeshTextureData::getCylinderText() const
{
	return mCylinderText;
}

DoublePropertyPtr MeshTextureData::getPositionY() const
{
	return mPositionY;
}

DoublePropertyPtr MeshTextureData::getPositionX() const
{
	return mPositionX;
}

BoolPropertyPtr MeshTextureData::getRepeat() const
{
	return mRepeat;
}
BoolPropertyPtr MeshTextureData::getEdgeClamp() const
{
	return mEdgeClamp;
}

std::vector<PropertyPtr> MeshTextureData::getProperties() const
{
	return mProperties;
}

DoublePropertyPtr MeshTextureData::getScaleY() const
{
	return mScaleY;
}

DoublePropertyPtr MeshTextureData::getScaleX() const
{
	return mScaleX;
}

StringPropertySelectImagePtr MeshTextureData::getTextureImage() const
{
	return mTextureImage;
}

StringPropertyPtr MeshTextureData::getTextureShape() const
{
	return mTextureShape;
}

} // namespace cx
