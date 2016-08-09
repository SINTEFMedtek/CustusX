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

namespace cx
{

MeshPropertyData::MeshPropertyData()
//	mWireframe(false),
//	mBackfaceCulling(false),
//	mFrontfaceCulling(false),
//	mVisSize(2.0),
//	mColor(255, 0, 0, 255),
//	mWireframeRepresentation(false),
//	mPointsRepresentation(false),
//	mEdgeVisibility(false),
//	mEdgeColor("white"),
//	mShading(false),
//	mAmbient(0.2),
//	mDiffuse(0.9),
//	mSpecular(0.3),
//	mSpecularPower(15)
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
	mVisSize = DoubleProperty::initialize("visSize", "Point size",
										  "Visualized size of points, glyphs etc.",
										  1, DoubleRange(1, 20, 1), 0);
	mVisSize->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	this->addProperty(mVisSize);
	//-------------------------------------------------------------------------
	mBackfaceCulling = BoolProperty::initialize("backfaceCulling", "Backface culling",
									   "Set backface culling on. This makes transparent meshes work, "
									   "but only draws outside mesh walls "
									   "(eg. navigating inside meshes will not work).",
									   true);
	this->addProperty(mBackfaceCulling);
	//-------------------------------------------------------------------------
	mFrontfaceCulling = BoolProperty::initialize("frontfaceCulling", "Frontface culling",
									   "Set frontface culling on. Can be used to make transparent "
									   "meshes work from inside the meshes.",
									   true);
	this->addProperty(mFrontfaceCulling);
	//-------------------------------------------------------------------------
	mWireframeRepresentation = BoolProperty::initialize("wireframeRepresentation", "Wireframe",
														"Show model as wireframe",
														false);
	this->addProperty(mWireframeRepresentation);
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
		CX_LOG_CHANNEL_DEBUG("CA") << "reading " << mProperties[i]->getUid()
								   << " , old=" << mProperties[i]->getValueAsVariant().value<QColor>().name()
								   << " newval=" << item.readVariant().value<QColor>().name();
		QVariant orgval = mProperties[i]->getValueAsVariant();
		mProperties[i]->setValueFromVariant(item.readVariant(orgval));
	}
}

} // namespace cx
