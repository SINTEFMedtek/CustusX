/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxOpenGLShaders.h"

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

void Texture3DSlicerProxyImpl::generateAndSetShaders()
{
	//===========
	// Modify vertex shader declarations
	//===========
	std::string vtk_dec = "//VTK::PositionVC::Dec";
	mOpenGLPolyDataMapper->AddShaderReplacement(
	  vtkShader::Vertex,
	  vtk_dec, // replace the normal block
	  true, // before the standard replacements
	  getVSReplacement_dec(vtk_dec, mShaderCallback->getNumberOfUploadedTextures()),
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
	  getVSReplacement_impl(vtk_impl, mShaderCallback->getNumberOfUploadedTextures()),
	  false // only do it once
	);

	//===========
	// Replace the fragment shader
	//===========
	mOpenGLPolyDataMapper->SetFragmentShaderCode(getFS(mShaderCallback->getNumberOfUploadedTextures()).c_str());
}

Texture3DSlicerProxyImpl::Texture3DSlicerProxyImpl(SharedOpenGLContextPtr context) :
	mSharedOpenGLContext(context)
{
	mShaderCallback = ShaderCallbackPtr::New();

	QString unique_string = qstring_cast(reinterpret_cast<long>(this));
	mUid = QString("Texture3DSlicerProxyImpl_%1").arg(unique_string);
	mTargetSpaceIsR = false;

	mActor = vtkActorPtr::New();

	mOpenGLPolyDataMapper = vtkOpenGLPolyDataMapperPtr::New();
	mOpenGLPolyDataMapper->AddObserver(vtkCommand::UpdateShaderEvent, mShaderCallback);

	mPlaneSource = vtkPlaneSourcePtr::New();

	vtkTriangleFilterPtr triangleFilter = vtkTriangleFilterPtr::New(); //create triangle polygons from input polygons
	triangleFilter->SetInputConnection(mPlaneSource->GetOutputPort()); //in this case a Planesource

	vtkStripperPtr stripper = vtkStripperPtr::New();
	stripper->SetInputConnection(triangleFilter->GetOutputPort());

	mPolyDataAlgorithm = stripper;
	mPolyDataAlgorithm->Update();
	mPolyData = mPolyDataAlgorithm->GetOutput();

	mOpenGLPolyDataMapper->SetInputConnection(mPolyDataAlgorithm->GetOutputPort());
	mOpenGLPolyDataMapper->SetInputData(mPolyData);

	this->generateAndSetShaders();

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
}

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
				{
					pp_s.push_back(sMd.coord(bb_d.corner(x,y,x)));
				}

		mBB_s = DoubleBoundingBox3D::fromCloud(pp_s);
		mBB_s[4] = 0;
		mBB_s[5] = 0;
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
		CX_LOG_DEBUG() << "zero-size bounding box in texture slicer- ignoring";
		return;
	}

	createGeometryPlane(p1, p2, origin);
}

void Texture3DSlicerProxyImpl::createGeometryPlane( Vector3D point1_s,  Vector3D point2_s, Vector3D origin_s )
{
	mPlaneSource->SetPoint1( point1_s.begin() );
	mPlaneSource->SetPoint2( point2_s.begin() );
	mPlaneSource->SetOrigin( origin_s.begin() );

	mPolyDataAlgorithm->Update();

	updateAndUploadCoordinates();
}

bool Texture3DSlicerProxyImpl::isNewInputImages(std::vector<ImagePtr> images_raw)
{
	if (mRawImages==images_raw)
	{
		return false;
	}

	mRawImages = images_raw;
	return true;
}

/*
void Texture3DSlicerProxyImpl::setImages(std::vector<ImagePtr> images_raw)
{
	if (!this->isNewInputImages(images_raw))
	{
		return;
	}

	std::vector<ImagePtr> images = processImages(images_raw);

	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		disconnect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		disconnect(mImages[i].get(), SIGNAL(transferFunctionsChanged()), this, SLOT(updateColorAttributeSlot()));
		disconnect(mImages[i].get(), SIGNAL(vtkImageDataChanged()), this, SLOT(imageChanged()));
	}

	mImages = images;

	//Clear all shaderitems before re-adding them.
	mShaderCallback->clearShaderItems();

	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		//TODO maybe we should upload the textures here instead???
		//we have a problem with Kaisa, because it is converted and thus not added to the viewgroup which causes it not to be uploaded
		//New Kaisa gets new uid with *_u
		QString imageUid = mImages[i]->getUid();

		if(mSharedOpenGLContext && !mSharedOpenGLContext->hasUploaded3DTexture(imageUid))
		{
			mSharedOpenGLContext->uploadImage(mImages[i]);
		}

		if(mSharedOpenGLContext && mSharedOpenGLContext->hasUploaded3DTexture(imageUid))
		{
			ShaderCallback::ShaderItemPtr shaderitem = ShaderCallback::ShaderItemPtr(new ShaderCallback::ShaderItem());
			shaderitem->mTextureUid = imageUid;
			shaderitem->mTexture = mSharedOpenGLContext->get3DTexture(imageUid);
			QString textureCoordinatesUid = this->generateTextureCoordinateName(imageUid);

			if(mSharedOpenGLContext->hasUploadedTextureCoordinates(textureCoordinatesUid))
			{
				shaderitem->mTextureCoordinatesUid = textureCoordinatesUid;
				shaderitem->mTextureCoordinates = mSharedOpenGLContext->getTextureCoordinates(textureCoordinatesUid);
			}

			mShaderCallback->add(shaderitem);
		}
		else
		{
			CX_LOG_WARNING() << "Setting image in Texture3DSlicerProxyImpl which is not uploaded to OpenGL: " << imageUid;
		}

		connect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		connect(mImages[i].get(), SIGNAL(transferFunctionsChanged()), this, SLOT(updateColorAttributeSlot()));
		connect(mImages[i].get(), SIGNAL(vtkImageDataChanged()), this, SLOT(imageChanged()));
		this->updateCoordinates(i);
	}

	this->updateColorAttributeSlot();

	this->setShaders(); //when number of uploaded textures changes from 0 to 1 we need to set new shader code
}
*/

void Texture3DSlicerProxyImpl::uploadImagesToSharedContext(std::vector<ImagePtr> images, SharedOpenGLContextPtr sharedOpenGLContext, ShaderCallbackPtr shaderCallback) const
{
	for (unsigned i = 0; i < images.size(); ++i)
	{
		//TODO:
		//we have a problem with Kaisa, because it is converted and thus not added to the viewgroup which causes it not to be uploaded
		//New Kaisa gets new uid with *_u
		QString imageUid = images[i]->getUid();

		if(sharedOpenGLContext && !sharedOpenGLContext->hasUploadedImage(imageUid))
		{
			sharedOpenGLContext->uploadImage(images[i]);
		}

		if(sharedOpenGLContext && sharedOpenGLContext->hasUploadedImage(imageUid))
		{
			ShaderCallback::ShaderItemPtr shaderitem = ShaderCallback::ShaderItemPtr(new ShaderCallback::ShaderItem());
			shaderitem->mTextureUid = imageUid;
			shaderitem->mTexture = sharedOpenGLContext->get3DTextureForImage(imageUid);
			QString textureCoordinatesUid = this->generateTextureCoordinateName(imageUid);

			if(sharedOpenGLContext->hasUploadedTextureCoordinates(textureCoordinatesUid))
			{
				shaderitem->mTextureCoordinatesUid = textureCoordinatesUid;
				shaderitem->mTextureCoordinates = sharedOpenGLContext->getTextureCoordinates(textureCoordinatesUid);
			}

			shaderCallback->add(shaderitem);
		}
		else
		{
			CX_LOG_WARNING() << "Setting image in Texture3DSlicerProxyImpl which is not uploaded to OpenGL: " << imageUid;
		}
	}
}

std::vector<ImagePtr> elementsInAButNotInB(std::vector<ImagePtr> A, std::vector<ImagePtr> B)
{
	std::vector<ImagePtr> C;
	for(int i=0; i<A.size(); ++i)
	{
		std::vector<ImagePtr>::iterator it = std::find(B.begin(), B.end(), A[i]);

		if(it == B.end())
		{
			CX_LOG_DEBUG() << "Going to delete: " << A[i]->getUid();
			C.push_back(A[i]);
		}
	}

	return C;
}

void Texture3DSlicerProxyImpl::updateAndUploadImages(std::vector<ImagePtr> new_images_raw)
{
	//only unsigned images are supported on the gpu
	std::vector<ImagePtr> unsigned_images = convertToUnsigned(new_images_raw);

	//removing unused textures from the gpu
	std::vector<ImagePtr> to_be_deleted = elementsInAButNotInB(mImages, unsigned_images);
	for(int i=0; i<to_be_deleted.size(); ++i)
	{
		mSharedOpenGLContext->delete3DTextureForImage(to_be_deleted[i]->getUid());
		mSharedOpenGLContext->delete1DTextureForLUT(to_be_deleted[i]->getUid());
	}

	//setup signals and slots
	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		disconnect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		disconnect(mImages[i].get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionChangedSlot()));
		disconnect(mImages[i].get(), SIGNAL(vtkImageDataChanged(QString)), this, SLOT(uploadChangedImage(QString)));
	}

	mImages = unsigned_images;

	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		connect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		connect(mImages[i].get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionChangedSlot()));
		connect(mImages[i].get(), SIGNAL(vtkImageDataChanged(QString)), this, SLOT(uploadChangedImage(QString)));
	}

	//upload any new images to the gpu
	this->uploadImagesToSharedContext(mImages, mSharedOpenGLContext, mShaderCallback);
}

void Texture3DSlicerProxyImpl::setImages(std::vector<ImagePtr> images_raw)
{
	if (!this->isNewInputImages(images_raw))
	{
		return;
	}

	//Clear all shaderitems before re-adding them.
	mShaderCallback->clearShaderItems();

	this->updateAndUploadImages(images_raw);

	this->updateAndUploadCoordinates();

	this->updateAndUploadColorAttribute();

	//when number of uploaded textures changes from 0 to 1 we need to set new shader code
	this->generateAndSetShaders();
}

std::vector<ImagePtr> Texture3DSlicerProxyImpl::convertToUnsigned(std::vector<ImagePtr> images_raw)
{
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
	{
		disconnect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(transformChangedSlot()));
	}

	mSliceProxy = slicer;

	if (mSliceProxy)
	{
		connect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this,	SLOT(transformChangedSlot()));

		this->updateAndUploadCoordinates();
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

QString Texture3DSlicerProxyImpl::generateTextureCoordinateName(QString imageUid) const
{
	QString textureCoordinatesUid = QString(mUid+"_%1").arg(imageUid);
	return textureCoordinatesUid;
}

void Texture3DSlicerProxyImpl::uploadTextureCoordinatesToSharedContext(QString image_uid, vtkFloatArrayPtr textureCoordinates, SharedOpenGLContextPtr sharedOpenGLContext, ShaderCallbackPtr shaderCallback) const
{
	if(sharedOpenGLContext && textureCoordinates)
	{
		QString textureCoordinatesUid = this->generateTextureCoordinateName(image_uid);
		sharedOpenGLContext->upload3DTextureCoordinates(textureCoordinatesUid, textureCoordinates);

		if(sharedOpenGLContext->hasUploadedTextureCoordinates(textureCoordinatesUid))
		{
			ShaderCallback::ShaderItemPtr item = shaderCallback->getShaderItem(image_uid);

			if(item)
			{
				item->mTextureCoordinates = sharedOpenGLContext->getTextureCoordinates(textureCoordinatesUid);
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
	{
		CX_LOG_WARNING() << "No mSharedOpenGLContext";
	}
}

void Texture3DSlicerProxyImpl::updateAndUploadCoordinates()
{
	if (!mPolyData || !mSliceProxy)
	{
		return;
	}

	for (unsigned i = 0; i < mImages.size(); ++i)
	{

		ImagePtr image = mImages[i];
		vtkImageDataPtr volume = image->getBaseVtkImageData();

		// create a bb describing the volume in physical (raw data) space
		Vector3D origin(volume->GetOrigin());
		Vector3D spacing(volume->GetSpacing());
		DoubleBoundingBox3D imageSize(volume->GetExtent());

		for (int j = 0; j < 3; ++j)
		{
			imageSize[2 * j] = origin[j] + spacing[j] * (imageSize[2 * j] - 0.5);
			imageSize[2 * j + 1] = origin[j] + spacing[j] * (imageSize[2 * j + 1] + 0.5);
		}

		// identity bb
		DoubleBoundingBox3D textureSpace(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);

		// create transform from slice space to raw data space
		Transform3D iMs = image->get_rMd().inv() * mSliceProxy->get_sMr().inv();

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

		for (unsigned j = 0; j < plane.size(); ++j)
		{
			plane[j] = nMs.coord(plane[j]);
		}

		if (!mTextureCoordinates)
		{
			mTextureCoordinates = vtkFloatArrayPtr::New();
			mTextureCoordinates->SetNumberOfComponents(3);
			mTextureCoordinates->Allocate(4 * 3);
			mTextureCoordinates->InsertNextTuple3(0.0, 0.0, 0.0);
			mTextureCoordinates->InsertNextTuple3(0.0, 0.0, 0.0);
			mTextureCoordinates->InsertNextTuple3(0.0, 0.0, 0.0);
			mTextureCoordinates->InsertNextTuple3(0.0, 0.0, 0.0);
			mTextureCoordinates->SetName(cstring_cast(getTCoordName(i)));
			mPolyData->GetPointData()->AddArray(mTextureCoordinates);
		}

		for (unsigned j = 0; j < plane.size(); ++j)
		{
			mTextureCoordinates->SetTuple3(j, plane[j][0], plane[j][1], plane[j][2]);
		}

		mPolyData->Modified();

		this->uploadTextureCoordinatesToSharedContext(image->getUid(), mTextureCoordinates, mSharedOpenGLContext, mShaderCallback);
	}
}

void Texture3DSlicerProxyImpl::uploadColorAttributesToSharedContext(QString imageUid, float llr, vtkLookupTablePtr lut, float window, float level, float alpha, SharedOpenGLContextPtr sharedOpenGLContext, ShaderCallbackPtr shaderCallback) const
{
	QString lutUid = imageUid;
	ShaderCallback::ShaderItemPtr shaderItem = shaderCallback->getShaderItem(lutUid);

	sharedOpenGLContext->uploadLUT(lutUid, lut->GetTable());

	if(sharedOpenGLContext->hasUploadedLUT(lutUid))
	{
		if(shaderItem)
		{
			shaderItem->mLUT = sharedOpenGLContext->get1DTextureForLUT(lutUid);
			shaderItem->mLUTUid = lutUid;
		}
	}

	shaderItem->mWindow = window;
	shaderItem->mLLR = llr;
	shaderItem->mLevel = level;
	shaderItem->mAlpha = alpha;
}

void Texture3DSlicerProxyImpl::updateAndUploadColorAttribute()
{
	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		ImagePtr image = mImages[i];
		QString imageUid = image->getUid();

		vtkImageDataPtr inputImage = image->getBaseVtkImageData() ;
		vtkLookupTablePtr lut = image->getLookupTable2D()->getOutputLookupTable();

		//TODO this is a HACK
		//for some reason the lut of the image is not marked as modified for the images where the transferfunction is actually modified
		//that is why we set it here on all luts
		//this results in that all luts will be uploaded to gpu every time someone changes any transferfunction...
		//real fix would be to mark the images lut as modified when it actually happens...
		//lut->GetTable()->Modified();

		//Generate window, level, llr, alpha
		int scalarTypeMax = (int)inputImage->GetScalarTypeMax();
		double imin = lut->GetRange()[0];
		double imax = lut->GetRange()[1];
		float window = (float) (imax-imin) / scalarTypeMax;
		float llr = (float) mImages[i]->getLookupTable2D()->getLLR() / scalarTypeMax;
		float level = (float) imin/scalarTypeMax + window/2;
		float alpha = (float) mImages[i]->getLookupTable2D()->getAlpha();

		this->uploadColorAttributesToSharedContext(imageUid, llr, lut, window, level, alpha, mSharedOpenGLContext, mShaderCallback);
	}

	mActor->Modified();
}

void Texture3DSlicerProxyImpl::transformChangedSlot()
{
	if (mTargetSpaceIsR)
	{
		this->resetGeometryPlane();
	}

	this->updateAndUploadCoordinates();
}

void Texture3DSlicerProxyImpl::transferFunctionChangedSlot()
{
	this->updateAndUploadColorAttribute();
}

void Texture3DSlicerProxyImpl::uploadChangedImage(QString uid)
{
	mActor->Modified();

	for (unsigned i = 0; i < mImages .size(); ++i)
	{
		ImagePtr image = mImages[i];
		if(mImages[i]->getUid() == uid)
		{
			this->mSharedOpenGLContext->uploadImage(image);
		}
	}

}


} //namespace cx

