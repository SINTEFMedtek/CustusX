/*
 * vmTexture3DSlicerRep.cpp
 *
 *  Created on: Oct 13, 2009
 *      Author: petterw
 */

#include "sscTexture3DSlicerRep.h"
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

typedef vtkSmartPointer<class vtkTriangleFilter> vtkTriangleFilterPtr;

//---------------------------------------------------------
namespace ssc
{
//---------------------------------------------------------

Texture3DSlicerRep::Texture3DSlicerRep(const QString& uid) :
	RepImpl(uid)
{
	std::cout << "create Texture3DSlicerRep" << std::endl;
	mActor = vtkActorPtr::New();
	mPainter = TextureSlicePainterPtr::New();
	mPainterPolyDatamapper = vtkPainterPolyDataMapperPtr::New();
}

Texture3DSlicerRep::~Texture3DSlicerRep()
{
	mImages.clear();
}

Texture3DSlicerRepPtr Texture3DSlicerRep::New(const QString& uid)
{
	Texture3DSlicerRepPtr retval(new Texture3DSlicerRep(uid));
	retval->mSelf = retval;
	return retval;
}

void Texture3DSlicerRep::setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp)
{
	mBB_s = transform(vpMs.inv(), vp);

	Vector3D p1(mBB_s[1], mBB_s[2], mBB_s[4]);
	Vector3D p2(mBB_s[0], mBB_s[3], mBB_s[4]);
	Vector3D origin(mBB_s[0], mBB_s[2], 0);

	createGeometryPlane(p1, p2, origin);
}

void Texture3DSlicerRep::createGeometryPlane( Vector3D point1_s,  Vector3D point2_s, Vector3D origin_s )
{
	if (!mPlaneSource)
	{
		mPlaneSource = vtkPlaneSourcePtr::New();

		mPlaneSource->SetPoint1( point1_s.begin() );
		mPlaneSource->SetPoint2( point2_s.begin() );
		mPlaneSource->SetOrigin( origin_s.begin() );

		vtkTriangleFilterPtr triangleFilter = vtkTriangleFilterPtr::New(); //create triangle polygons from input polygons
		triangleFilter->SetInputConnection( mPlaneSource->GetOutputPort() ); //in this case a Planesource

		mStripper = vtkStripperPtr::New();
		mStripper->SetInputConnection(triangleFilter->GetOutputPort());
		mStripper->Update();

		mPolyData = mStripper->GetOutput();
		mPolyData->GetPointData()->SetNormals(NULL);
	}

	mPlaneSource->SetPoint1( point1_s.begin() );
	mPlaneSource->SetPoint2( point2_s.begin() );
	mPlaneSource->SetOrigin( origin_s.begin() );
	mStripper->Update();

	// each stripper->update() resets the contents of polydata, thus we must reinsert the data here.
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		createCoordinates(i);
		updateCoordinates(i);
	}
}

void Texture3DSlicerRep::setImages(std::vector<ssc::ImagePtr> images)
{
	mImages = images;
	for (unsigned i = 0; i < mImages .size(); ++i)
	{
		//vtkImageDataPtr inputImage = images[i]->getGrayScaleBaseVtkImageData() ;//
		vtkImageDataPtr inputImage = images[i]->getBaseVtkImageData() ;//

		ssc::GPUImageDataBufferPtr dataBuffer = ssc::GPUImageBufferRepository::getInstance()->getGPUImageDataBuffer(inputImage);

		mPainter->SetVolumeBuffer(i, dataBuffer);

		connect(images[i].get(), SIGNAL(transferFunctionsChanged()), this, SLOT(updateColorAttributeSlot()));
		createCoordinates(i);
	}
	updateColorAttributeSlot();
}

void Texture3DSlicerRep::setSliceProxy(ssc::SliceProxyPtr slicer)
{
	mSliceProxy = slicer;
	if (mSliceProxy)
	{
		disconnect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this,
				SLOT(sliceTransformChangedSlot(Transform3D)));
	}
	mSliceProxy = slicer;
	if (mSliceProxy)
	{
		connect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this,
				SLOT(sliceTransformChangedSlot(Transform3D)));
	}
}

void Texture3DSlicerRep::addRepActorsToViewRenderer(ssc::View* view)
{
	if (mImages.empty())
	{
		return;
	}
	mPainter->SetDelegatePainter(mPainterPolyDatamapper->GetPainter());
	mPainterPolyDatamapper->SetPainter(mPainter);
	mPainterPolyDatamapper->SetInput(mPolyData);
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		mPainterPolyDatamapper->MapDataArrayToMultiTextureAttribute(2*i, cstring_cast(getTCoordName(i)), vtkDataObject::FIELD_ASSOCIATION_POINTS);
	}

	mActor->SetMapper(mPainterPolyDatamapper);
	view->getRenderer()->AddActor(mActor);
}
void Texture3DSlicerRep::createCoordinates(int count)
{
	vtkFloatArrayPtr TCoords = vtkFloatArrayPtr::New();
	TCoords->SetNumberOfComponents(3);
	TCoords->Allocate(4* 3 );
	TCoords->InsertNextTuple3( 0.0, 0.0, 0.0 );
	TCoords->InsertNextTuple3( 0.0, 0.0, 0.0 );
	TCoords->InsertNextTuple3( 0.0, 0.0, 0.0 );
	TCoords->InsertNextTuple3( 0.0, 0.0, 0.0 );
	TCoords->SetName(cstring_cast(getTCoordName(count)) );

	if (!mPolyData)
		std::cerr << "Error in Texture3DSlicerRep: setViewport was not called" << std::endl;
	mPolyData->GetPointData()->AddArray(TCoords);
	//mPainterPolyDatamapper->MapDataArrayToMultiTextureAttribute(2*count, cstring_cast(getTCoordName(count)), vtkDataObject::FIELD_ASSOCIATION_POINTS);
}

QString Texture3DSlicerRep::getTCoordName(int index)
{
	 return  "texture"+qstring_cast(index);
}

void Texture3DSlicerRep::updateCoordinates(int index)
{
		//vtkImageDataPtr volume = mImages[index]->getGrayScaleBaseVtkImageData();
		vtkImageDataPtr volume = mImages[index]->getBaseVtkImageData();
		// create a bb describing the volume in physical (raw data) space
		Vector3D origin(volume->GetOrigin());
		Vector3D spacing(volume->GetSpacing());
		DoubleBoundingBox3D imageSize(volume->GetWholeExtent());

		for (int i=0; i<3; ++i)
		{
			imageSize[2*i  ] = origin[i] + spacing[i] * (imageSize[2*i  ] - 0.5);
			imageSize[2*i+1] = origin[i] + spacing[i] * (imageSize[2*i+1] + 0.5);
		}

		// identity bb
		DoubleBoundingBox3D textureSpace(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);

		// create transform from slice space to raw data space
		Transform3D iMs = mImages[index]->get_rMd().inv() * mSliceProxy->get_sMr().inv();
		// create transform from image space to texture normalized space
		Transform3D nMi = createTransformNormalize( imageSize, textureSpace);
		// total transform from slice space to texture space
		Transform3D nMs = nMi*iMs;
		// transform the viewport to texture coordinates (must use coords since bb3D doesnt handle non-axis-aligned transforms)
		std::vector<Vector3D> plane(4);
		plane[0] = mBB_s.corner(0,0,0);
		plane[1] = mBB_s.corner(1,0,0);
		plane[2] = mBB_s.corner(0,1,0);
		plane[3] = mBB_s.corner(1,1,0);

		for (unsigned i=0; i<plane.size(); ++i)
		{
			plane[i] = nMs.coord(plane[i]);
		}

		vtkFloatArrayPtr TCoords = vtkFloatArray::SafeDownCast(mPolyData->GetPointData()->GetArray(cstring_cast(getTCoordName(index))));

		if (!TCoords)
		{
			std::cout << "No TCoords present in polydata mapper" << std::endl;
			return;
		}

		for (unsigned i=0; i<plane.size(); ++i)
		{
			TCoords->SetTuple3(i, plane[i][0], plane[i][1], plane[i][2]);
		}

		mPolyData->Modified();
}


void Texture3DSlicerRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
	view->getRenderer()->RemoveActor(mActor);
}

void Texture3DSlicerRep::printSelf(std::ostream & os, ssc::Indent indent)
{
	//mImageSlicer->printSelf(os, indent);
}

void Texture3DSlicerRep::updateColorAttributeSlot()
{
	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		//vtkImageDataPtr inputImage = mImages[i]->getGrayScaleBaseVtkImageData() ;
		vtkImageDataPtr inputImage = mImages[i]->getBaseVtkImageData() ;
		//inputImage->Update();

		vtkLookupTablePtr lut = mImages[i]->getLookupTable2D()->getBaseLookupTable();
		ssc::GPUImageLutBufferPtr lutBuffer = ssc::GPUImageBufferRepository::getInstance()->getGPUImageLutBuffer(lut->GetTable());

		// no lut indicates to the fragment shader that RGBA should be used
		if (inputImage->GetNumberOfScalarComponents()==1)
		{
			mPainter->SetLutBuffer(i, lutBuffer);
		}

		//Logger::log("vm.log", "i="+string_cast(i)+", smax="+string_cast(inputImage->GetScalarTypeMax())+", win="+string_cast(mImages[i]->getLookupTable2D()->getWindow()));
		int scalarTypeMax = (int)inputImage->GetScalarTypeMax();
		float window = (float) mImages[i]->getLookupTable2D()->getWindow() / scalarTypeMax;
		float llr = (float) mImages[i]->getLookupTable2D()->getLLR() / scalarTypeMax;
		float level = (float) mImages[i]->getLookupTable2D()->getLevel() / scalarTypeMax;
		float alpha = (float) mImages[i]->getLookupTable2D()->getAlpha();
		mPainter->SetColorAttribute(i, window, level, llr, alpha);
	}
}

void Texture3DSlicerRep::sliceTransformChangedSlot(Transform3D sMr)
{
	update();
}

void Texture3DSlicerRep::update()
{
//	Vector3D p1(mBB_s[1], mBB_s[2], mBB_s[4]);
//	Vector3D p2(mBB_s[0], mBB_s[3], mBB_s[4]);
//	Vector3D origin(mBB_s[0], mBB_s[2], 0);
//
//	createGeometryPlane(p1, p2, origin);

	for (unsigned i=0; i<mImages.size(); ++i)
	{
//		createCoordinates(i);
		updateCoordinates(i);
	}

}
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
