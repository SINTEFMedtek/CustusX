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
#include "cxMeshPropertyData.h"

#include <QDomDocument>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "vtkProperty.h"

namespace cx
{

MeshPropertyData::MeshPropertyData()
{
	this->initialize();
}

void MeshPropertyData::initialize()
{
	//-------------------------------------------------------------------------
	mColor = ColorProperty::initialize("Color", "",
									   "Mesh color",
									   QColor("red"));
	this->addProperty(mColor);

	//-------------------------------------------------------------------------
	mUseColorFromPolydataScalars = BoolProperty::initialize( "colorFromPolydataScalars", "Color from polydata scalars",
									   "If your polydata has a scalar array with color data in you can use that to color the mesh.",
									   false);
	this->addProperty(mUseColorFromPolydataScalars);
	//-------------------------------------------------------------------------
	mVisSize = DoubleProperty::initialize("visSize", "Point size",
										  "Visualized size of points, glyphs etc.",
										  2, DoubleRange(1, 20, 1), 0);
	mVisSize->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	this->addProperty(mVisSize);
	//-------------------------------------------------------------------------
	mBackfaceCulling = BoolProperty::initialize("backfaceCulling", "Backface culling",
									   "Set backface culling on. This makes transparent meshes work, "
									   "but only draws outside mesh walls "
									   "(eg. navigating inside meshes will not work).",
									   false);
	this->addProperty(mBackfaceCulling);
	//-------------------------------------------------------------------------
	mFrontfaceCulling = BoolProperty::initialize("frontfaceCulling", "Frontface culling",
									   "Set frontface culling on. Can be used to make transparent "
									   "meshes work from inside the meshes.",
									   false);
	this->addProperty(mFrontfaceCulling);
	//-------------------------------------------------------------------------
	mRepresentation = StringProperty::initialize("representation", "Representation",
												 "How to represent model visually",
												 QString::number(VTK_SURFACE),
												 QStringList()
												 << QString::number(VTK_SURFACE)
												 << QString::number(VTK_WIREFRAME)
												 << QString::number(VTK_POINTS));
	std::map<QString,QString> representationNames;
	representationNames[QString::number(VTK_SURFACE)] = "Surface";
	representationNames[QString::number(VTK_WIREFRAME)] = "Wireframe";
	representationNames[QString::number(VTK_POINTS)] = "Points";
	mRepresentation->setDisplayNames(representationNames);
	this->addProperty(mRepresentation);
	//-------------------------------------------------------------------------
	mEdgeVisibility = BoolProperty::initialize("edgeVisibility", "Show Edges",
														"Show model edges",
														false);
	this->addProperty(mEdgeVisibility);
	//-------------------------------------------------------------------------
	mEdgeColor = ColorProperty::initialize("edgeColor", "Edge color",
										   "Edge color, used when edges are visible.",
										   QColor("green"));
	this->addProperty(mEdgeColor);
	//-------------------------------------------------------------------------
	mAmbient = DoubleProperty::initialize("ambient", "Ambient",
										  "Ambient color coefficient",
										  0.2, DoubleRange(0, 1, 0.05), 2);
	mAmbient->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	this->addProperty(mAmbient);
	//-------------------------------------------------------------------------
	mDiffuse = DoubleProperty::initialize("diffuse", "Diffuse",
										  "Diffuse color coefficient",
										  0.9, DoubleRange(0, 1, 0.05), 2);
	mDiffuse->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	this->addProperty(mDiffuse);
	//-------------------------------------------------------------------------
	mSpecular = DoubleProperty::initialize("specular", "Specular",
										  "Specular color coefficient",
										  0.3, DoubleRange(0, 1, 0.05), 2);
	mSpecular->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	this->addProperty(mSpecular);
	//-------------------------------------------------------------------------
	mSpecularPower = DoubleProperty::initialize("specularPower", "Specular Power",
										  "Specular color power",
										  15, DoubleRange(1, 30, 1), 0);
	mSpecularPower->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	this->addProperty(mSpecularPower);
	//-------------------------------------------------------------------------
}

void MeshPropertyData::addProperty(PropertyPtr property)
{
	mProperties.push_back(property);
	connect(property.get(), &Property::changed, this, &MeshPropertyData::changed);
}

void MeshPropertyData::addXml(QDomNode &dataNode)
{
	for (unsigned i=0; i<mProperties.size(); ++i)
	{
		XmlOptionItem item(mProperties[i]->getUid(), dataNode.toElement());
		item.writeVariant(mProperties[i]->getValueAsVariant());
	}
}

void MeshPropertyData::parseXml(QDomNode dataNode)
{
	for (unsigned i=0; i<mProperties.size(); ++i)
	{
		XmlOptionItem item(mProperties[i]->getUid(), dataNode.toElement());
		QVariant orgval = mProperties[i]->getValueAsVariant();
		mProperties[i]->setValueFromVariant(item.readVariant(orgval));
	}
}

} // namespace cx
