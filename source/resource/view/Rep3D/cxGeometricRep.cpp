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
#include "cxLogger.h"


#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkArrowSource.h>

#include "cxMesh.h"
#include "cxView.h"

#include "cxTypeConversions.h"

namespace cx
{

GeometricRep::GeometricRep() :
    RepImpl()
{
    mGraphicalPolyDataPtr.reset(new GraphicalPolyData());
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
        disconnect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));
        disconnect(mMesh.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
    }
    mMesh = mesh;
    if (mMesh)
    {
        connect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));
        connect(mMesh.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
        this->meshChangedSlot();
        this->transformChangedSlot();
    }
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
    //	std::cout << "GeometricRep::meshChangedSlot()" << std::endl;
    //	mMesh->connectToRep(mSelf);

    //if glyph and showGlyoh:
    //    mMapper->SetOrientationArray("Flow direction"); //mMesh.getOrientationArray()
    //    mMapper->SelectColorArray("Vessel velocity");//mMesh.getColorArray()
    //    mMapper->SetLookupTable(); //mMesh.getLookupTable()

    mGraphicalGlyph3DDataPtr->updateGlyph(mMesh->getVtkPolyData(),"Flow direction","Vessel velocity");


    mGraphicalPolyDataPtr->setData(mMesh->getVtkPolyData());
    mGraphicalPolyDataPtr->setIsWireFrame(mMesh->getIsWireframe());
    mGraphicalPolyDataPtr->scalarVisibilityOff();//Don't use the LUT from the VtkPolyData

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

GraphicalMeshBase::GraphicalMeshBase()
{
    mProperty = vtkPropertyPtr::New();
    mActor = vtkActorPtr::New();
    mActor->SetProperty(mProperty);
    mProperty->SetPointSize(2);
}

GraphicalMeshBase::~GraphicalMeshBase()
{
    this->setRenderer(NULL);
}


void GraphicalMeshBase::setRenderer(vtkRendererPtr renderer)
{
    if (mRenderer)
        mRenderer->RemoveActor(mActor);

    mRenderer = renderer;

    if (mRenderer)
        mRenderer->AddActor(mActor);
}


void GraphicalMeshBase::setBackfaceCulling(bool val)
{
    mActor->GetProperty()->SetBackfaceCulling(val);
}

void GraphicalMeshBase::setFrontfaceCulling(bool val)
{
    mActor->GetProperty()->SetFrontfaceCulling(val);
}

void GraphicalMeshBase::setRepresentation()
{
    mActor->GetProperty()->SetRepresentationToSurface();
}

void GraphicalMeshBase::setColor(double red, double green, double blue)
{
    mActor->GetProperty()->SetColor(red, green, blue);
}

void GraphicalMeshBase::setOpacity(double val)
{
    mActor->GetProperty()->SetOpacity(val);
}


void GraphicalMeshBase::setUserMatrix(vtkMatrix4x4 *matrix)
{
    mActor->SetUserMatrix(matrix);
}

vtkActorPtr GraphicalMeshBase::getActor()
{
    return mActor;
}

vtkPolyDataPtr GraphicalMeshBase::getPolyData()
{
    return mData;
}


//---------------------------------------------------------

GraphicalPolyData::GraphicalPolyData() :
    GraphicalMeshBase()
{
    mMapper =  vtkPolyDataMapperPtr::New();
    mActor->SetMapper(mMapper);

    mIsWireFrame = false;
}


void GraphicalPolyData::setIsWireFrame(bool val)
{
    mIsWireFrame = val;
}


void GraphicalPolyData::setRepresentation()
{
    if (mIsWireFrame)
        mActor->GetProperty()->SetRepresentationToWireframe();
    else
        mActor->GetProperty()->SetRepresentationToSurface();
}

void GraphicalPolyData::setData(vtkPolyDataPtr data)
{
    mData = data;
    if (data)
        mMapper->SetInputData(mData);
}

void GraphicalPolyData::scalarVisibilityOff()
{
    mMapper->ScalarVisibilityOff();//Don't use the LUT from the VtkPolyData
}




//---------------------------------------------------------

GraphicalGlyph3DData::GraphicalGlyph3DData() :
    GraphicalMeshBase()
{
    mMapper = vtkSmartPointer<vtkGlyph3DMapper>::New();
    vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
    mMapper->SetSourceConnection(arrowSource->GetOutputPort());

    mActor->SetMapper(mMapper);
}


void GraphicalGlyph3DData::updateGlyph(vtkPolyDataPtr data, const char * orientationArray, const char * colorArray)
{
    mData = data;
    if (!data)
        return;


    if(mData->GetPointData()->GetNumberOfArrays()>0)
    {
        report("Found field data");
        report(QString(mData->GetPointData()->GetArrayName(0)));
        int test = mData->GetPointData()->GetArray(0)->GetNumberOfComponents();
        report(QString(test));
        mMapper->SetInputData(mData);
        mMapper->SetOrientationArray(orientationArray);
        mMapper->SelectColorArray(colorArray);
        mMapper->SetScalarVisibility(1);
        mMapper->SetUseLookupTableScalarRange(1);
        mMapper->SetScalarMode(VTK_SCALAR_MODE_USE_POINT_FIELD_DATA);
        // mMapper->SetLookupTable(); //mMesh.getLookupTable()
    }else{
        report("Found no field data");
        report(QString(mData->GetFieldData()->GetNumberOfArrays()));
    }
}



//---------------------------------------------------------
}
// namespace cx
//---------------------------------------------------------
