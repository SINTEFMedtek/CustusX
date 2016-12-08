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

#include "cxTexture3DSlicerProxy.h"

#include <vtkRenderer.h>
#include <vtkFloatArray.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkTriangleFilter.h>
#include <vtkStripper.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkOpenGLRenderWindow.h>

#include <vtkOpenGLPolyDataMapper.h>

#include "cxImage.h"
#include "cxView.h"
#include "cxImageLUT2D.h"
#include "cxSliceProxy.h"
#include "cxTypeConversions.h"
#include "cxGPUImageBuffer.h"
#include "cxReporter.h"
#include "cxConfig.h"

#include "cxDataLocations.h"
#include <qtextstream.h>

#include "cxSharedOpenGLContext.h"

namespace cx
{

//---------------------------------------------------------

Texture3DSlicerProxyPtr Texture3DSlicerProxy::New(SharedOpenGLContextPtr context)
{
	return Texture3DSlicerProxyImpl::New(context);
}
//---------------------------------------------------------


void Texture3DSlicerProxyImpl::setTargetSpaceToR()
{
	mTargetSpaceIsR = true;
}

void Texture3DSlicerProxyImpl::setShaders()
{
	//===========
	// Modify vertex shader declarations
	//===========
	std::string vtk_dec = "//VTK::PositionVC::Dec";
	mOpenGLPolyDataMapper->AddShaderReplacement(
				vtkShader::Vertex,
				vtk_dec, // replace the normal block
				true, // before the standard replacements
				mShaderCallback->getVSReplacement_dec(vtk_dec),
				false // only do it once
				);

	//===========
	// Modify vertex shader implementations
	//===========
	std::string vtk_impl = "//VTK::PositionVC::Impl";
	mOpenGLPolyDataMapper->AddShaderReplacement(
				vtkShader::Vertex,
				vtk_impl, // replace the normal block
				true, // before the standard replacements
				mShaderCallback->getVSReplacement_impl(vtk_impl),
				false // only do it once
				);

	//===========
	// Replace the fragment shader
	//===========
	mOpenGLPolyDataMapper->SetFragmentShaderCode(mShaderCallback->getFS().c_str());
}

Texture3DSlicerProxyImpl::Texture3DSlicerProxyImpl(SharedOpenGLContextPtr context) :
	mSharedOpenGLContext(context)
{
	mShaderCallback = ShaderCallbackPtr::New();
	mShaderCallback->mSharedOpenGLContext = mSharedOpenGLContext;

	//TODO is this unique enough? used for texture coordinates
	QString unique_string = qstring_cast(reinterpret_cast<long>(this));
	mUid = QString("Texture3DSlicerProxyImpl_%1").arg(unique_string);
	//CX_LOG_DEBUG_CHECKPOINT() << mUid;

	mTargetSpaceIsR = false;
	mActor = vtkActorPtr::New();
	//mActor->DebugOn();

	mOpenGLPolyDataMapper = vtkOpenGLPolyDataMapperPtr::New();
	mOpenGLPolyDataMapper->AddObserver(vtkCommand::UpdateShaderEvent, mShaderCallback);
//	mOpenGLPolyDataMapper->DebugOn();
//	mOpenGLPolyDataMapper->PrintSelf(std::cout, vtkIndent(3));

	mPlaneSource = vtkPlaneSourcePtr::New();
	//mPlaneSource->DebugOn();

	vtkTriangleFilterPtr triangleFilter = vtkTriangleFilterPtr::New(); //create triangle polygons from input polygons
	//triangleFilter->DebugOn();
	triangleFilter->SetInputConnection(mPlaneSource->GetOutputPort()); //in this case a Planesource

	vtkStripperPtr stripper = vtkStripperPtr::New();
	//stripper->DebugOn();
	stripper->SetInputConnection(triangleFilter->GetOutputPort());
	//stripper->Update();

	mPolyDataAlgorithm = stripper;
	mPolyDataAlgorithm->Update();

	mPolyData = mPolyDataAlgorithm->GetOutput();
	//mPolyData->DebugOn();
	//mPolyData->GetPointData()->SetNormals(NULL);


	mOpenGLPolyDataMapper->SetInputConnection(mPolyDataAlgorithm->GetOutputPort());
	mOpenGLPolyDataMapper->SetInputData(mPolyData);

	this->setShaders();

	mActor->SetMapper(mOpenGLPolyDataMapper);
}


Texture3DSlicerProxyImpl::~Texture3DSlicerProxyImpl()
{
	mImages.clear();
}

Texture3DSlicerProxyPtr Texture3DSlicerProxyImpl::New(SharedOpenGLContextPtr context)
{
	return Texture3DSlicerProxyPtr(new Texture3DSlicerProxyImpl(context));
}

vtkActorPtr Texture3DSlicerProxyImpl::getActor()
{
	return mActor;
}

std::vector<ImagePtr> Texture3DSlicerProxyImpl::getImages()
{
	return mImages;
}

void Texture3DSlicerProxyImpl::setRenderWindow(vtkRenderWindowPtr window)
{
	mCurrentRenderWindow = window;
	//mShaderCallback->mCurrentContext = mCurrentRenderWindow;
}

/*
void Texture3DSlicerProxyImpl::setShaderPath(QString shaderFile)
{
	//	mPainter->setShaderPath(shaderFile);
}
*/


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
		std::cout << "zero-size bounding box in texture slicer- ignoring" << std::endl;
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
	mPolyDataAlgorithm->Update();

//  std::cout << "createGeometryPlane update end" << std::endl;
	// each stripper->update() resets the contents of polydata, thus we must reinsert the data here.
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		updateCoordinates(i);
	}
}

bool Texture3DSlicerProxyImpl::isNewInputImages(std::vector<ImagePtr> images_raw)
{
	if (mRawImages==images_raw)
		return false;
	mRawImages = images_raw;
	return true;
}

void Texture3DSlicerProxyImpl::setImages(std::vector<ImagePtr> images_raw)
{
	//std::cout << "A2| glGetError() " << glGetError() << std::endl;
	if (!this->isNewInputImages(images_raw))
		return;

	std::vector<ImagePtr> images = processImages(images_raw);
//	mTexture->SetInputData(0, images[0]->getBaseVtkImageData());

	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		disconnect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		disconnect(mImages[i].get(), SIGNAL(transferFunctionsChanged()), this, SLOT(updateColorAttributeSlot()));
		disconnect(mImages[i].get(), SIGNAL(vtkImageDataChanged()), this, SLOT(imageChanged()));
	}

	mImages = images;

	//Clear all shaderitems before re-adding them.
	mShaderCallback->mShaderItems.clear();
	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		//TODO maybe we should upload the textures here instead???
		//we have a problem with Kaisa, because it is converted and thus not added to the viewgroup which causes it not to be uploaded
		//New Kaisa gets new uid with *_u
		if(mSharedOpenGLContext && !mSharedOpenGLContext->hasUploaded3DTexture(mImages[i]->getUid()))
		{
			mSharedOpenGLContext->uploadImage(mImages[i]);
		}

		if(mSharedOpenGLContext && mSharedOpenGLContext->hasUploaded3DTexture(mImages[i]->getUid()))
		{
			QString imageUid = mImages[i]->getUid();
			ShaderCallback::ShaderItemPtr shaderitem = ShaderCallback::ShaderItemPtr(new ShaderCallback::ShaderItem());
			shaderitem->mTextureUid = imageUid;
			shaderitem->mTexture = mSharedOpenGLContext->get3DTexture(imageUid);

			QString textureCoordinatesUid = this->generateTextureCoordinateName(imageUid);
			if(mSharedOpenGLContext->hasUploadedTextureCoordinates(textureCoordinatesUid))
			{
				shaderitem->mTextureCoordinatesUid = textureCoordinatesUid;
				shaderitem->mTextureCoordinates = mSharedOpenGLContext->getTextureCoordinates(textureCoordinatesUid);
			}
			mShaderCallback->mShaderItems.push_back(shaderitem);
		}
		else
			CX_LOG_WARNING() << "Setting image in Texture3DSlicerProxyImpl which is not uploaded to OpenGL: " << mImages[i]->getUid();

//		vtkImageDataPtr inputImage = mImages[i]->getBaseVtkImageData();
//		GPUImageDataBufferPtr dataBuffer = GPUImageBufferRepository::getInstance()->getGPUImageDataBuffer(
//			inputImage);

//		mPainter->SetVolumeBuffer(i, dataBuffer);
		//this->SetVolumeBuffer(i, dataBuffer);

		connect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		connect(mImages[i].get(), SIGNAL(transferFunctionsChanged()), this, SLOT(updateColorAttributeSlot()));
		connect(mImages[i].get(), SIGNAL(vtkImageDataChanged()), this, SLOT(imageChanged()));
		this->updateCoordinates(i);
	}
	this->updateColorAttributeSlot();

//	mPainterPolyDatamapper->RemoveAllVertexAttributeMappings();
	//for (unsigned i = 0; i < mImages.size(); ++i)
	//{
//		mPainterPolyDatamapper->MapDataArrayToMultiTextureAttribute(2 * i,
//			cstring_cast(this->getTCoordName(i)),
//			vtkDataObject::FIELD_ASSOCIATION_POINTS);
	//}

	this->setShaders(); //when number of uploaded textures changes from 0 to 1 we need to set new shaders
}

//void Texture3DSlicerProxyImpl::SetVolumeBuffer(int index, GPUImageDataBufferPtr buffer)
//{
	//this->safeIndex(index)->SetBuffer(buffer);
//}

/*
ShaderCallbackPtr Texture3DSlicerProxyImpl::safeIndex(int index)
{
	if ((int)mElement.size() <= index)
	{
		mElement.resize(index+1);
		mElement[index] = mShaderCallback;
	}
	return mElement[index];
}
*/


std::vector<ImagePtr> Texture3DSlicerProxyImpl::processImages(std::vector<ImagePtr> images_raw)
{
	/*
	if(images_raw.size() > mMaxImages)
	{
		QString errorText = QString("Texture3DSlicerProxyImpl: GPU multislicer can't handle more than %1 images. Additional images are not shown.").arg(mMaxImages);
		reportError(errorText);
		images_raw.resize(mMaxImages);
	}
	*/
	std::vector<ImagePtr> images(images_raw.size());
	for (unsigned i=0; i<images.size(); ++i)
	{
		images[i] = images_raw[i]->getUnsigned(images_raw[i]);
	}

	return images;
}


void Texture3DSlicerProxyImpl::setSliceProxy(SliceProxyPtr slicer)
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

SliceProxyPtr Texture3DSlicerProxyImpl::getSliceProxy()
{
	return mSliceProxy;
}

QString Texture3DSlicerProxyImpl::getTCoordName(int index)
{
	return  "texture"+qstring_cast(index);
}

QString Texture3DSlicerProxyImpl::generateTextureCoordinateName(QString imageUid)
{
	QString textureCoordinatesUid = QString(mUid+"_%1").arg(imageUid);

	return textureCoordinatesUid;
}

void Texture3DSlicerProxyImpl::updateCoordinates(int index)
{
	if (!mPolyData || !mSliceProxy)
		return;

	ImagePtr image = mImages[index];
	vtkImageDataPtr volume = image->getBaseVtkImageData();
	// create a bb describing the volume in physical (raw data) space
	Vector3D origin(volume->GetOrigin());
	Vector3D spacing(volume->GetSpacing());
	DoubleBoundingBox3D imageSize(volume->GetExtent());

	for (int i = 0; i < 3; ++i)
	{
		imageSize[2 * i] = origin[i] + spacing[i] * (imageSize[2 * i] - 0.5);
		imageSize[2 * i + 1] = origin[i] + spacing[i] * (imageSize[2 * i + 1] + 0.5);
	}

	// identity bb
	DoubleBoundingBox3D textureSpace(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);

	// create transform from slice space to raw data space
	Transform3D iMs = mImages[index]->get_rMd().inv() * mSliceProxy->get_sMr().inv();

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


	//vtkFloatArrayPtr TCoords = vtkFloatArray::SafeDownCast(mPolyData->GetPointData()->GetArray(cstring_cast(getTCoordName(index))));

	//vtkFloatArrayPtr TCoords;

	if (!TCoords) // create the TCoords
	{
		//CX_LOG_DEBUG_CHECKPOINT();
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
		//CX_LOG_DEBUG() << "coord: " << i << ", " << plane[i][0] << ", " << plane[i][1] << ", " << plane[i][2];
	}
	mPolyData->Modified();

	if(mSharedOpenGLContext && TCoords)
	{
		QString textureCoordinatesUid = this->generateTextureCoordinateName(image->getUid());
		mSharedOpenGLContext->upload3DTextureCoordinates(textureCoordinatesUid, TCoords);
		if(mSharedOpenGLContext->hasUploadedTextureCoordinates(textureCoordinatesUid))
		{
			ShaderCallback::ShaderItemPtr item = mShaderCallback->getShaderItem(image->getUid());
			if(item)
			{
				item->mTextureCoordinates = mSharedOpenGLContext->getTextureCoordinates(textureCoordinatesUid);
				item->mTextureCoordinatesUid = textureCoordinatesUid;
			}
		}

		/*
	//DEBUGGING
	CX_LOG_DEBUG() << "-------------- 1 TESTING DOWNLOAD!!! --------------";
	vtkImageDataPtr imageData = mSharedOpenGLContext->downloadImageFromTextureBuffer(image->getUid());
	//REQUIRE(imageData);

	vtkImageDataPtr imageData0 = image->getBaseVtkImageData();

	char* imagePtr = static_cast<char*>(imageData->GetScalarPointer());
	char* imagePtr0 = static_cast<char*>(imageData0->GetScalarPointer());

	//imageData should now be equal to image0;
	Eigen::Array3i dims(imageData->GetDimensions());
	int size = dims[0]*dims[1]*dims[2];
	for (int i = 0; i < size; ++i)
	{
//		std::cout << static_cast<int>(imagePtr[i]) << " ";
//		std::cout << (unsigned)(imagePtr[i]) << " ";
//		INFO(i);
//		REQUIRE(imagePtr[i] == imagePtr0[i]);
		if(imagePtr[i] != imagePtr0[i])
			CX_LOG_DEBUG_CHECKPOINT() << i;
	}
	std::cout << std::endl;
	CX_LOG_DEBUG() << "-------------- 2 TESTING DOWNLOAD!!! --------------";
	//DEBUGGING
	*/
	}
	else
		CX_LOG_WARNING() << "No mSharedOpenGLContext";

}

void Texture3DSlicerProxyImpl::updateColorAttributeSlot()
{
	//CX_LOG_DEBUG_CHECKPOINT() << "START";
	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		ImagePtr image = mImages[i];
		QString imageUid = image->getUid();

		ShaderCallback::ShaderItemPtr shaderItem = mShaderCallback->getShaderItem(imageUid);

		////CX_LOG_DEBUG_CHECKPOINT();
		vtkImageDataPtr inputImage = image->getBaseVtkImageData() ;

		vtkLookupTablePtr lut = image->getLookupTable2D()->getOutputLookupTable();
		lut->GetTable()->Modified();
		//GPUImageLutBufferPtr lutBuffer = GPUImageBufferRepository::getInstance()->getGPUImageLutBuffer(lut->GetTable());

		//TODO what to do if image data or lut data changes?
		QString lutUid = imageUid; //TODO or is it per view???
		mSharedOpenGLContext->uploadLUT(lutUid, lut->GetTable());
		if(mSharedOpenGLContext->hasUploaded1DTexture(lutUid))
		{
			if(shaderItem)
			{
				shaderItem->mLUT = mSharedOpenGLContext->get1DTexture(lutUid);
				shaderItem->mLUTUid = lutUid;
			}
		}

		// no lut indicates to the fragment shader that RGBA should be used
		if (inputImage->GetNumberOfScalarComponents()==1)
		{
//			mPainter->SetLutBuffer(i, lutBuffer);
			//this->SetLutBuffer(i, lutBuffer);
		}

		//Generate window, level, llr, alpha
		int scalarTypeMax = (int)inputImage->GetScalarTypeMax();
		double imin = lut->GetRange()[0];
		double imax = lut->GetRange()[1];

		float window = (float) (imax-imin) / scalarTypeMax;
		float llr = (float) mImages[i]->getLookupTable2D()->getLLR() / scalarTypeMax;
		float level = (float) imin/scalarTypeMax + window/2;
		float alpha = (float) mImages[i]->getLookupTable2D()->getAlpha();

		shaderItem->mWindow = window;
		shaderItem->mLLR = llr;
		shaderItem->mLevel = level;
		shaderItem->mAlpha = alpha;

		////CX_LOG_DEBUG_CHECKPOINT();
//		mPainter->SetColorAttribute(i, window, level, llr, alpha);
	}
	mActor->Modified();
	//CX_LOG_DEBUG_CHECKPOINT() << "END";
}

//void Texture3DSlicerProxyImpl::SetLutBuffer(int index, GPUImageLutBufferPtr buffer)
//{
//	this->safeIndex(index)->SetBuffer(buffer);
//}

void Texture3DSlicerProxyImpl::transformChangedSlot()
{
	if (mTargetSpaceIsR)
		this->resetGeometryPlane();
	this->update();
}

void Texture3DSlicerProxyImpl::imageChanged()
{
	mActor->Modified();

	for (unsigned i = 0; i < mImages .size(); ++i)
	{
		vtkImageDataPtr inputImage = mImages[i]->getBaseVtkImageData();//

//		GPUImageDataBufferPtr dataBuffer = getGPUImageBufferRepository()->getGPUImageDataBuffer(
//			inputImage);

//		mPainter->SetVolumeBuffer(i, dataBuffer);
//		this->SetVolumeBuffer(i, dataBuffer);
	}
}

void Texture3DSlicerProxyImpl::update()
{
	for (unsigned i=0; i<mImages.size(); ++i)
	{
		updateCoordinates(i);
	}
}

//#endif // WIN32
//#endif //CX_VTK_OPENGL2


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------

