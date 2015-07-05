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


#include "cxMesh.h"

#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkPointData.h>
#include <QDomDocument>
#include <QColor>
#include <QDir>
#include "cxTypeConversions.h"
#include "cxRegistrationTransform.h"
#include "cxBoundingBox3D.h"
#include "cxDataReaderWriter.h"

namespace cx
{

MeshPtr Mesh::create(const QString& uid, const QString& name)
{
	return MeshPtr(new Mesh(uid, name));
}

Mesh::Mesh(const QString& uid, const QString& name) :
    Data(uid, name), mVtkPolyData(vtkPolyDataPtr::New()), mWireframe(false), mBackfaceCulling(false), mFrontfaceCulling(false),mHasGlyph(false), mOrientationArray(""), mColorArray("")
{
	mColor = QColor(255, 0, 0, 255);
    mShowGlyph = shouldGlyphBeEnableByDefault();
	this->setAcquisitionTime(QDateTime::currentDateTime());
}
Mesh::Mesh(const QString& uid, const QString& name, const vtkPolyDataPtr& polyData) :
    Data(uid, name), mVtkPolyData(polyData), mWireframe(false), mBackfaceCulling(false), mFrontfaceCulling(false),mHasGlyph(false), mOrientationArray(""), mColorArray("")
{
	mColor = QColor(255, 0, 0, 255);
    mShowGlyph = shouldGlyphBeEnableByDefault();
    this->setAcquisitionTime(QDateTime::currentDateTime());
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

bool Mesh::load(QString path)
{
	vtkPolyDataPtr raw;
	raw = DataReaderWriter().loadVtkPolyData(path);
	this->setVtkPolyData(raw);
	if(raw)
	{
		this->setName(QFileInfo(path).baseName());
		this->setFilename(path); // need path even when not set explicitly: nice for testing
	}
	return raw!=0;
}

void Mesh::setVtkPolyData(const vtkPolyDataPtr& polyData)
{
	mVtkPolyData = polyData;

    int num;
    for(int k=0; k <  mVtkPolyData->GetPointData()->GetNumberOfArrays(); k++)
    {
        num=mVtkPolyData->GetPointData()->GetArray(k)->GetNumberOfComponents();
        if(num==3)
        {
            if(strlen(mOrientationArray.c_str())==0)
            {
                mOrientationArray=mVtkPolyData->GetPointData()->GetArrayName(k);
                mHasGlyph=true;
            }
            mOrientationArrayList << mVtkPolyData->GetPointData()->GetArrayName(k);
        }
    }
    mColorArrayList << "";
    mColorArray="";
    for(int k=0; k <  mVtkPolyData->GetPointData()->GetNumberOfArrays(); k++)
    {
        num=mVtkPolyData->GetPointData()->GetArray(k)->GetNumberOfComponents();
        if(num==1)
        {
            mColorArrayList << mVtkPolyData->GetPointData()->GetArrayName(k);
        }
    }

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
    elem.setAttribute("frontfaceCulling", mFrontfaceCulling);
	meshNode.appendChild(cullingNode);

    QDomElement glyphNode = doc.createElement("glyph");
    QDomElement elemGlyph = glyphNode.toElement();
    elemGlyph.setAttribute("showGlyph", mShowGlyph);
    elemGlyph.setAttribute("orientationArray", mOrientationArray.c_str());
    elemGlyph.setAttribute("colorArray", mColorArray.c_str());
    meshNode.appendChild(elemGlyph);

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
    {
        mBackfaceCulling = cullingNode.toElement().attribute("backfaceCulling").toInt();
        mFrontfaceCulling = cullingNode.toElement().attribute("frontfaceCulling").toInt();
    }

    QDomNode glyphNode = dataNode.namedItem("glyph");
    if (!glyphNode.isNull())
    {
        mShowGlyph = glyphNode.toElement().attribute("showGlyph").toInt();
        mOrientationArray = glyphNode.toElement().attribute("orientationArray").toStdString();
        mColorArray = glyphNode.toElement().attribute("colorArray").toStdString();
    }

	emit meshChanged();
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

void Mesh::setBackfaceCullingSlot(bool backfaceCulling)
{
	mBackfaceCulling = backfaceCulling;
	emit meshChanged();
}

bool Mesh::getBackfaceCulling()
{
	return mBackfaceCulling;
}

void Mesh::setFrontfaceCullingSlot(bool frontfaceCulling)
{
	mFrontfaceCulling = frontfaceCulling;
	emit meshChanged();
}

bool Mesh::getFrontfaceCulling()
{
	return mFrontfaceCulling;
}

void Mesh::setShowGlyph(bool val)
{
    mShowGlyph = val;
    emit meshChanged();
}

bool Mesh::hasGlyph()
{
    return mHasGlyph;
}

bool Mesh::showGlyph()
{
    return mShowGlyph;
}

bool Mesh::shouldGlyphBeEnableByDefault()
{
    if(! mHasGlyph) return false;
    if(mVtkPolyData->GetNumberOfVerts() > 0) return false;
    if(mVtkPolyData->GetNumberOfLines() > 0) return false;
    if(mVtkPolyData->GetNumberOfPolys() > 0) return false;
    if(mVtkPolyData->GetNumberOfStrips() > 0) return false;

    return true;
}


const char * Mesh::getOrientationArray()
{
    return mOrientationArray.c_str();
}

void Mesh::setOrientationArray(const char * orientationArray)
{
    mOrientationArray = orientationArray;
    emit meshChanged();
}

const char * Mesh::getColorArray()
{
    return mColorArray.c_str();
}

void Mesh::setColorArray(const char * colorArray)
{
    mColorArray = colorArray;
    emit meshChanged();
}

QStringList Mesh::getOrientationArrayList()
{
    return mOrientationArrayList;
}

QStringList Mesh::getColorArrayList()
{
    return mColorArrayList;
}



DoubleBoundingBox3D Mesh::boundingBox() const
{
//	getVtkPolyData()->Update();
	DoubleBoundingBox3D bounds(getVtkPolyData()->GetBounds());
	return bounds;
}

vtkPolyDataPtr Mesh::getTransformedPolyData(Transform3D transform)
{
	// if transform elements exists, create a copy with entire position inside the polydata:
	if (similar(transform, Transform3D::Identity()))
		return getVtkPolyData();

//	getVtkPolyData()->Update();
	vtkPolyDataPtr poly = vtkPolyDataPtr::New();
	poly->DeepCopy(getVtkPolyData());
	vtkPointsPtr points = poly->GetPoints();

	vtkPointsPtr floatPoints = vtkPointsPtr::New();
	floatPoints->DeepCopy(points);
	floatPoints->SetDataTypeToFloat();
	for (int i = 0; i < poly->GetNumberOfPoints(); ++i)
	{
		Vector3D p(points->GetPoint(i));
		p = transform.coord(p);
		floatPoints->SetPoint(i, p.begin());
	}
	poly->SetPoints(floatPoints.GetPointer());
	poly->Modified();
//	poly->Update();

	return poly;
}

bool Mesh::isFiberBundle() const
{
	vtkPolyDataPtr poly = getVtkPolyData();
	return poly->GetLines()->GetNumberOfCells() > 0 && poly->GetPolys()->GetNumberOfCells() == 0 && poly->GetStrips()->GetNumberOfCells() == 0;
}

void Mesh::save(const QString& basePath)
{
	vtkPolyDataWriterPtr writer = vtkPolyDataWriterPtr::New();
	writer->SetInputData(this->getVtkPolyData());
	QString filename = basePath + "/Images/" + this->getUid() + ".vtk";
	this->setFilename(QDir(basePath).relativeFilePath(filename));
	writer->SetFileName(cstring_cast(filename));

	writer->Update();
	writer->Write();
}

} // namespace cx
