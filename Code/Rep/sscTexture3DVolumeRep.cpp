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

#include "sscTexture3DVolumeRep.h"

#include <vtkCellArray.h>
#include <vtkRenderer.h>
#include <vtkFloatArray.h>
#include <vtkCubeSource.h>
#include <vtkPointData.h>
#include <vtkTriangleFilter.h>
#include <vtkStripper.h>
#include <vtkImageData.h>
#include <vtkPainterPolyDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkMatrixToLinearTransform.h>
#include <vtkMatrix4x4.h>

#include "sscImage.h"
#include "sscView.h"
#include "sscImageTF3D.h"
#include "sscTypeConversions.h"
#include "sscGPUImageBuffer.h"
#include "sscTextureVolumePainter.h"

//---------------------------------------------------------
namespace ssc
{
//---------------------------------------------------------

void Texture3DVolumeRep::setTargetSpaceToR()
{
	mTargetSpaceIsR = true;
}

static vtkPolyDataPtr createCube()
{
	static float x[8][3]={{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
	                      {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}};
	static vtkIdType pts[6][4]={{0,1,2,3}, {4,5,6,7}, {0,1,5,4},
	                            {1,2,6,5}, {2,3,7,6}, {3,0,4,7}};
	// We'll create the building blocks of polydata including data attributes.
	vtkPolyDataPtr cube = vtkPolyData::New();
	vtkPoints *points = vtkPoints::New();
	vtkCellArray *polys = vtkCellArray::New();
	vtkFloatArray *tCoords = vtkFloatArray::New();
	tCoords->SetNumberOfComponents(3);
	int i;
	// Load the point, cell, and data attributes.
	for (i=0; i<8; i++) points->InsertPoint(i,x[i]);
	for (i=0; i<8; i++) tCoords->InsertTupleValue(i, x[i]);
	for (i=0; i<6; i++) polys->InsertNextCell(4,pts[i]);
	
	// We now assign the pieces to the vtkPolyData.
	cube->SetPoints(points);
	points->Delete();
	cube->SetPolys(polys);
	polys->Delete();
	cube->GetPointData()->SetTCoords(tCoords);
	tCoords->Delete();
	return cube;
}

Texture3DVolumeRep::Texture3DVolumeRep(const QString& uid) :
	RepImpl(uid)
{
	mView = NULL;
	mTargetSpaceIsR = false;
	//	std::cout << "create Texture3DSlicerRep" << std::endl;
	mActor = vtkActorPtr::New();
	mPainter = TextureVolumePainterPtr::New();
	// default shader for sonowand: override using setshaderfile()
	mPainter->setShaderFile("/Data/Resources/Shaders/volumetest.xml");
	mPainterPolyDatamapper = vtkPainterPolyDataMapperPtr::New();

/**	vtkTriangleFilterPtr triangleFilter = vtkTriangleFilterPtr::New(); //create triangle polygons from input polygons
	triangleFilter->SetInputConnection(mCubeSource->GetOutputPort()); //in this case a Planesource

	vtkStripperPtr stripper = vtkStripperPtr::New();
	stripper->SetInputConnection(triangleFilter->GetOutputPort());
	stripper->Update(); */

	mPolyData = createCube();
	mPolyData->GetPointData()->SetNormals(NULL);
	mTransformPolyData = vtkTransformPolyDataFilter::New();
	mTransformPolyData->SetInput(mPolyData);

	mPainter->SetDelegatePainter(mPainterPolyDatamapper->GetPainter());
	mPainterPolyDatamapper->SetPainter(mPainter);
	mPainterPolyDatamapper->SetInput(mTransformPolyData->GetOutput());
	mActor->SetMapper(mPainterPolyDatamapper);
}

Texture3DVolumeRep::~Texture3DVolumeRep()
{
}

Texture3DVolumeRepPtr Texture3DVolumeRep::New(const QString& uid)
{
    Texture3DVolumeRepPtr retval(new Texture3DVolumeRep(uid));
    retval->mSelf = retval;
    return retval;
}

void Texture3DVolumeRep::setShaderFile(QString shaderFile)
{
	mPainter->setShaderFile(shaderFile);
}

void Texture3DVolumeRep::viewChanged()
{
	if (!mView)
		return;
	if (mView->getZoomFactor() < 0)
		return; // ignore if zoom is invalid
	if (!mTargetSpaceIsR)
	{
		mBB_s = transform(mView->get_vpMs(), mView->getViewport());
	}

}

void Texture3DVolumeRep::setImages(std::vector<ssc::ImagePtr> images)
{
	if (mImages.size() == images.size())
	{
		bool equal = true;
		for (unsigned i = 0; i < mImages.size(); ++i)
			equal &= (mImages[i] == images[i]);
		if (equal)
			return;
	}
	//  std::cout << "Texture3DSlicerRep::setImages" << std::endl;
	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		disconnect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
	}

	mImages = images;

	for (unsigned i = 0; i < mImages .size(); ++i)
	{
		connect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		vtkImageDataPtr inputImage = mImages[i]->getBaseVtkImageData();//

		ssc::GPUImageDataBufferPtr dataBuffer = ssc::GPUImageBufferRepository::getInstance()->getGPUImageDataBuffer(
			inputImage);

		mPainter->SetVolumeBuffer(i, dataBuffer);

		connect(mImages[i].get(), SIGNAL(transferFunctionsChanged()), this, SLOT(updateColorAttributeSlot()));
	}
	this->updateColorAttributeSlot();
	update();

	for (unsigned i = 0; i < mImages.size(); ++i)
	{
#if 0
		mPainterPolyDatamapper->MapDataArrayToMultiTextureAttribute(1,
			cstring_cast(this->getTCoordName(i)),
			vtkDataObject::FIELD_ASSOCIATION_POINTS);
		std::cerr << "Mapping texture coordinates to polydata" << std::endl;
#endif
	}
	DoubleBoundingBox3D bb = mImages[0]->boundingBox();
	vtkMatrixToLinearTransform *transform = vtkMatrixToLinearTransform::New();
	vtkMatrix4x4Ptr vtkMatrix = vtkMatrix4x4::New();
	transform->SetInput((mImages[0]->get_rMd()*createTransformScale(Vector3D(bb[1]-bb[0], bb[3]-bb[2], bb[5]-bb[4]))).getVtkMatrix());
	mTransformPolyData->SetTransform(transform);
}

std::vector<ssc::ImagePtr> Texture3DVolumeRep::getImages()
{
	return mImages;
}

void Texture3DVolumeRep::addRepActorsToViewRenderer(ssc::View* view)
{
    view->getRenderer()->AddActor(mActor);
    mView = view;
    connect(view, SIGNAL(resized(QSize)), this, SLOT(viewChanged()));
    this->viewChanged();
}

void Texture3DVolumeRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
	view->getRenderer()->RemoveActor(mActor);
	disconnect(view, SIGNAL(resized(QSize)), this, SLOT(viewChanged()));
	mView = NULL;
}

void Texture3DVolumeRep::update()
{
    for (unsigned i=0; i<mImages.size(); ++i)
    {
#if 0
	    vtkFloatArrayPtr TCoords = vtkFloatArray::SafeDownCast(mPolyData->GetPointData()->GetArray(
		                                                           cstring_cast(getTCoordName(i))));

	    if (!TCoords) // create the TCoords
	    {
		    TCoords = vtkFloatArrayPtr::New();
		    TCoords->SetNumberOfComponents(3);
		    TCoords->Allocate(8 * 3);
		    TCoords->InsertNextTuple3(0.0, 0.0, 0.0);
		    TCoords->InsertNextTuple3(0.0, 0.0, 1.0);
		    TCoords->InsertNextTuple3(0.0, 1.0, 1.0);
		    TCoords->InsertNextTuple3(0.0, 1.0, 0.0);
		    TCoords->InsertNextTuple3(1.0, 0.0, 1.0);
		    TCoords->InsertNextTuple3(1.0, 1.0, 1.0);
		    TCoords->InsertNextTuple3(1.0, 1.0, 0.0);
		    TCoords->InsertNextTuple3(1.0, 1.0, 1.0);
		    TCoords->SetName(cstring_cast(getTCoordName(i)));
		    mPolyData->GetPointData()->AddArray(TCoords);
		    std::cerr << "Added texture coordinates" << std:: endl;
	    }
#endif

	    mPolyData->Modified();
    }

}

void Texture3DVolumeRep::printSelf(std::ostream & os, ssc::Indent indent)
{

}

void Texture3DVolumeRep::setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp)
{
	if (!mTargetSpaceIsR)
	{
		mBB_s = transform(vpMs.inv(), vp);
	}
}

void Texture3DVolumeRep::updateColorAttributeSlot()
{
	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		vtkImageDataPtr inputImage = mImages[i]->getBaseVtkImageData() ;

//		vtkLookupTablePtr lut = mImages[i]->getLookupTable2D()->getBaseLookupTable();
//		ssc::GPUImageLutBufferPtr lutBuffer = ssc::GPUImageBufferRepository::getInstance()->getGPUImageLutBuffer(lut->GetTable());

		// no lut indicates to the fragment shader that RGBA should be used
//		if (inputImage->GetNumberOfScalarComponents()==1)
//		{
//			mPainter->SetLutBuffer(i, lutBuffer);
//		}

		int scalarTypeMax = (int)inputImage->GetScalarTypeMax();
		float window = (float) mImages[i]->getTransferFunctions3D()->getWindow() / scalarTypeMax;
		float llr = (float) 2* mImages[i]->getTransferFunctions3D()->getLLR() / scalarTypeMax;
		float level = (float) mImages[i]->getTransferFunctions3D()->getLevel() / scalarTypeMax;
		float alpha = (float) mImages[i]->getTransferFunctions3D()->getAlpha();
		mPainter->SetColorAttribute(i, window, level, llr, alpha);
		std::cout << "Using window, level, llr: " << window << ", " << level << ", " << llr << std::endl;
	}
	mActor->Modified();
}
QString Texture3DVolumeRep::getTCoordName(int index)
{
     return  "texture"+qstring_cast(index);
}


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
