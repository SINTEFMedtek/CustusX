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
#include <vtkColorSeries.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkPointData.h>
#include <QDomDocument>
#include <QColor>
#include <QDir>
#include <vtkImageReader2Factory.h>
#include <vtkTransformTextureCoords.h>
#include <vtkImageReader2.h>
#include <vtkTexture.h>
#include <vtkTextureMapToCylinder.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTextureMapToSphere.h>
#include "cxTypeConversions.h"
#include "cxRegistrationTransform.h"
#include "cxBoundingBox3D.h"
#include "cxDataReaderWriter.h"
#include "vtkProperty.h"

namespace cx
{




//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

MeshPtr Mesh::create(const QString& uid, const QString& name)
{
	return MeshPtr(new Mesh(uid, name));
}

Mesh::Mesh(const QString& uid, const QString& name, vtkPolyDataPtr polyData) :
    Data(uid, name), mVtkPolyData(polyData), mHasGlyph(false), mOrientationArray(""), mColorArray(""), mTextureType(""), mTextureFile("")
{
	if (!mVtkPolyData)
		mVtkPolyData = vtkPolyDataPtr::New();
    //vtktexture
	connect(&mProperties, &MeshPropertyData::changed, this, &Mesh::meshChanged);
	mShowGlyph = shouldGlyphBeEnableByDefault();
    mGlyphLUT ="Citrus";
    this->setAcquisitionTime(QDateTime::currentDateTime());
}

Mesh::~Mesh()
{
}

void Mesh::setIsWireframe(bool on)
{
	if (on)
		mProperties.mRepresentation->setValue(QString::number(VTK_WIREFRAME));
	else
		mProperties.mRepresentation->setValue(QString::number(VTK_SURFACE));
}

bool Mesh::getIsWireframe() const
{
	return mProperties.mRepresentation->getValue().toInt() == VTK_WIREFRAME;
}

bool Mesh::load(QString path)
{
	vtkPolyDataPtr raw;
	raw = DataReaderWriter().loadVtkPolyData(path);
	if(raw)
	{
		this->setVtkPolyData(raw);
		this->setName(QFileInfo(path).baseName());
		this->setFilename(path); // need path even when not set explicitly: nice for testing
	}
	return raw!=0;
}

void Mesh::setVtkPolyData(const vtkPolyDataPtr& polyData)
{
	mVtkPolyData = polyData;
	mOrientationArrayList.clear();
	mColorArrayList.clear();

	if (mVtkPolyData)
	{
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
	}
	mShowGlyph = shouldGlyphBeEnableByDefault();

	emit meshChanged();
}
vtkPolyDataPtr Mesh::getVtkPolyData() const
{
	return mVtkPolyData;
}

vtkTexturePtr Mesh::getVtkTexture() const
{
    return mVtkTexture;
}

void Mesh::addXml(QDomNode& dataNode)
{
	Data::addXml(dataNode);
	QDomDocument doc = dataNode.ownerDocument();

	QDomNode meshNode = dataNode;

	mProperties.addXml(dataNode);

    QDomElement glyphNode = doc.createElement("glyph");
    QDomElement elemGlyph = glyphNode.toElement();
    elemGlyph.setAttribute("showGlyph", mShowGlyph);
    elemGlyph.setAttribute("orientationArray", mOrientationArray.c_str());
    elemGlyph.setAttribute("colorArray", mColorArray.c_str());
    elemGlyph.setAttribute("glyphLUT", mGlyphLUT.c_str());
    meshNode.appendChild(elemGlyph);

}

void Mesh::parseXml(QDomNode& dataNode)
{
	Data::parseXml(dataNode);

	// image node must be parsed in the data manager to create this Image object
	// Only subnodes are parsed here

	if (dataNode.isNull())
		return;

	mProperties.parseXml(dataNode);

    QDomNode glyphNode = dataNode.namedItem("glyph");
    if (!glyphNode.isNull())
    {
        mShowGlyph = glyphNode.toElement().attribute("showGlyph").toInt();
        mOrientationArray = glyphNode.toElement().attribute("orientationArray").toStdString();
        mColorArray = glyphNode.toElement().attribute("colorArray").toStdString();
        mGlyphLUT = glyphNode.toElement().attribute("glyphLUT").toStdString();
    }

	emit meshChanged();
}

void Mesh::setColor(const QColor& color)
{
	mProperties.mColor->setValue(color);
}

QColor Mesh::getColor()
{
	return mProperties.mColor->getValue();
}

void Mesh::setBackfaceCullingSlot(bool backfaceCulling)
{
	mProperties.mBackfaceCulling->setValue(backfaceCulling);
}

bool Mesh::getBackfaceCulling()
{
	return mProperties.mBackfaceCulling->getValue();
}

void Mesh::setFrontfaceCullingSlot(bool frontfaceCulling)
{
	mProperties.mFrontfaceCulling->setValue(frontfaceCulling);
	emit meshChanged();
}

bool Mesh::getFrontfaceCulling()
{
	return mProperties.mFrontfaceCulling->getValue();
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
	if(!mVtkPolyData) return false;
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

double Mesh::getVisSize()
{
	return mProperties.mVisSize->getValue();
}

void Mesh::setVisSize(double size)
{
	mProperties.mVisSize->setValue(size);
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

const char * Mesh::getGlyphLUT()
{
    return mGlyphLUT.c_str();
}

const char *Mesh::getTextureType()
{
    return mTextureType.c_str();
}

const char *Mesh::getTextureFile()
{
    return mTextureFile.c_str();
}

void Mesh::setGlyphLUT(const char * glyphLUT)
{
    mGlyphLUT = glyphLUT;
    emit meshChanged();
}

void Mesh::setTextureType(const char *textureType)
{
    mTextureType = textureType;
    this->updateVtkPolyDataWithTexture();
    emit meshChanged();
}

void Mesh::setTextureFile(const char *textureFile)
{
    mTextureFile = textureFile;
    this->updateVtkPolyDataWithTexture();
    emit meshChanged();
}

void Mesh::updateVtkPolyDataWithTexture()
{
    if(mTextureFile.empty() || mTextureType.empty())
        return;

    //create texture coordinates
    vtkSmartPointer<vtkDataSetAlgorithm> tMapper;
    if (mTextureType == "Cylinder")
    {
        tMapper = vtkSmartPointer<vtkTextureMapToCylinder>::New();
        //vtkSmartPointer<vtkTextureMapToCylinder> tMapper = vtkSmartPointer<vtkTextureMapToCylinder>::New();
    }
    if (mTextureType == "Plane")
    {
        tMapper = vtkSmartPointer<vtkTextureMapToPlane>::New();
        //vtkSmartPointer<vtkTextureMapToCylinder> tMapper = vtkSmartPointer<vtkTextureMapToCylinder>::New();
    }
    if (mTextureType == "Sphere")
    {
        tMapper = vtkSmartPointer<vtkTextureMapToSphere>::New();
        //vtkSmartPointer<vtkTextureMapToCylinder> tMapper = vtkSmartPointer<vtkTextureMapToCylinder>::New();
    }
    else
    {
        tMapper = vtkSmartPointer<vtkTextureMapToCylinder>::New();
        //vtkSmartPointer<vtkTextureMapToCylinder> tMapper = vtkSmartPointer<vtkTextureMapToCylinder>::New();
    }
    //tMapper->SetInputConnection(mVtkPolyData->);
    tMapper->SetInputData(mVtkPolyData);
    //tMapper->PreventSeamOn();

    //read texture file
    vtkSmartPointer<vtkImageReader2Factory> readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkImageReader2 *imageReader = readerFactory->CreateImageReader2(mTextureFile.c_str());
    imageReader->SetFileName(mTextureFile.c_str());

    //create texture
    //vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
    mVtkTexture = vtkSmartPointer<vtkTexture>::New();
    mVtkTexture->SetInputConnection(imageReader->GetOutputPort());

    //transform texture coordinates
    double translate[3];
    translate[0] = 0.0;
    translate[1] = 0.0;
    translate[2] = 0.0;
    vtkSmartPointer<vtkTransformTextureCoords> transformTexture = vtkSmartPointer<vtkTransformTextureCoords>::New();
    transformTexture->SetInputConnection(tMapper->GetOutputPort());
    transformTexture->SetPosition(translate);
    transformTexture->SetScale(10,10,1);
    transformTexture->Update();

    //mVtkPolyData = transformTexture->GetOutputPort();
    mVtkPolyData = transformTexture->GetPolyDataOutput();
    //this->setVtkPolyData();

    imageReader->Delete();
}

QStringList Mesh::getOrientationArrayList()
{
    return mOrientationArrayList;
}

QStringList Mesh::getColorArrayList()
{
	return mColorArrayList;
}

const MeshPropertyData& Mesh::getProperties() const
{
	return mProperties;
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
