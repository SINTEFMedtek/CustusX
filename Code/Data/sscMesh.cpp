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
	Data(uid, name), mVtkPolyData(vtkPolyDataPtr::New())
{
	mColor = QColor(255, 0, 0, 255);
}
Mesh::Mesh(const QString& uid, const QString& name, const vtkPolyDataPtr& polyData) :
	Data(uid, name), mVtkPolyData(polyData)
{
	mColor = QColor(255, 0, 0, 255);
}
Mesh::~Mesh()
{
}
void Mesh::setVtkPolyData(const vtkPolyDataPtr& polyData)
{
	mVtkPolyData = polyData;
}
vtkPolyDataPtr Mesh::getVtkPolyData()
{
	return mVtkPolyData;
}
void Mesh::addXml(QDomNode& dataNode)
{
	Data::addXml(dataNode);
	QDomDocument doc = dataNode.ownerDocument();

	QDomNode meshNode = dataNode;
	//
	//  QDomDocument doc = parentNode.ownerDocument();
	//  QDomElement meshNode = doc.createElement("mesh");
	//  parentNode.appendChild(meshNode);

	//  m_rMd_History->addXml(meshNode); //TODO: should be in the superclass
	//
	//  QDomElement uidNode = doc.createElement("uid");
	//  uidNode.appendChild(doc.createTextNode(mUid.c_str()));
	//  meshNode.appendChild(uidNode);
	//
	//  QDomElement nameNode = doc.createElement("name");
	//  nameNode.appendChild(doc.createTextNode(mName.c_str()));
	//  meshNode.appendChild(nameNode);
	//
	//  QDomElement filePathNode = doc.createElement("filePath");
	//  filePathNode.appendChild(doc.createTextNode(mFilePath.c_str()));
	//  meshNode.appendChild(filePathNode);


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

DoubleBoundingBox3D Mesh::boundingBox() const
{
	mVtkPolyData->UpdateInformation();
	DoubleBoundingBox3D bounds(mVtkPolyData->GetBounds());
	return bounds;
}

vtkPolyDataPtr Mesh::getTransformedPolyData(ssc::Transform3D transform)
{
	// if transform elements exists, create a copy with entire position inside the polydata:
	if (ssc::similar(transform, ssc::Transform3D::Identity()))
		return mVtkPolyData;

	vtkPolyDataPtr poly = vtkPolyDataPtr::New();
	poly->DeepCopy(mVtkPolyData);
	vtkPointsPtr points = poly->GetPoints();

	for (int i = 0; i < poly->GetNumberOfPoints(); ++i)
	{
		ssc::Vector3D p(points->GetPoint(i));
		p = transform.coord(p);
		points->SetPoint(i, p.begin());
	}

	return poly;
}

} // namespace ssc
