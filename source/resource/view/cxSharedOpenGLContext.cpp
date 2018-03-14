/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

//needed on windows to where <windows.h> is included
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "cxSharedOpenGLContext.h"

#include <vtkOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <vtkTextureObject.h>
#include <vtkOpenGLBufferObject.h>
#include <vtkImageData.h>
#include <vtkSetGet.h>
#include <vtkFloatArray.h>
#include <vtkPixelBufferObject.h>
#include <vtkUnsignedCharArray.h>
#include <vtkTextureUnitManager.h>

#include "cxGLHelpers.h"
#include "cxLogger.h"
#include "cxImage.h"
#include "cxUtilHelpers.h"
#include "cxSettings.h"

namespace cx
{

bool SharedOpenGLContext::isValid(vtkOpenGLRenderWindowPtr opengl_renderwindow, bool print)
{
	bool valid = true;

	if(!opengl_renderwindow)
	{
		valid=false;
	}

	if(print)
	{
		CX_LOG_DEBUG() <<  "\n==== START SharedContext ====";
		//NB! opengl_renderwindow->SupportsOpenGL() creates a new context even if you have one, and this seg fault on render in vtkCocoaRenderWindow::CreateGLContext().
		//CX_LOG_DEBUG() <<  "SupportsOpenGL: " << opengl_renderwindow->SupportsOpenGL();
		CX_LOG_DEBUG() <<  "IsDrawable: " << opengl_renderwindow->IsDrawable();
		CX_LOG_DEBUG() <<  "Context support for open gl core 3.2: " << (vtkOpenGLRenderWindow::GetContextSupportsOpenGL32() ? "true" : "false");
		CX_LOG_DEBUG() <<  "Context was created at: " << opengl_renderwindow->GetContextCreationTime();
		const char *renderlib = vtkRenderWindow::GetRenderLibrary();
		CX_LOG_DEBUG() <<  "GetRenderLibrary: " << ((renderlib!=0) ? std::string(renderlib) : "NOT FOUND");
		CX_LOG_DEBUG() <<  "vtkOpenGLRenderWindow:";
		CX_LOG_DEBUG() <<  "==== END SharedContext ====\n";
	}

	report_gl_error();
	return valid;
}

SharedOpenGLContext::SharedOpenGLContext(vtkOpenGLRenderWindowPtr sharedContext) :
	mContext(sharedContext)
{
}

SharedOpenGLContext::~SharedOpenGLContext()
{
}

bool SharedOpenGLContext::hasUploadedImage(QString image_uid) const
{
	return m3DTextureObjects.count(image_uid);
}

vtkTextureObjectPtr SharedOpenGLContext::get3DTextureForImage(QString image_uid) const
{
	vtkTextureObjectPtr retval;

	if(hasUploadedImage(image_uid))
	{
		retval = m3DTextureObjects.at(image_uid).first;
	}
//	else
//	{
//		CX_LOG_ERROR() << "get3DTexture failed, seems 3D texture is not uploaded";
//	}

	return retval;
}

bool SharedOpenGLContext::delete3DTextureForImage(QString image_uid)
{
	bool success = false;
	vtkTextureObjectPtr texture = this->get3DTextureForImage(image_uid);
	std::map<QString, std::pair<vtkTextureObjectPtr, unsigned long> >::iterator it = m3DTextureObjects.find(image_uid);

	if(it != m3DTextureObjects.end())
	{
		m3DTextureObjects.erase(it);
		texture->ReleaseGraphicsResources(mContext);
		success = true;
	}

	return success;
}

vtkImageDataPtr SharedOpenGLContext::downloadImageFromTextureBuffer(QString image_uid)
{
	vtkPixelBufferObjectPtr pixelBuffer;
	vtkImageDataPtr imageData = vtkImageDataPtr::New();
	vtkTextureObjectPtr texture = this->get3DTextureForImage(image_uid);

	if(texture)
	{
		pixelBuffer = texture->Download();
		int dataType = texture->GetVTKDataType();
		unsigned width = texture->GetWidth();
		unsigned height = texture->GetHeight();
		unsigned depth = texture->GetDepth();
		unsigned dims[3] = {width, height, depth};
		int numComps = texture->GetComponents();
		vtkIdType increments[3] = {0, 0, 0};
		imageData->SetExtent(0, width-1, 0, height-1, 0, depth-1);
		//imageData->SetSpacing(1, 1, 1);
		imageData->AllocateScalars(dataType, numComps);
		void* data = imageData->GetScalarPointer();
		pixelBuffer->Download3D(dataType, data, dims, numComps, increments);
	}
	else
	{
		CX_LOG_ERROR() << "SharedOpenGLContext::downloadImageFromTextureBuffer failed";
	}

	return imageData;
}

bool SharedOpenGLContext::makeCurrent() const
{
	mContext->MakeCurrent();
	return mContext->IsCurrent();
}

int SharedOpenGLContext::getNumberOfTexturesInUse() const
{
	int texture_units_in_use = 0;
	texture_units_in_use += m3DTextureObjects.size();
	texture_units_in_use += m1DTextureObjects.size();
	return texture_units_in_use;
}

bool SharedOpenGLContext::uploadImage(ImagePtr image)
{
	report_gl_error();
	bool success = false;

	if(!image)
	{
		CX_LOG_ERROR() << "Cannot upload en empty image as a 3D texture";
		return success;
	}

	unsigned long new_modified_time = image->getBaseVtkImageData()->GetMTime();
	std::map<QString, std::pair<vtkTextureObjectPtr, unsigned long> >::iterator it = m3DTextureObjects.find(image->getUid());
	bool uploaded_but_modified = (it != m3DTextureObjects.end() && (it->second.second != new_modified_time) );
	bool not_uploaded = it == m3DTextureObjects.end();
	bool uploade_and_not_modified = (it != m3DTextureObjects.end() && (it->second.second == new_modified_time) );

	if( uploaded_but_modified || not_uploaded)
	{
		//upload new data to gpu
		vtkImageDataPtr vtkImageData = image->getBaseVtkImageData();
		int* dims = vtkImageData->GetDimensions();
		int dataType = vtkImageData->GetScalarType();
		int numComps = vtkImageData->GetNumberOfScalarComponents();
		int coordinate[3] = {dims[0]/2, dims[1]/2, dims[2]/2};
		void* data = vtkImageData->GetScalarPointer();
		//CX_LOG_DEBUG() << "dims: " << dims[0] << " " << dims[1] << " " << dims[2] << " " << " scalarType: " << vtkImageData->GetScalarTypeAsString() << " numComps: " << numComps;
		//CX_LOG_DEBUG() << "data in the middle 1 " << vtkImageData->GetScalarComponentAsFloat(dims[0]/2, dims[1]/2, dims[2]/2, 0);
		//CX_LOG_DEBUG() << "data in the middle 2 " << (int)((unsigned char*)vtkImageData->GetScalarPointer(coordinate))[0];

		//int size = dims[0]*dims[1]*dims[2]*numComps;
		//CX_LOG_DEBUG() << "data in the middle 3 " << (int)((reinterpret_cast<unsigned char*>(data)[size/2+1]));
		//for(int i=0; i<size; i=i+150)
		//	std::cout << (int)((reinterpret_cast<unsigned char*>(data)[i]));
		//std::cout << std::endl;

		vtkTextureObjectPtr texture_object = this->get3DTextureForImage(image->getUid());
		if(!texture_object) //not uploaded
		{
			texture_object = vtkTextureObjectPtr::New();
			//CX_LOG_DEBUG() << "create new texture_object";
		}

		success = this->create3DTextureObject(texture_object, dims[0], dims[1], dims[2], dataType, numComps, data, mContext);
		m3DTextureObjects[image->getUid()] = std::make_pair(texture_object, vtkImageData->GetMTime());
	}
	else if(uploade_and_not_modified)
	{
		//do nothing
		success = true;
	}

	/*
		report_gl_error();
		vtkTextureObjectPtr texture_object;

		//TODO refactor to be similar to uploadLUT
		if(!m3DTextureObjects.count(image->getUid()))
		{
			vtkImageDataPtr vtkImageData = image->getBaseVtkImageData();
			int* dims = vtkImageData->GetDimensions();
			int dataType = vtkImageData->GetScalarType();
			int numComps = vtkImageData->GetNumberOfScalarComponents();
			int coordinate[3] = {dims[0]/2, dims[1]/2, dims[2]/2};
			void* data = vtkImageData->GetScalarPointer();
			//CX_LOG_DEBUG() << "dims: " << dims[0] << " " << dims[1] << " " << dims[2] << " " << " scalarType: " << vtkImageData->GetScalarTypeAsString() << " numComps: " << numComps;
			//CX_LOG_DEBUG() << "data in the middle 1 " << vtkImageData->GetScalarComponentAsFloat(dims[0]/2, dims[1]/2, dims[2]/2, 0);
			//CX_LOG_DEBUG() << "data in the middle 2 " << (int)((unsigned char*)vtkImageData->GetScalarPointer(coordinate))[0];

			//int size = dims[0]*dims[1]*dims[2]*numComps;
			//CX_LOG_DEBUG() << "data in the middle 3 " << (int)((reinterpret_cast<unsigned char*>(data)[size/2+1]));
			//for(int i=0; i<size; i=i+150)
			//	std::cout << (int)((reinterpret_cast<unsigned char*>(data)[i]));
			//std::cout << std::endl;

			texture_object = this->create3DTextureObject(dims[0], dims[1], dims[2], dataType, numComps, data, mContext);
			m3DTextureObjects[image->getUid()] = std::make_pair(texture_object, vtkImageData->GetMTime());
			//m3DTextureObjects_new[image->getUid()] = std::pair<vtkTextureObjectPtr, unsigned long int>(texture_object, texture_object->GetMTime());
			success = true;
		}
		else
		{
			//TODO Update image data?
			//CX_LOG_WARNING() << "Image already exists as a texture, it is not uploaded again. This might be an error if you have changed the images data since first upload.";
		}
	*/
	report_gl_error();
	return success;
}

bool SharedOpenGLContext::uploadLUT(QString imageUid, vtkUnsignedCharArrayPtr lutTable)
{
	report_gl_error();
	bool success = false;

	if(lutTable->GetSize() == 0)
	{
		CX_LOG_ERROR() << "Cannot upload en empty LUT table as a 1D texture";
		return success;
	}

	unsigned long new_modified_time = lutTable->GetMTime();
	std::map<QString, std::pair<vtkTextureObjectPtr, unsigned long> >::iterator it = m1DTextureObjects.find(imageUid);
	bool uploaded_but_modified = (it != m1DTextureObjects.end() && (it->second.second != new_modified_time) );
	bool not_uploaded = it == m1DTextureObjects.end();
	bool uploade_and_not_modified = (it != m1DTextureObjects.end() && (it->second.second == new_modified_time) );

	if( uploaded_but_modified || not_uploaded)
	{
		//upload new data to gpu
		int lutSize = lutTable->GetNumberOfTuples();
		int lutDataSize = lutSize * lutTable->GetNumberOfComponents();

		//LUT table contains values between 0-255
		//we need normalized values between 0-1
		std::vector<float> normalizeLUT;
		normalizeLUT.resize(lutDataSize);
		unsigned char* ptr = lutTable->GetPointer(0);

		for (int i = 0; i < normalizeLUT.size(); ++i)
		{
			normalizeLUT[i] = ((float) *ptr) / 255.0;
			++ptr;
		}

		unsigned int width = lutSize;
		int dataType = VTK_FLOAT;
		int numComps = lutTable->GetNumberOfComponents();
		void *data = &(*normalizeLUT.begin());


		vtkTextureObjectPtr texture_object = this->get1DTextureForLUT(imageUid);
		if(!texture_object.GetPointer()) //not uploaded
		{
			//CX_LOG_DEBUG() << "lut for " << imageUid << " not_uploaded";
			texture_object = vtkTextureObjectPtr::New();
			//CX_LOG_DEBUG() << "create new texture_object";
		}
		//CX_LOG_DEBUG() << "lut for " << imageUid << " uploaded_but_modified";
		success = this->create1DTextureObject(texture_object, width, dataType, numComps, data, mContext);
		//CX_LOG_DEBUG() << "1D texture, handlet: " << texture_object->GetHandle() << " width: " << width << " numComps: " << numComps;
		m1DTextureObjects[imageUid] = std::make_pair(texture_object, lutTable->GetMTime());
	}
	else if(uploade_and_not_modified)
	{
		//do nothing
		success = true;
		//TODO these never happens for some reason, someone is setting LUT as modified
		//CX_LOG_DEBUG() << "----------------- lut for " << imageUid << " uploade_and_not_modified";
	}
	report_gl_error();

	return success;
}

bool SharedOpenGLContext::hasUploadedLUT(QString image_uid) const
{
	return m1DTextureObjects.count(image_uid);
}

vtkTextureObjectPtr SharedOpenGLContext::get1DTextureForLUT(QString image_uid) const
{
	vtkTextureObjectPtr retval;
	if(hasUploadedLUT(image_uid))
	{
		retval = m1DTextureObjects.at(image_uid).first;
	}
	return retval;
}

bool SharedOpenGLContext::delete1DTextureForLUT(QString image_uid)
{
	bool success = false;
	vtkTextureObjectPtr texture = this->get1DTextureForLUT(image_uid);
	std::map<QString, std::pair<vtkTextureObjectPtr, unsigned long> >::iterator it = m1DTextureObjects.find(image_uid);

	if(it != m1DTextureObjects.end())
	{
		m1DTextureObjects.erase(it);
		texture->ReleaseGraphicsResources(mContext);
		success = true;
	}

	return success;

}

bool SharedOpenGLContext::upload3DTextureCoordinates(QString uid, vtkFloatArrayPtr texture_coordinates)
{
	bool success = false;

	if(uid.isEmpty())
	{
		return success;
	}

	if(!texture_coordinates)
	{
		return success;
	}

	if(texture_coordinates->GetNumberOfTuples() < 1)
	{
		CX_LOG_ERROR() << "Cannot upload en empty array as 3D texture coordinates";
		return success;
	}

	int numberOfLines = texture_coordinates->GetNumberOfTuples();
	int numberOfComponentsLine = texture_coordinates->GetNumberOfComponents();
	vtkOpenGLBufferObjectPtr buffer = allocateAndUploadArrayBuffer(uid, numberOfLines, numberOfComponentsLine, texture_coordinates->GetPointer(0));

	if(buffer)
	{
		mTextureCoordinateBuffers[uid] = buffer;
		success = true;
	}

	return success;
}

bool SharedOpenGLContext::hasUploadedTextureCoordinates(QString uid) const
{
	return mTextureCoordinateBuffers.count(uid);
}

vtkOpenGLBufferObjectPtr SharedOpenGLContext::getTextureCoordinates(QString uid) const
{
	vtkOpenGLBufferObjectPtr retval;

	if(hasUploadedTextureCoordinates(uid))
	{
		retval = mTextureCoordinateBuffers.at(uid);
	}
	else
	{
		CX_LOG_ERROR() << "getTextureCoordinates failed";
	}

	return retval;
}

bool SharedOpenGLContext::create3DTextureObject(vtkTextureObjectPtr texture_object, unsigned int width, unsigned int height,  unsigned int depth, int dataType, int numComps, void *data, vtkOpenGLRenderWindowPtr opengl_renderwindow) const
{

	if(!this->makeCurrent())
	{
		CX_LOG_ERROR() << "Could not make current for 3D texture";
		return false;
	}

	texture_object->SetContext(opengl_renderwindow);
	//opengl_renderwindow->ActivateTexture(texture_object);

	if(texture_object->Create3DFromRaw(width, height, depth, numComps, dataType, data))
	{
		report_gl_error();
		//6403 == GL_RED 0x1903
		//6407 == GL_RGB 0x1907
		//6408 == GL_RGBA 0x1908
		//CX_LOG_DEBUG() << texture_object->GetFormat(dataType, numComps, true);
		texture_object->Activate();
		report_gl_error();
		texture_object->SetWrapS(vtkTextureObject::ClampToBorder);
		texture_object->SetWrapT(vtkTextureObject::ClampToBorder);
		texture_object->SetWrapR(vtkTextureObject::ClampToBorder);
		if(this->useLinearInterpolation())
		{
			texture_object->SetMagnificationFilter(vtkTextureObject::Linear);
			texture_object->SetMinificationFilter(vtkTextureObject::Linear);
		}
		else
		{
			texture_object->SetMagnificationFilter(vtkTextureObject::Nearest);
			texture_object->SetMinificationFilter(vtkTextureObject::Nearest);
		}
		texture_object->SendParameters();
		texture_object->Deactivate();
		report_gl_error();
	}
	else
	{
		CX_LOG_ERROR() << "Error creating 3D texture";
	}

	return true;
}

bool SharedOpenGLContext::useLinearInterpolation() const
{
	return settings()->value("View2D/useLinearInterpolationIn2DRendering").toBool();
}

bool SharedOpenGLContext::create1DTextureObject(vtkTextureObjectPtr texture_object, unsigned int width, int dataType, int numComps, void *data, vtkOpenGLRenderWindowPtr opengl_renderwindow) const
{
	if(!this->makeCurrent())
	{
		CX_LOG_ERROR() << "Could not make current for 1D texture";
		return false;
	}

	texture_object->SetContext(opengl_renderwindow);

	if(texture_object->Create1DFromRaw(width, numComps, dataType, data))
	{
		report_gl_error();
		//6403 == GL_RED 0x1903
		//6407 == GL_RGB 0x1907
		//6408 == GL_RGBA 0x1908
		texture_object->Activate();
		report_gl_error();
		texture_object->SetWrapS(vtkTextureObject::ClampToEdge);
		texture_object->SetMagnificationFilter(vtkTextureObject::Linear);
		texture_object->SetMinificationFilter(vtkTextureObject::Linear);
		texture_object->SendParameters();
		//CX_LOG_DEBUG() << "Texture unit: " << texture_object->GetTextureUnit();
		//CX_LOG_DEBUG() << "Created. Handle: " << texture_object->GetHandle() << " target: " << texture_object->GetTarget() << " context: ";
		//texture_object->GetContext()->PrintSelf(std::cout, vtkIndent(9));
		texture_object->Deactivate();
		report_gl_error();
	}
	else
	{
		CX_LOG_ERROR() << "Error creating 1D texture";
	}

	return true;
}

vtkOpenGLBufferObjectPtr SharedOpenGLContext::allocateAndUploadArrayBuffer(QString uid, int numberOfLines, int numberOfComponentsLine, const float *data) const
{
	if(!data)
	{
		CX_LOG_WARNING() << "NO DATA!";
	}

	vtkOpenGLBufferObjectPtr buffer_object;;
	//buffer_object->DebugOn();

	if(!this->makeCurrent())
	{
		CX_LOG_ERROR() << "Could not make current for ArraryBuffer";
		return buffer_object;
	}

	//CX_LOG_DEBUG() << "ALLOCATING BUFFER";
	if(hasUploadedTextureCoordinates(uid))
	{
		buffer_object = getTextureCoordinates(uid);
	}
	else
	{
		buffer_object = vtkOpenGLBufferObjectPtr::New();
	}

	buffer_object->GenerateBuffer(vtkOpenGLBufferObject::ArrayBuffer);

	if(buffer_object->Bind())
	{
		//CX_LOG_DEBUG() <<  "UPLOADING BUFFER: uid:" << uid << " handle: " << buffer_object->GetHandle();
		if(!buffer_object->Upload(
		      data,
		      numberOfLines*numberOfComponentsLine,  //how many floats to upload! (aka number of floats in the vector)
		      vtkOpenGLBufferObject::ArrayBuffer
		    ))
		{
			CX_LOG_ERROR() << "Error uploading buffer object data.";
		}

		report_gl_error();
	}
	else
	{
		CX_LOG_ERROR() << "Buffer object could not bind";
	}

	report_gl_error();
	return buffer_object;
}


}//cx

