// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscMesh.h"

#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <QDomDocument>
#include <QColor>
#include "sscRep.h"
#include "sscTypeConversions.h"
#include "sscRegistrationTransform.h"
#include "sscBoundingBox3D.h"

namespace ssc
{
Mesh::Mesh(const QString& uid, const QString& name) :
	Data(uid, name), mVtkPolyData(vtkPolyDataPtr::New()), mWireframe(false), mBackfaceCulling(true)
{
	mColor = QColor(255, 0, 0, 255);
}
Mesh::Mesh(const QString& uid, const QString& name, const vtkPolyDataPtr& polyData) :
	Data(uid, name), mVtkPolyData(polyData), mWireframe(false), mBackfaceCulling(true)
{
	mColor = QColor(255, 0, 0, 255);
}
Mesh::~Mesh()
{
}

void Mesh::setIsWireframe(bool on)
{
	mWireframe = on;
	emit meshChanged();
}
bool Mesh::getIsWireframe() const
{
	return mWireframe;
}

void Mesh::setVtkPolyData(const vtkPolyDataPtr& polyData)
{
	mVtkPolyData = polyData;
	emit meshChanged();
}
vtkPolyDataPtr Mesh::getVtkPolyData() const
{
	return mVtkPolyData;
}
void Mesh::addXml(QDomNode& dataNode)
{
	Data::addXml(dataNode);
	QDomDocument doc = dataNode.ownerDocument();

	QDomNode meshNode = dataNode;

	QDomElement colorNode = doc.createElement("color");
	QDomElement subNode = doc.createElement("red");
	subNode.appendChild(doc.createTextNode(string_cast(mColor.red()).c_str()));
	colorNode.appendChild(subNode);
	subNode = doc.createElement("green");
	subNode.appendChild(doc.createTextNode(string_cast(mColor.green()).c_str()));
	colorNode.appendChild(subNode);
	subNode = doc.createElement("blue");
	subNode.appendChild(doc.createTextNode(string_cast(mColor.blue()).c_str()));
	colorNode.appendChild(subNode);
	subNode = doc.createElement("alpha");
	subNode.appendChild(doc.createTextNode(string_cast(mColor.alpha()).c_str()));
	colorNode.appendChild(subNode);
	meshNode.appendChild(colorNode);

	QDomElement cullingNode = doc.createElement("culling");
	QDomElement elem = cullingNode.toElement();
	elem.setAttribute("backfaceCulling", mBackfaceCulling);
	meshNode.appendChild(cullingNode);
}

void Mesh::parseXml(QDomNode& dataNode)
{
	Data::parseXml(dataNode);

	// image node must be parsed in the data manager to create this Image object
	// Only subnodes are parsed here

	if (dataNode.isNull())
		return;

	//  QDomNode registrationHistory = dataNode.namedItem("registrationHistory");
	//  m_rMd_History->parseXml(registrationHistory);

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

		mColor = QColor(red, green, blue, alpha);
	}

	QDomNode cullingNode = dataNode.namedItem("culling");
	if (!cullingNode.isNull())
		mBackfaceCulling = cullingNode.toElement().attribute("backfaceCulling").toInt();
}

void Mesh::setColor(const QColor& color)
{
	mColor = color;
	emit meshChanged();
}

QColor Mesh::getColor()
{
	return mColor;
}

void Mesh::setBackfaceCulling(bool backfaceCulling)
{
	mBackfaceCulling = backfaceCulling;
	emit meshChanged();
}

bool Mesh::getBackfaceCulling()
{
	return mBackfaceCulling;
}

DoubleBoundingBox3D Mesh::boundingBox() const
{
	getVtkPolyData()->Update();
	DoubleBoundingBox3D bounds(getVtkPolyData()->GetBounds());
	return bounds;
}

vtkPolyDataPtr Mesh::getTransformedPolyData(ssc::Transform3D transform)
{
	// if transform elements exists, create a copy with entire position inside the polydata:
	if (ssc::similar(transform, ssc::Transform3D::Identity()))
		return getVtkPolyData();

	getVtkPolyData()->Update();
	vtkPolyDataPtr poly = vtkPolyDataPtr::New();
	poly->DeepCopy(getVtkPolyData());
	vtkPointsPtr points = poly->GetPoints();

	for (int i = 0; i < poly->GetNumberOfPoints(); ++i)
	{
		ssc::Vector3D p(points->GetPoint(i));
		p = transform.coord(p);
		points->SetPoint(i, p.begin());
	}

	return poly;
}

bool Mesh::isFiberBundle() const
{
	vtkPolyDataPtr poly = getVtkPolyData();
	return poly->GetLines()->GetNumberOfCells() > 0 && poly->GetPolys()->GetNumberOfCells() == 0 && poly->GetStrips()->GetNumberOfCells() == 0;
}

} // namespace ssc
