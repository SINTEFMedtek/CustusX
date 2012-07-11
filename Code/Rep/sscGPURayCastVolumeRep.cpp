// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2012- SINTEF Medical Technology
// Copyright (C) 2012- Sonowand AS
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

#ifndef WIN32
#include "sscGPURayCastVolumeRep.h"

#include <stdint.h>

#include <vtkAppendPolyData.h>
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
#include <vtkProperty.h>

#include "sscImage.h"
#include "sscView.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include "sscTypeConversions.h"
#include "sscGPUImageBuffer.h"
#include "sscGPURayCastVolumePainter.h"

//---------------------------------------------------------
namespace ssc
{
//---------------------------------------------------------

static vtkPolyDataPtr createCube()
{
	static float x[8][3]={{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
	                      {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}};
	static vtkIdType pts[6][4]={{0,1,2,3}, {4,5,6,7}, {0,1,5,4},
	                            {1,2,6,5}, {2,3,7,6}, {3,0,4,7}};
	// We'll create the building blocks of polydata including data attributes.
	vtkPolyDataPtr cube = vtkPolyDataPtr::New();
	vtkPoints *points = vtkPoints::New();
	vtkCellArray *polys = vtkCellArray::New();
	int i;
	// Load the point, cell, and data attributes.
	for (i=0; i<8; i++) points->InsertPoint(i,x[i]);
	for (i=0; i<6; i++) polys->InsertNextCell(4,pts[i]);

	// We now assign the pieces to the vtkPolyData.
	cube->SetPoints(points);
	points->Delete();
	cube->SetPolys(polys);
	polys->Delete();
	return cube;
}

double maxIntensity(ssc::ImagePtr image)
{
	return (double)image->getMax()/image->getBaseVtkImageData()->GetScalarTypeMax();
}

GPURayCastVolumeRep::GPURayCastVolumeRep(const QString& uid) :
	RepImpl(uid)
{
	//	std::cout << "create Texture3DSlicerRep" << std::endl;
	mActor = vtkActorPtr::New();
	mPainter = GPURayCastVolumePainterPtr::New();
	mPainter->setStepSize(defaultStepSize);
	// default shader for sonowand: override using setshaderfile()
	mPainter->setShaderFolder("/Data/Resources/Shaders/");
	mPainterPolyDatamapper = vtkPainterPolyDataMapperPtr::New();

	mMerger = vtkAppendPolyDataPtr::New();

	mPainter->SetDelegatePainter(mPainterPolyDatamapper->GetPainter());
	mPainterPolyDatamapper->SetPainter(mPainter);
	mPainterPolyDatamapper->SetInput(mMerger->GetOutput());
	mActor->SetMapper(mPainterPolyDatamapper);
	mActor->GetProperty()->SetOpacity(0.99);
}

GPURayCastVolumeRep::~GPURayCastVolumeRep()
{
}

GPURayCastVolumeRepPtr GPURayCastVolumeRep::New(const QString& uid)
{
	GPURayCastVolumeRepPtr retval(new GPURayCastVolumeRep(uid));
	retval->mSelf = retval;
	return retval;
}

void GPURayCastVolumeRep::setShaderFolder(QString folder)
{
	mPainter->setShaderFolder(folder);
}

void GPURayCastVolumeRep::setImages(std::vector<ssc::ImagePtr> images)
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
	mClipVolumes.clear();

	mImages = images;
	if (mImages.size() > mPainter->maxVolumes)
	{
		mImages.resize(mPainter->maxVolumes);
	}

	while (mMerger->GetInput())
	{
		mMerger->RemoveInput(mMerger->GetInput());
	}

	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		connect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		vtkImageDataPtr inputImage = mImages[i]->getBaseVtkImageData();//

		ssc::GPUImageDataBufferPtr dataBuffer = ssc::GPUImageBufferRepository::getInstance()->getGPUImageDataBuffer(
			inputImage);

		double maxVal = maxIntensity(images[i]);
		mPainter->SetVolumeBuffer(i, dataBuffer, maxVal);

		connect(mImages[i].get(), SIGNAL(transferFunctionsChanged()), this, SLOT(updateColorAttributeSlot()));

		vtkPolyDataPtr cube = createCube();
		mTransformPolyData[i] = vtkTransformPolyDataFilterPtr::New();
		mTransformPolyData[i]->SetInput(cube);
		mMerger->AddInput(mTransformPolyData[i]->GetOutput());
	}
	this->updateColorAttributeSlot();
	transformChangedSlot();
}

void GPURayCastVolumeRep::transformChangedSlot()
{
	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		DoubleBoundingBox3D bb = mImages[i]->boundingBox();
		vtkMatrixToLinearTransformPtr transform = vtkMatrixToLinearTransformPtr::New();
		transform->SetInput((mImages[i]->get_rMd()*createTransformScale(Vector3D(bb[1]-bb[0], bb[3]-bb[2], bb[5]-bb[4]))).getVtkMatrix());
		mTransformPolyData[i]->SetTransform(transform);

		// identity bb
		DoubleBoundingBox3D textureSpace(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);

		// create transform from world space to raw data space
		Transform3D iMr = mImages[i]->get_rMd().inv();
		// create transform from image space to texture normalized space
		Transform3D nMi = createTransformNormalize(mImages[i]->boundingBox(), textureSpace);
		// total transform from slice space to texture space
		Transform3D nMr = nMi * iMr;
		mPainter->set_nMr(i, nMr);
	}
}

std::vector<ssc::ImagePtr> GPURayCastVolumeRep::getImages()
{
	return mImages;
}

void GPURayCastVolumeRep::addRepActorsToViewRenderer(ssc::View *view)
{
	view->getRenderer()->AddActor(mActor);
}

void GPURayCastVolumeRep::removeRepActorsFromViewRenderer(ssc::View *view)
{
	view->getRenderer()->RemoveActor(mActor);
}

void GPURayCastVolumeRep::printSelf(std::ostream & os, ssc::Indent indent)
{
}

void GPURayCastVolumeRep::updateColorAttributeSlot()
{
	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		vtkImageDataPtr inputImage = mImages[i]->getBaseVtkImageData() ;

		vtkLookupTablePtr lut = mImages[i]->getTransferFunctions3D()->getLut();

		ssc::GPUImageLutBufferPtr lutBuffer;
		if (lut) lutBuffer = ssc::GPUImageBufferRepository::getInstance()->getGPUImageLutBuffer(lut->GetTable());

		// no lut indicates to the fragment shader that RGBA should be used
		if (inputImage->GetNumberOfScalarComponents()==1)
		{
			mPainter->SetLutBuffer(i, lutBuffer);
		}

		int scalarTypeMax = (int)inputImage->GetScalarTypeMax();
		float window = (float) mImages[i]->getTransferFunctions3D()->getWindow() / scalarTypeMax;
		float llr = (float) mImages[i]->getTransferFunctions3D()->getLLR() / scalarTypeMax;
		float level = (float) mImages[i]->getTransferFunctions3D()->getLevel() / scalarTypeMax;
		float alpha = (float) mImages[i]->getTransferFunctions3D()->getAlpha();
		mPainter->SetColorAttribute(i, window, level, llr, alpha);
	}
	mActor->Modified();
}

void GPURayCastVolumeRep::setClipper(SlicePlaneClipperPtr clipper)
{
	mClipper = clipper;
	mPainter->setClipper(clipper);
	mActor->Modified();
}

void GPURayCastVolumeRep::setClipVolumes(QStringList volumes)
{
	mClipVolumes = QSet<QString>::fromList(volumes);
	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		mPainter->setClipVolume(i, mClipVolumes.contains(mImages[i]->getUid()));
	}
	mActor->Modified();
}

void GPURayCastVolumeRep::setStepSize(double stepsize)
{
	mPainter->setStepSize(stepsize);
	mActor->Modified();
}

void GPURayCastVolumeRep::enableImagePlaneDownsampling(int maxPixels)
{
	mPainter->enableImagePlaneDownsampling(maxPixels);
	mActor->Modified();
}

void GPURayCastVolumeRep::disableImagePlaneDownsampling()
{
	mPainter->disableImagePlaneDownsampling();
	mActor->Modified();
}

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif // WIN32
