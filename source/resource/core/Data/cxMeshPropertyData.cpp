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

namespace cx
{

MeshPropertyData::MeshPropertyData() :
	mWireframe(false),
	mBackfaceCulling(false),
	mFrontfaceCulling(false),
	mVisSize(2.0),
	mColor(255, 0, 0, 255),
	mWireframeRepresentation(false),
	mPointsRepresentation(false),
	mEdgeVisibility(false),
	mEdgeColor("white"),
	mShading(false),
	mAmbient(0.2),
	mDiffuse(0.9),
	mSpecular(0.3),
	mSpecularPower(15)
{

}

void MeshPropertyData::addColorToXml(QDomNode &node, QColor color)
{
	QDomDocument doc = node.ownerDocument();
	QDomElement elem = node.toElement();

	QDomElement colorNode = doc.createElement("color");
	QDomElement subNode = doc.createElement("red");
	subNode.appendChild(doc.createTextNode(string_cast(color.red()).c_str()));
	colorNode.appendChild(subNode);
	subNode = doc.createElement("green");
	subNode.appendChild(doc.createTextNode(string_cast(color.green()).c_str()));
	colorNode.appendChild(subNode);
	subNode = doc.createElement("blue");
	subNode.appendChild(doc.createTextNode(string_cast(color.blue()).c_str()));
	colorNode.appendChild(subNode);
	subNode = doc.createElement("alpha");
	subNode.appendChild(doc.createTextNode(string_cast(color.alpha()).c_str()));
	colorNode.appendChild(subNode);
	elem.appendChild(colorNode);
}

QColor MeshPropertyData::parseColorFromXml(QDomNode dataNode, QColor defval)
{
	QDomNode colorNode = dataNode.namedItem("color");
	if (!colorNode.isNull())
	{
		int red = 255;
		int green = 255;
		int blue = 255;
		int alpha = 255;

		QDomNode node = colorNode.namedItem("red");
		if (!node.isNull())
			red = node.toElement().text().toInt();

		node = colorNode.namedItem("green");
		if (!node.isNull())
			green = node.toElement().text().toInt();

		node = colorNode.namedItem("blue");
		if (!node.isNull())
			blue = node.toElement().text().toInt();

		node = colorNode.namedItem("alpha");
		if (!node.isNull())
			alpha = node.toElement().text().toInt();

		return QColor(red, green, blue, alpha);
	}
	return defval;
}


void MeshPropertyData::addXml(QDomNode &dataNode)
{
	QDomDocument doc = dataNode.ownerDocument();
	QDomElement elem = dataNode.toElement();

	this->addColorToXml(dataNode, mColor);

	QDomElement cullingNode = doc.createElement("culling").toElement();
	cullingNode.setAttribute("backfaceCulling", mBackfaceCulling);
	cullingNode.setAttribute("frontfaceCulling", mFrontfaceCulling);
	elem.appendChild(cullingNode);

	elem.setAttribute("visSize", mVisSize);

//	bool mWireframeRepresentation;
//	bool mPointsRepresentation;
//	bool mEdgeVisibility;
//	QColor mEdgeColor;
//	bool mShading;
//	double mAmbient;
//	double mDiffuse;
//	double mSpecular;
//	double mSpecularPower;
//	elem.setAttribute("wireframeRepresentation", mWireframeRepresentation);
//	elem.setAttribute("pointsRepresentation", mPointsRepresentation);
//	elem.setAttribute("mEdgeVisibility", mEdgeVisibility);
//	elem.setAttribute("mEdgeColor", mEdgeColor);
//	elem.setAttribute("mShading", mShading);
//	elem.setAttribute("mAmbient", mAmbient);
//	elem.setAttribute("mDiffuse", mDiffuse);
//	elem.setAttribute("mSpecular", mSpecular);
//	elem.setAttribute("mSpecularPower", mSpecularPower);
}

void MeshPropertyData::parseXml(QDomNode dataNode)
{
	mColor = this->parseColorFromXml(dataNode, mColor);
	QDomElement elem = dataNode.toElement();

	QDomElement cullingNode = dataNode.namedItem("culling").toElement();
	if (!cullingNode.isNull())
	{
		mBackfaceCulling = cullingNode.attribute("backfaceCulling", QString::number(mBackfaceCulling)).toInt();
		mFrontfaceCulling = cullingNode.attribute("frontfaceCulling", QString::number(mFrontfaceCulling)).toInt();
	}

	mVisSize =  elem.attribute("visSize", QString::number(mVisSize)).toDouble();


//	QDomElement elem = dataNode.toElement();
//	mCameraFollowTool = elem.attribute("cameraFollowTool", QString::number(mCameraFollowTool)).toInt();
//	mFocusFollowTool = elem.attribute("focusFollowTool", QString::number(mFocusFollowTool)).toInt();
//	mCameraOnTooltip = elem.attribute("cameraOnTooltip", QString::number(mCameraOnTooltip)).toInt();
//	mTableLock = elem.attribute("tableLock", QString::number(mTableLock)).toInt();
//	mElevation = elem.attribute("elevation", QString::number(mElevation)).toDouble();
//	mUniCam = elem.attribute("uniCam", QString::number(mUniCam)).toInt();
//	mAutoZoomROI = elem.attribute("autoZoomROI", mAutoZoomROI);
//	mFocusROI = elem.attribute("focusROI", mFocusROI);
}

} // namespace cx
