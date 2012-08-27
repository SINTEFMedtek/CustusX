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

/*
 * sscTexture3DSlicerProxyImpl.cpp
 *
 *  Created on: Oct 13, 2011
 *      Author: christiana
 */

#include <sscTexture3DSlicerProxy.h>
#include "sscTextureSlicePainter.h"

#include <vtkRenderer.h>
#include <vtkFloatArray.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkTriangleFilter.h>
#include <vtkStripper.h>
#include <vtkImageData.h>
#include <vtkPainterPolyDataMapper.h>
#include <vtkLookupTable.h>

#include "sscImage.h"
#include "sscView.h"
#include "sscImageLUT2D.h"
#include "sscSliceProxy.h"
#include "sscTypeConversions.h"
#include "sscGPUImageBuffer.h"

//---------------------------------------------------------
namespace ssc
{
//---------------------------------------------------------

#ifdef WIN32

Texture3DSlicerProxyPtr Texture3DSlicerProxy::New()
{
	return Texture3DSlicerProxyPtr(new Texture3DSlicerProxy());
}

#else

Texture3DSlicerProxyPtr Texture3DSlicerProxy::New()
{
	return Texture3DSlicerProxyImpl::New();
}


void Texture3DSlicerProxyImpl::setTargetSpaceToR()
{
	mTargetSpaceIsR = true;
}

Texture3DSlicerProxyImpl::Texture3DSlicerProxyImpl()
{
	mTargetSpaceIsR = false;
//	mView = NULL;
	//	std::cout << "create Texture3DSlicerRep" << std::endl;
	mActor = vtkActorPtr::New();
	mPainter = TextureSlicePainterPtr::New();
	// default shader for sonowand: override using setshaderfile()
	mPainter->setShaderFile("/Data/Resources/Shaders/Texture3DOverlay.frag");
	mPainterPolyDatamapper = vtkPainterPolyDataMapperPtr::New();

	mPlaneSource = vtkPlaneSourcePtr::New();

	vtkTriangleFilterPtr triangleFilter = vtkTriangleFilterPtr::New(); //create triangle polygons from input polygons
	triangleFilter->SetInputConnection(mPlaneSource->GetOutputPort()); //in this case a Planesource

	vtkStripperPtr stripper = vtkStripperPtr::New();
	stripper->SetInputConnection(triangleFilter->GetOutputPort());
	stripper->Update();

	mPolyData = stripper->GetOutput();
	mPolyData->GetPointData()->SetNormals(NULL);

	mPainter->SetDelegatePainter(mPainterPolyDatamapper->GetPainter());
	mPainterPolyDatamapper->SetPainter(mPainter);
	mPainterPolyDatamapper->SetInput(mPolyData);
	mActor->SetMapper(mPainterPolyDatamapper);
}

Texture3DSlicerProxyImpl::~Texture3DSlicerProxyImpl()
{
	mImages.clear();
}

Texture3DSlicerProxyPtr Texture3DSlicerProxyImpl::New()
{
	return Texture3DSlicerProxyPtr(new Texture3DSlicerProxyImpl());
}

vtkActorPtr Texture3DSlicerProxyImpl::getActor()
{
	return mActor;
}

void Texture3DSlicerProxyImpl::setShaderFile(QString shaderFile)
{
	mPainter->setShaderFile(shaderFile);
}

//void Texture3DSlicerProxyImpl::viewChanged()
//{
//	if (!mView)
//		return;
//	if (!mView->getZoomFactor() < 0)
//		return; // ignore if zoom is invalid
//	this->setViewportData(mView->get_vpMs(), mView->getViewport());
//}

void Texture3DSlicerProxyImpl::setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp)
{
	if (!mTargetSpaceIsR)
	{
		mBB_s = transform(vpMs.inv(), vp);
	}

	this->resetGeometryPlane();
}

void Texture3DSlicerProxyImpl::resetGeometryPlane()
{
	if (mTargetSpaceIsR)
	{
		// use largest bb from all volume box vertices projected onto s:
		Transform3D rMs = mSliceProxy->get_sMr().inv();
		DoubleBoundingBox3D bb_d = mImages[0]->boundingBox();
		Transform3D sMd = rMs.inv()*mImages[0]->get_rMd();
		std::vector<Vector3D> pp_s;
		for (unsigned x=0; x<2; ++x)
			for (unsigned y=0; y<2; ++y)
				for (unsigned z=0; z<2; ++z)
					pp_s.push_back(sMd.coord(bb_d.corner(x,y,x)));

		mBB_s = DoubleBoundingBox3D::fromCloud(pp_s);
		mBB_s[4] = 0;
		mBB_s[5] = 0;
//		double extent = 100;
//		mBB_s = DoubleBoundingBox3D(-extent, extent, -extent, extent, 0, 0);
	}

	Vector3D origin(mBB_s[0], mBB_s[2], 0);
	Vector3D p1(mBB_s[1], mBB_s[2], 0);
	Vector3D p2(mBB_s[0], mBB_s[3], 0);

	if (mTargetSpaceIsR)
	{
		Transform3D rMs = mSliceProxy->get_sMr().inv();
		p1 = rMs.coord(p1);
		p2 = rMs.coord(p2);
		origin = rMs.coord(origin);
	}

	if (similar(mBB_s.range()[0] * mBB_s.range()[1], 0.0))
	{
//		std::cout << "zero-size bounding box in texture slicer- ignoring" << std::endl;
		return;
	}

	createGeometryPlane(p1, p2, origin);
}

void Texture3DSlicerProxyImpl::createGeometryPlane( Vector3D point1_s,  Vector3D point2_s, Vector3D origin_s )
{
//  std::cout << "createGeometryPlane " << point1_s << ", " << point2_s << ", " << origin_s << std::endl;
	mPlaneSource->SetPoint1( point1_s.begin() );
	mPlaneSource->SetPoint2( point2_s.begin() );
	mPlaneSource->SetOrigin( origin_s.begin() );
//  std::cout << "createGeometryPlane update begin" << std::endl;
	mPolyData->Update();
//  std::cout << "createGeometryPlane update end" << std::endl;
	// each stripper->update() resets the contents of polydata, thus we must reinsert the data here.
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		updateCoordinates(i);
	}
}

void Texture3DSlicerProxyImpl::setImages(std::vector<ssc::ImagePtr> images)
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
		disconnect(mImages[i].get(), SIGNAL(vtkImageDataChanged()), this, SLOT(imageChanged()));
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
		connect(mImages[i].get(), SIGNAL(vtkImageDataChanged()), this, SLOT(imageChanged()));
		this->updateCoordinates(i);
	}
	this->updateColorAttributeSlot();

	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		mPainterPolyDatamapper->MapDataArrayToMultiTextureAttribute(2 * i,
			cstring_cast(this->getTCoordName(i)),
			vtkDataObject::FIELD_ASSOCIATION_POINTS);
	}
}


void Texture3DSlicerProxyImpl::setSliceProxy(ssc::SliceProxyPtr slicer)
{
	if (mSliceProxy)
		disconnect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(transformChangedSlot()));
	mSliceProxy = slicer;
	if (mSliceProxy)
	{
		connect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this,	SLOT(transformChangedSlot()));
		for (unsigned i=0; i < mImages.size(); ++i)
		{
			updateCoordinates(i);
		}
	}
}

//void Texture3DSlicerProxyImpl::addRepActorsToViewRenderer(ssc::View* view)
//{
////  std::cout << "void Texture3DSlicerRep::addRepActorsToViewRenderer(ssc::View* view)" << std::endl;
//    view->getRenderer()->AddActor(mActor);
//    mView = view;
//    connect(view, SIGNAL(resized(QSize)), this, SLOT(viewChanged()));
//    this->viewChanged();
//}

QString Texture3DSlicerProxyImpl::getTCoordName(int index)
{
	return  "texture"+qstring_cast(index);
}

void Texture3DSlicerProxyImpl::updateCoordinates(int index)
{
//	std::cout << "Texture3DSlicerRep::updateCoordinates" << std::endl;
	if (!mPolyData || !mSliceProxy)
		return;

	vtkImageDataPtr volume = mImages[index]->getBaseVtkImageData();
	// create a bb describing the volume in physical (raw data) space
	Vector3D origin(volume->GetOrigin());
	Vector3D spacing(volume->GetSpacing());
	DoubleBoundingBox3D imageSize(volume->GetWholeExtent());

	for (int i = 0; i < 3; ++i)
	{
		imageSize[2 * i] = origin[i] + spacing[i] * (imageSize[2 * i] - 0.5);
		imageSize[2 * i + 1] = origin[i] + spacing[i] * (imageSize[2 * i + 1] + 0.5);
	}

	// identity bb
	DoubleBoundingBox3D textureSpace(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);

	// create transform from slice space to raw data space
	Transform3D iMs = mImages[index]->get_rMd().inv() * mSliceProxy->get_sMr().inv();
	//		std::cout << "Texture3DSlicerRep iMs " << this <<  "\n" << iMs << std::endl;
	// create transform from image space to texture normalized space
	Transform3D nMi = createTransformNormalize(imageSize, textureSpace);
	// total transform from slice space to texture space
	Transform3D nMs = nMi * iMs;
	// transform the viewport to texture coordinates (must use coords since bb3D doesnt handle non-axis-aligned transforms)
	std::vector<Vector3D> plane(4);
	plane[0] = mBB_s.corner(0, 0, 0);
	plane[1] = mBB_s.corner(1, 0, 0);
	plane[2] = mBB_s.corner(0, 1, 0);
	plane[3] = mBB_s.corner(1, 1, 0);

	for (unsigned i = 0; i < plane.size(); ++i)
	{
		plane[i] = nMs.coord(plane[i]);
	}

	vtkFloatArrayPtr TCoords = vtkFloatArray::SafeDownCast(mPolyData->GetPointData()->GetArray(
		cstring_cast(getTCoordName(index))));

	if (!TCoords) // create the TCoords
	{
		TCoords = vtkFloatArrayPtr::New();
		TCoords->SetNumberOfComponents(3);
		TCoords->Allocate(4 * 3);
		TCoords->InsertNextTuple3(0.0, 0.0, 0.0);
		TCoords->InsertNextTuple3(0.0, 0.0, 0.0);
		TCoords->InsertNextTuple3(0.0, 0.0, 0.0);
		TCoords->InsertNextTuple3(0.0, 0.0, 0.0);
		TCoords->SetName(cstring_cast(getTCoordName(index)));
		mPolyData->GetPointData()->AddArray(TCoords);
	}

	for (unsigned i = 0; i < plane.size(); ++i)
	{
		TCoords->SetTuple3(i, plane[i][0], plane[i][1], plane[i][2]);
	}

	mPolyData->Modified();
}

//void Texture3DSlicerProxyImpl::removeRepActorsFromViewRenderer(ssc::View* view)
//{
//	view->getRenderer()->RemoveActor(mActor);
//	disconnect(view, SIGNAL(resized(QSize)), this, SLOT(viewChanged()));
//	mView = NULL;
//}

//void Texture3DSlicerProxyImpl::printSelf(std::ostream & os, ssc::Indent indent)
//{
//    //mImageSlicer->printSelf(os, indent);
//}

void Texture3DSlicerProxyImpl::updateColorAttributeSlot()
{
	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		vtkImageDataPtr inputImage = mImages[i]->getBaseVtkImageData() ;

		vtkLookupTablePtr lut = mImages[i]->getLookupTable2D()->getBaseLookupTable();
		ssc::GPUImageLutBufferPtr lutBuffer = ssc::GPUImageBufferRepository::getInstance()->getGPUImageLutBuffer(lut->GetTable());

		// no lut indicates to the fragment shader that RGBA should be used
		if (inputImage->GetNumberOfScalarComponents()==1)
		{
			mPainter->SetLutBuffer(i, lutBuffer);
		}

		int scalarTypeMax = (int)inputImage->GetScalarTypeMax();
		float window = (float) mImages[i]->getLookupTable2D()->getWindow() / scalarTypeMax;
		float llr = (float) mImages[i]->getLookupTable2D()->getLLR() / scalarTypeMax;
		float level = (float) mImages[i]->getLookupTable2D()->getLevel() / scalarTypeMax;
		float alpha = (float) mImages[i]->getLookupTable2D()->getAlpha();
		mPainter->SetColorAttribute(i, window, level, llr, alpha);
	}
	mActor->Modified();
}

void Texture3DSlicerProxyImpl::transformChangedSlot()
{
	if (mTargetSpaceIsR)
		this->resetGeometryPlane();
//	this->viewChanged();
	this->update();
}

void Texture3DSlicerProxyImpl::imageChanged()
{
	mActor->Modified();
}

void Texture3DSlicerProxyImpl::update()
{
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		updateCoordinates(i);
	}
}

#endif // WIN32


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------

