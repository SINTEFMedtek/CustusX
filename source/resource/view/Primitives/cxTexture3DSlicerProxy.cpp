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


/*
 * sscTexture3DSlicerProxyImpl.cpp
 *
 *  Created on: Oct 13, 2011
 *      Author: christiana
 */

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


//#include "cxTextureSlicePainter.h"
//#ifndef CX_VTK_OPENGL2
//#include <vtkPainterPolyDataMapper.h>
//#endif

//---------------------------------------------------------
namespace cx
{
//---------------------------------------------------------

//#ifdef WIN32
//#ifdef CX_VTK_OPENGL2
//#if defined(CX_VTK_OPENGL2) || defined(WIN32)

//Texture3DSlicerProxyPtr Texture3DSlicerProxy::New()
//{
//	return Texture3DSlicerProxyPtr(new Texture3DSlicerProxy());
//}

//bool Texture3DSlicerProxy::isSupported(vtkRenderWindowPtr window)
//{
//	return false;
//}

//#else

bool Texture3DSlicerProxy::isSupported(vtkRenderWindowPtr window)
{
	return true;
//	vtkOpenGLRenderWindow *context = vtkOpenGLRenderWindow::SafeDownCast(window);
//	bool success =  context && TextureSlicePainter::LoadRequiredExtensions(context->GetExtensionManager());
//	return success;
}

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
	mActor = vtkActorPtr::New();
//	mPainter = TextureSlicePainterPtr::New();
//	mPainterPolyDatamapper = vtkPainterPolyDataMapperPtr::New();
	mOpenGLPolyDataMapper = vtkOpenGLPolyDataMapperPtr::New();

	mPlaneSource = vtkPlaneSourcePtr::New();

	vtkTriangleFilterPtr triangleFilter = vtkTriangleFilterPtr::New(); //create triangle polygons from input polygons
	triangleFilter->SetInputConnection(mPlaneSource->GetOutputPort()); //in this case a Planesource

	vtkStripperPtr stripper = vtkStripperPtr::New();
	stripper->SetInputConnection(triangleFilter->GetOutputPort());
//	stripper->Update();
	mPolyDataAlgorithm = stripper;
	mPolyDataAlgorithm->Update();

	mPolyData = mPolyDataAlgorithm->GetOutput();
	mPolyData->GetPointData()->SetNormals(NULL);

//	mTexture = vtkTexturePtr::New();

//	mPainter->SetDelegatePainter(mPainterPolyDatamapper->GetPainter());
//	mPainterPolyDatamapper->SetPainter(mPainter);
//	mPainterPolyDatamapper->SetInputData(mPolyData);
//	mActor->SetMapper(mPainterPolyDatamapper);

	mOpenGLPolyDataMapper->SetInputConnection(mPolyDataAlgorithm->GetOutputPort());
	mOpenGLPolyDataMapper->SetInputData(mPolyData);

	//===========
	// Modify vertex shader declarations
	//===========
	mOpenGLPolyDataMapper->AddShaderReplacement(
				vtkShader::Vertex,
				"//VTK::PositionVC::Dec", // replace the normal block
				true, // before the standard replacements
				"//VTK::PositionVC::Dec\n" // we still want the default
				"attribute vec3 COLOR_VSIN;\n"
				"attribute vec3 TEXTURE_COORDINATE_VSIN;\n"
				"varying vec3 COLOR_VSOUT;\n"
				"varying vec3 TEXTURE_COORDINATE_VSOUT;\n",
				false // only do it once
				);

	//===========
	// Modify vertex shader implementations
	//===========
	mOpenGLPolyDataMapper->AddShaderReplacement(
				vtkShader::Vertex,
				"//VTK::PositionVC::Impl", // replace the normal block
				true, // before the standard replacements
				"//VTK::PositionVC::Impl\n" // we still want the default
				"COLOR_VSOUT = COLOR_VSIN;\n"
				"TEXTURE_COORDINATE_VSOUT = TEXTURE_COORDINATE_VSIN;\n",
				false // only do it once
				);

	//===========
	// Replace the fragment shader
	//===========
	std::string fragment_shader =
		"//VTK::System::Dec\n"  // always start with this line
		"//VTK::Output::Dec\n"  // always have this line in your FS
		"in vec3 COLOR_VSOUT;\n"
		"in vec3 TEXTURE_COORDINATE_VSOUT;\n"
		"uniform sampler3D my_texture;\n"
		"out vec4 color;\n"
		"void main () {\n"
		"	color = texture(my_texture, TEXTURE_COORDINATE_VSOUT);\n"
		"}\n";
	mOpenGLPolyDataMapper->SetFragmentShaderCode(fragment_shader.c_str());

	mActor->SetMapper(mOpenGLPolyDataMapper.Get());
//	mActor->SetTexture(mTexture);
}

////copied from TextureSlicePainter
//QString Texture3DSlicerProxyImpl::loadShaderFile()
//{
//	QString mShaderPath = DataLocations::findConfigFolder("/shaders");
//	QString filepath = mShaderPath + "/cxOverlay2D_frag.glsl";
//	QFile fp(filepath);
//	if (fp.exists())
//	{
//		fp.open(QFile::ReadOnly);
//		QTextStream shaderfile(&fp);
//		return shaderfile.readAll();
//	}
//	else
//	{
//		std::cout << "TextureSlicer can't read shaderfile [" << fp.fileName() << "]" << std::endl;
//	}
//	return "";
//}

////copied from TextureSlicePainter
//QString Texture3DSlicerProxyImpl::replaceShaderSourceMacros(QString shaderSource)
//{
//	// set constant layers
////	int layers = this->mElement.size();
//	int layers = 1;//temp hack
//	shaderSource = shaderSource.replace("${LAYERS}", QString("%1").arg(layers));

//	// fill function vec4 sampleLut(in int index, in float idx)
//	QString element = "\tif (index==%1) return texture1D(lut[%1], idx);\n";
//	QString sampleLutContent;
//	for (unsigned i=0; i<layers; ++i)
//		sampleLutContent += element.arg(i);
//	shaderSource = shaderSource.replace("${SAMPLE_LUT_CONTENT}", sampleLutContent);

//	return shaderSource;
//}

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

void Texture3DSlicerProxyImpl::setShaderPath(QString shaderFile)
{
//	mPainter->setShaderPath(shaderFile);
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
	mPolyDataAlgorithm->Update();
//	mPolyData->Update();
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

	for (unsigned i = 0; i < mImages .size(); ++i)
	{
		vtkImageDataPtr inputImage = mImages[i]->getBaseVtkImageData();

		GPUImageDataBufferPtr dataBuffer = GPUImageBufferRepository::getInstance()->getGPUImageDataBuffer(
			inputImage);

//		mPainter->SetVolumeBuffer(i, dataBuffer);
		this->SetVolumeBuffer(i, dataBuffer);

		connect(mImages[i].get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		connect(mImages[i].get(), SIGNAL(transferFunctionsChanged()), this, SLOT(updateColorAttributeSlot()));
		connect(mImages[i].get(), SIGNAL(vtkImageDataChanged()), this, SLOT(imageChanged()));
		this->updateCoordinates(i);
	}
	this->updateColorAttributeSlot();

//	mPainterPolyDatamapper->RemoveAllVertexAttributeMappings();
	for (unsigned i = 0; i < mImages.size(); ++i)
	{
//		mPainterPolyDatamapper->MapDataArrayToMultiTextureAttribute(2 * i,
//			cstring_cast(this->getTCoordName(i)),
//			vtkDataObject::FIELD_ASSOCIATION_POINTS);
	}
}
void Texture3DSlicerProxyImpl::SetVolumeBuffer(int index, GPUImageDataBufferPtr buffer)
{
	this->safeIndex(index)->SetBuffer(buffer);
}

ShaderCallbackPtr Texture3DSlicerProxyImpl::safeIndex(int index)
{
	if ((int)mElement.size() <= index)
	{
		mElement.resize(index+1);
		vtkSmartPointer<ShaderCallback> shaderCallback = vtkSmartPointer<ShaderCallback>::New();
		shaderCallback->mIndex = index;
//		mElement[index] = ShaderCallback(index);
		mElement[index] = shaderCallback;

		// Setup a callback to change some uniforms
//		VTK_CREATE(ShaderCallback, mElement[index]);
//	    mElement[index]->Renderer = renderer.Get();


		//TODO: Need vtkRenderer?
		mOpenGLPolyDataMapper->AddObserver(vtkCommand::UpdateShaderEvent, shaderCallback);
		mOpenGLPolyDataMapper->AddObserver(vtkCommand::EndEvent, shaderCallback);
	}
	return mElement[index];
}


std::vector<ImagePtr> Texture3DSlicerProxyImpl::processImages(std::vector<ImagePtr> images_raw)
{
	if(images_raw.size() > mMaxImages)
	{
		QString errorText = QString("Texture3DSlicerProxyImpl: GPU multislicer can't handle more than %1 images. Additional images are not shown.").arg(mMaxImages);
		reportError(errorText);
		images_raw.resize(mMaxImages);
	}

	std::vector<ImagePtr> images(images_raw.size());
	for (unsigned i=0; i<images.size(); ++i)
		images[i] = images_raw[i]->getUnsigned(images_raw[i]);

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

QString Texture3DSlicerProxyImpl::getTCoordName(int index)
{
	return  "texture"+qstring_cast(index);
}

void Texture3DSlicerProxyImpl::updateCoordinates(int index)
{
	if (!mPolyData || !mSliceProxy)
		return;

	vtkImageDataPtr volume = mImages[index]->getBaseVtkImageData();
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
//		CX_LOG_DEBUG() << "coord: " << i << ", " << plane[i][0] << ", " << plane[i][1] << ", " << plane[i][2];
	}

	mPolyData->Modified();

}

void Texture3DSlicerProxyImpl::updateColorAttributeSlot()
{
	for (unsigned i = 0; i < mImages.size(); ++i)
	{
		vtkImageDataPtr inputImage = mImages[i]->getBaseVtkImageData() ;

		vtkLookupTablePtr lut = mImages[i]->getLookupTable2D()->getOutputLookupTable();
		lut->GetTable()->Modified();
		GPUImageLutBufferPtr lutBuffer = GPUImageBufferRepository::getInstance()->getGPUImageLutBuffer(lut->GetTable());

		// no lut indicates to the fragment shader that RGBA should be used
		if (inputImage->GetNumberOfScalarComponents()==1)
		{
//			mPainter->SetLutBuffer(i, lutBuffer);
			this->SetLutBuffer(i, lutBuffer);
		}

		int scalarTypeMax = (int)inputImage->GetScalarTypeMax();
		double imin = lut->GetRange()[0];
		double imax = lut->GetRange()[1];

		float window = (float) (imax-imin) / scalarTypeMax;
		float llr = (float) mImages[i]->getLookupTable2D()->getLLR() / scalarTypeMax;
		float level = (float) imin/scalarTypeMax + window/2;
		float alpha = (float) mImages[i]->getLookupTable2D()->getAlpha();

//		mPainter->SetColorAttribute(i, window, level, llr, alpha);
	}
	mActor->Modified();
}

void Texture3DSlicerProxyImpl::SetLutBuffer(int index, GPUImageLutBufferPtr buffer)
{
	this->safeIndex(index)->SetBuffer(buffer);
}

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

		GPUImageDataBufferPtr dataBuffer = GPUImageBufferRepository::getInstance()->getGPUImageDataBuffer(
			inputImage);

//		mPainter->SetVolumeBuffer(i, dataBuffer);
		this->SetVolumeBuffer(i, dataBuffer);
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

