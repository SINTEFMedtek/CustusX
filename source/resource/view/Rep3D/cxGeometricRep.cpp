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

namespace cx
{

GeometricRep::GeometricRep() :
    RepImpl()
{
    mGraphicalPolyDataPtr.reset(new GraphicalPolyData3D());
	mGraphicalGlyph3DDataPtr.reset(new GraphicalGlyph3DData());

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
    mGraphicalPolyDataPtr->setRenderer(view->getRenderer());
    mGraphicalGlyph3DDataPtr->setRenderer(view->getRenderer());
}

void GeometricRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
    mGraphicalPolyDataPtr->setRenderer(NULL);
    mGraphicalGlyph3DDataPtr->setRenderer(NULL);
}

void GeometricRep::setMesh(MeshPtr mesh)
{
    if (mesh == mMesh)
        return;
    if (mMesh)
    {
		disconnect(mMesh.get(), &Mesh::meshChanged, this, &GeometricRep::meshChangedSlot);
		disconnect(mMesh.get(), &Data::transformChanged, this, &GeometricRep::transformChangedSlot);
		disconnect(mMesh.get(), &Data::clipPlanesChanged, this, &GeometricRep::clipPlanesChangedSlot);
    }
    mMesh = mesh;
    if (mMesh)
    {
		connect(mMesh.get(), &Mesh::meshChanged, this, &GeometricRep::meshChangedSlot);
		connect(mMesh.get(), &Data::transformChanged, this, &GeometricRep::transformChangedSlot);
		connect(mMesh.get(), &Data::clipPlanesChanged, this, &GeometricRep::clipPlanesChangedSlot);
        this->meshChangedSlot();
        this->transformChangedSlot();
		this->clipPlanesChangedSlot();
    }
}

//Copied from ImageMapperMonitor (used by VolumetricRep)
void GeometricRep::clipPlanesChangedSlot()
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

void GeometricRep::clearClipping()
{
	if (!mMesh)
		return;

	mGraphicalPolyDataPtr->getMapper()->RemoveAllClippingPlanes();
}

MeshPtr GeometricRep::getMesh()
{
    return mMesh;
}
bool GeometricRep::hasMesh(MeshPtr mesh) const
{
    return (mMesh == mesh);
}

void GeometricRep::meshChangedSlot()
{
    //    mMapper->SetLookupTable(); //mMesh.getLookupTable()

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

    mGraphicalPolyDataPtr->setData(mMesh->getVtkPolyData());
    mGraphicalPolyDataPtr->setIsWireFrame(mMesh->getIsWireframe());
    mGraphicalPolyDataPtr->setPointSize(mMesh->getVisSize());
    mGraphicalPolyDataPtr->setScalarVisibility(false);//Don't use the LUT from the VtkPolyData
    //Set mesh color
    mGraphicalPolyDataPtr->setColor(mMesh->getColor().redF(), mMesh->getColor().greenF(), mMesh->getColor().blueF());
    //Set mesh opacity
    mGraphicalPolyDataPtr->setOpacity(mMesh->getColor().alphaF());
    mGraphicalPolyDataPtr->setRepresentation();
    //Set backface and frontface culling
    mGraphicalPolyDataPtr->setBackfaceCulling(mMesh->getBackfaceCulling());
    mGraphicalPolyDataPtr->setFrontfaceCulling(mMesh->getFrontfaceCulling());
}

/**called when transform is changed
 * reset it in the prop.*/
void GeometricRep::transformChangedSlot()
{
    if (!mMesh)
    {
        return;
    }
    mGraphicalPolyDataPtr->setUserMatrix(mMesh->get_rMd().getVtkMatrix());
    mGraphicalGlyph3DDataPtr->setUserMatrix(mMesh->get_rMd().getVtkMatrix());
}



//---------------------------------------------------------
}
// namespace cx
//---------------------------------------------------------
