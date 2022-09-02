/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxMesh.h"
#include <vtkCellArray.h>
#include <vtkColorSeries.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <QDomDocument>
#include <QColor>
#include <QDir>
#include <vtkTransformTextureCoords.h>
#include <vtkTexture.h>
#include <vtkTextureMapToCylinder.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTextureMapToSphere.h>
#include "cxTypeConversions.h"
#include "cxRegistrationTransform.h"
#include "cxBoundingBox3D.h"
#include "vtkProperty.h"
#include "vtkImageData.h"
#include "cxImage.h"
#include "cxFileManagerService.h"
#include "cxLogger.h"
#include "cxNullDeleter.h"

namespace cx
{


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

MeshPtr Mesh::create(const QString& uid, const QString& name, PatientModelServicePtr patientModelService, SpaceProviderPtr spaceProvider)
{
	return MeshPtr(new Mesh(uid, name, vtkPolyDataPtr(), patientModelService, spaceProvider));
}

Mesh::Mesh(const QString& uid, const QString& name, vtkPolyDataPtr polyData, PatientModelServicePtr patientModelService, SpaceProviderPtr spaceProvider) :
	Data(uid, name), mVtkPolyData(polyData), mHasGlyph(false), mOrientationArray(""), mColorArray(""), mPatientModelService(patientModelService),
	mSpaceProvider(spaceProvider), mTextureData(patientModelService)
{
	if (!mVtkPolyData)
		mVtkPolyData = vtkPolyDataPtr::New();
	connect(&mProperties, &MeshPropertyData::changed, this, &Mesh::meshChanged);
	connect(&mTextureData, &MeshTextureData::changed, this, &Mesh::meshChanged);
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

bool Mesh::load(QString path, FileManagerServicePtr filemanager)
{
	vtkPolyDataPtr raw;
	raw = filemanager->loadVtkPolyData(path);
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
	mVtkPolyDataOriginal = mVtkPolyData;
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
	mTextureData.addXml(dataNode);

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
	mTextureData.parseXml(dataNode);

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

void Mesh::setUseColorFromPolydataScalars(bool on)
{
	mProperties.mUseColorFromPolydataScalars->setValue(on);
}

bool Mesh::getUseColorFromPolydataScalars() const
{
    return mProperties.mUseColorFromPolydataScalars->getValue();
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

QString Mesh::getTextureShape()
{
	return mTextureData.getTextureShape()->getValue();
}

void Mesh::setGlyphLUT(const char * glyphLUT)
{
	mGlyphLUT = glyphLUT;
	emit meshChanged();
}

bool Mesh::hasTexture() const
{
	if(mTextureData.getTextureImage()->getValue().isEmpty() || mTextureData.getTextureImage()->getImage() == NULL)
		return false;
	else
		return true;
}

void Mesh::updateVtkPolyDataWithTexture()
{
	if (!this->hasTexture())
	{
		mVtkTexture = NULL;
		if(mVtkPolyDataOriginal)
		  mVtkPolyData = mVtkPolyDataOriginal;
		return;
	}

	//create the texture mapper
	vtkDataSetAlgorithmPtr tMapper;
	if(!this->createTextureMapper(tMapper))
		return;

	//Get the image data
	ImagePtr textureImage = mTextureData.getTextureImage()->getImage();
	vtkImageDataPtr vtkImageData = textureImage->getBaseVtkImageData();

	//Create the texture
	mVtkTexture = vtkTexturePtr::New();
	mVtkTexture->SetRepeat(mTextureData.getRepeat()->getValue());
	mVtkTexture->SetInputData(vtkImageData);

	//transform texture coordinates
	//VTK uses r, s and t coordinates. t is only used for 3D texturing which is not supported by VTK yet.
	//We map r and s to match the CustusX X and Y directions.
	vtkTransformTextureCoordsPtr transformTexture = vtkTransformTextureCoordsPtr::New();
	transformTexture->SetInputConnection(tMapper->GetOutputPort());
	double posR = this->getTextureData().getPositionX()->getValue();
	double posS = this->getTextureData().getPositionY()->getValue();
	transformTexture->SetPosition(-posR, -posS, 0);
	double scaleR = this->getTextureData().getScaleX()->getValue();
	double scaleS = this->getTextureData().getScaleY()->getValue();
	transformTexture->SetScale(scaleR, scaleS, 1);
	transformTexture->Update();

	//Update the poly data
	mVtkPolyData = transformTexture->GetPolyDataOutput();
}

bool Mesh::createTextureMapper(vtkDataSetAlgorithmPtr &tMapper)
{
	QString textureShape = this->getTextureShape();

	if (textureShape == mTextureData.getCylinderText())
	{
		tMapper = vtkTextureMapToCylinderPtr::New();
		dynamic_cast<vtkTextureMapToCylinder*>(tMapper.Get())->PreventSeamOn();
	}
	else if (textureShape == mTextureData.getPlaneText())
	{
		vtkTextureMapToPlanePtr mapper = vtkTextureMapToPlanePtr::New();
		DoubleBoundingBox3D bb = this->boundingBox();
		// Explicitly state the plane as the upper xy-plane of the bounding box
		// The automatic plane generation is not deterministic for  square cases.
		mapper->SetOrigin(bb.corner(0,0,1).data());
		mapper->SetPoint1(bb.corner(1,0,1).data());
		mapper->SetPoint2(bb.corner(0,1,1).data());

		tMapper = mapper;
	}
	else if (textureShape == mTextureData.getSphereText())
	{
		tMapper = vtkTextureMapToSpherePtr::New();
	}
	else
	{
		return false;
	}

	tMapper->SetInputData(mVtkPolyData);
	return true;
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

const MeshTextureData &Mesh::getTextureData() const
{
	return mTextureData;
}

DoubleBoundingBox3D Mesh::boundingBox() const
{
	//	getVtkPolyData()->Update();
	DoubleBoundingBox3D bounds(getVtkPolyData()->GetBounds());
	return bounds;
}

vtkPolyDataPtr Mesh::getTransformedPolyDataCopy(Transform3D transform)
{
	// if transform elements exists, create a copy with entire position inside the polydata:
    if (similar(transform, Transform3D::Identity()))
    {
        vtkPolyDataPtr poly = vtkPolyDataPtr::New();
        poly->DeepCopy(getVtkPolyData());
        return poly;
    }

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

	return poly;
}

bool Mesh::isFiberBundle() const
{
	vtkPolyDataPtr poly = getVtkPolyData();
	return poly->GetLines()->GetNumberOfCells() > 0 && poly->GetPolys()->GetNumberOfCells() == 0 && poly->GetStrips()->GetNumberOfCells() == 0;
}

void Mesh::save(const QString& basePath, FileManagerServicePtr fileManager)
{
	QString filename = basePath + "/Images/" + this->getUid() + ".vtk";
	this->setFilename(QDir(basePath).relativeFilePath(filename));
	MeshPtr self = MeshPtr(this, null_deleter());
	fileManager->save(self, filename);

}

} // namespace cx
