/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxGeometricRep.h"

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkArrowSource.h>
#include <vtkPlane.h>

#include "cxMesh.h"
#include "cxView.h"

#include "cxTypeConversions.h"
#include "cxVtkHelperClasses.h"

namespace cx
{

GraphicalGeometric::GraphicalGeometric()
{
	m_rMrr = Transform3D::Identity();
	mGraphicalPolyDataPtr.reset(new GraphicalPolyData3D());
	mGraphicalGlyph3DDataPtr.reset(new GraphicalGlyph3DData());
}
GraphicalGeometric::~GraphicalGeometric()
{
}

void GraphicalGeometric::setMesh(MeshPtr mesh)
{
	if (mesh == mMesh)
		return;
	if (mMesh)
	{
		disconnect(mMesh.get(), &Mesh::meshChanged, this, &GraphicalGeometric::meshChangedSlot);
		disconnect(mMesh.get(), &Data::transformChanged, this, &GraphicalGeometric::transformChangedSlot);
		disconnect(mMesh.get(), &Data::clipPlanesChanged, this, &GraphicalGeometric::clipPlanesChangedSlot);
	}
	mMesh = mesh;
	if (mMesh)
	{
		connect(mMesh.get(), &Mesh::meshChanged, this, &GraphicalGeometric::meshChangedSlot);
		connect(mMesh.get(), &Data::transformChanged, this, &GraphicalGeometric::transformChangedSlot);
		connect(mMesh.get(), &Data::clipPlanesChanged, this, &GraphicalGeometric::clipPlanesChangedSlot);
		this->meshChangedSlot();
		this->transformChangedSlot();
		this->clipPlanesChangedSlot();
	}
}

//Copied from ImageMapperMonitor (used by VolumetricRep)
void GraphicalGeometric::clipPlanesChangedSlot()
{
	this->clearClipping();

	if (!mMesh)
		return;

	std::vector<vtkPlanePtr> mPlanes;
	mPlanes = mMesh->getAllClipPlanes();
	for (unsigned i=0; i<mPlanes.size(); ++i)
	{
		mGraphicalPolyDataPtr->getMapper()->AddClippingPlane(mPlanes[i]);
	}
}

void GraphicalGeometric::clearClipping()
{
	if (!mMesh)
		return;

	mGraphicalPolyDataPtr->getMapper()->RemoveAllClippingPlanes();
}

MeshPtr GraphicalGeometric::getMesh()
{
	return mMesh;
}

void GraphicalGeometric::setRenderer(vtkRendererPtr renderer)
{
	mGraphicalPolyDataPtr->setRenderer(renderer);
	mGraphicalGlyph3DDataPtr->setRenderer(renderer);
}

void GraphicalGeometric::setTransformOffset(Transform3D rMrr)
{
	m_rMrr = rMrr;
	this->transformChangedSlot();
}

void GraphicalGeometric::meshChangedSlot()
{
	mGraphicalGlyph3DDataPtr->setVisibility(mMesh->showGlyph());
	if(mMesh->showGlyph())
	{
		mGraphicalGlyph3DDataPtr->setData(mMesh->getVtkPolyData());
		mGraphicalGlyph3DDataPtr->setOrientationArray(mMesh->getOrientationArray());
		mGraphicalGlyph3DDataPtr->setColorArray(mMesh->getColorArray());
		mGraphicalGlyph3DDataPtr->setColor(mMesh->getColor().redF(), mMesh->getColor().greenF(), mMesh->getColor().blueF());
		mGraphicalGlyph3DDataPtr->setLUT(mMesh->getGlyphLUT());
		mGraphicalGlyph3DDataPtr->setScaleFactor(mMesh->getVisSize());
	}

	mMesh->updateVtkPolyDataWithTexture();
	mGraphicalPolyDataPtr->setData(mMesh->getVtkPolyData());
	mGraphicalPolyDataPtr->setTexture(mMesh->getVtkTexture());

	mGraphicalPolyDataPtr->setOpacity(1.0);
	mGraphicalPolyDataPtr->setScalarVisibility(mMesh->getUseColorFromPolydataScalars());
	if(!mMesh->getUseColorFromPolydataScalars())
	{
	    mGraphicalPolyDataPtr->setColor(mMesh->getColor().redF(), mMesh->getColor().greenF(), mMesh->getColor().blueF());
	    mGraphicalPolyDataPtr->setOpacity(mMesh->getColor().alphaF());
	}

	//Set other properties
	vtkPropertyPtr dest = mGraphicalPolyDataPtr->getProperty();
	const MeshPropertyData& src = mMesh->getProperties();

	dest->SetPointSize(src.mVisSize->getValue());
	dest->SetBackfaceCulling(src.mBackfaceCulling->getValue());
	dest->SetFrontfaceCulling(src.mFrontfaceCulling->getValue());
	dest->SetRepresentation(src.mRepresentation->getValue().toInt());
	dest->SetEdgeVisibility(src.mEdgeVisibility->getValue());
	dest->SetEdgeColor(cx::getColorAsVector3D(src.mEdgeColor->getValue()).begin());
	dest->SetAmbient(src.mAmbient->getValue());
	dest->SetDiffuse(src.mDiffuse->getValue());
	dest->SetSpecular(src.mSpecular->getValue());
	dest->SetSpecularPower(src.mSpecularPower->getValue());
}

/**called when transform is changed
 * reset it in the prop.*/
void GraphicalGeometric::transformChangedSlot()
{
	if (!mMesh)
	{
		return;
	}

	Transform3D rrMd = mMesh->get_rMd();
	Transform3D rMd = m_rMrr * rrMd;

	mGraphicalPolyDataPtr->setUserMatrix(rMd.getVtkMatrix());
	mGraphicalGlyph3DDataPtr->setUserMatrix(rMd.getVtkMatrix());
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


GeometricRep::GeometricRep() :
	RepImpl()
{
	mGraphics.reset(new GraphicalGeometric());
}
GeometricRep::~GeometricRep()
{
}
GeometricRepPtr GeometricRep::New(const QString& uid)
{
	return wrap_new(new GeometricRep(), uid);
}

void GeometricRep::addRepActorsToViewRenderer(ViewPtr view)
{
	mGraphics->setRenderer(view->getRenderer());
}

void GeometricRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
	mGraphics->setRenderer(NULL);
}

void GeometricRep::setMesh(MeshPtr mesh)
{
	mGraphics->setMesh(mesh);
}

MeshPtr GeometricRep::getMesh()
{
	return mGraphics->getMesh();
}
bool GeometricRep::hasMesh(MeshPtr mesh) const
{
	return mGraphics->getMesh() == mesh;
}

//---------------------------------------------------------
}
// namespace cx
//---------------------------------------------------------
