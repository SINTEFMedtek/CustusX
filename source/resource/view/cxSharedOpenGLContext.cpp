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

#include "cxGLHelpers.h"
#include "cxLogger.h"
#include "cxImage.h"
#include "cxUtilHelpers.h"

namespace cx
{

bool SharedOpenGLContext::isValid(vtkOpenGLRenderWindowPtr opengl_renderwindow, bool print)
{
	bool valid = true;

	if(!opengl_renderwindow)
		valid=false;
	//NB! opengl_renderwindow->SupportsOpenGL() creates a new context even if you have one, and this seg fault on render in vtkCocoaRenderWindow::CreateGLContext().
//	if(!opengl_renderwindow->SupportsOpenGL())
//		valid=false;
	//if(!opengl_renderwindow->IsDrawable())
	//	valid=false;
	//TODO add check to see if OpenGL context is initialized

	if(print)
	{
		CX_LOG_DEBUG() <<  "\n==== START SharedContext ====";
//		CX_LOG_DEBUG() <<  "SupportsOpenGL: " << opengl_renderwindow->SupportsOpenGL();
		CX_LOG_DEBUG() <<  "IsDrawable: " << opengl_renderwindow->IsDrawable();
		CX_LOG_DEBUG() <<  "Context support for open gl core 3.2: " << (vtkOpenGLRenderWindow::GetContextSupportsOpenGL32() ? "true" : "false");
		CX_LOG_DEBUG() <<  "Context was created at: " << opengl_renderwindow->GetContextCreationTime();
		const char *renderlib = vtkRenderWindow::GetRenderLibrary();
		CX_LOG_DEBUG() <<  "GetRenderLibrary: " << ((renderlib!=0) ? std::string(renderlib) : "NOT FOUND");
		CX_LOG_DEBUG() <<  "vtkOpenGLRenderWindow:";
//		opengl_renderwindow->PrintSelf(std::cout, vtkIndent(5));
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

bool SharedOpenGLContext::hasUploaded3DTexture(QString image_uid) const
{
	return m3DTextureObjects.count(image_uid);
}

vtkTextureObjectPtr SharedOpenGLContext::get3DTexture(QString image_uid) const
{
	vtkTextureObjectPtr retval;
	if(hasUploaded3DTexture(image_uid))
		retval = m3DTextureObjects.at(image_uid);
	else
		CX_LOG_ERROR() << "get3DTexture failed";
	return retval;
}

vtkImageDataPtr SharedOpenGLContext::downloadImageFromTextureBuffer(QString image_uid)
{
	vtkPixelBufferObjectPtr pixelBuffer;
	vtkImageDataPtr imageData = vtkImageDataPtr::New();
	vtkTextureObjectPtr texture = this->get3DTexture(image_uid);
	if(texture)
	{
		pixelBuffer = texture->Download();
		int dataType = texture->GetVTKDataType();
		unsigned width = texture->GetWidth();
		unsigned height = texture->GetHeight();
		unsigned depth = texture->GetDepth();
		unsigned dims[3] = {width, height, depth};
		int numComps = texture->GetComponents();
		vtkIdType increments[3] = {0 , 0, 0};

		imageData->SetExtent(0, width-1, 0, height-1, 0, depth-1);
//		imageData->SetSpacing(1, 1, 1);
		imageData->AllocateScalars(dataType, numComps);
		void* data = imageData->GetScalarPointer();

		pixelBuffer->Download3D(dataType, data, dims, numComps, increments);
	}
	else
		CX_LOG_ERROR() << "SharedOpenGLContext::downloadImageFromTextureBuffer failed";

	return imageData;
}

bool SharedOpenGLContext::makeCurrent() const
{
	//CX_LOG_DEBUG() << "1 Context is current: " << mContext->IsCurrent();
	mContext->MakeCurrent();
	//CX_LOG_DEBUG() << "2 Context is current: " << mContext->IsCurrent();

	/*
	if(!mContext)
		return false;
	if(!mContext->IsCurrent())
		mContext->MakeCurrent();
	if(!mContext->IsCurrent())
		CX_LOG_ERROR() << "OpenGL context is not current.";
		*/
	return mContext->IsCurrent();
}

bool SharedOpenGLContext::uploadImage(ImagePtr image)
{

	bool success = false;
	if(!image)
	{
		CX_LOG_ERROR() << "Cannot upload en empty image as a 3D texture";
		return success;
	}
	report_gl_error();

	vtkTextureObjectPtr texture_object;
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

		/*
		int size = dims[0]*dims[1]*dims[2]*numComps;
		CX_LOG_DEBUG() << "data in the middle 3 " << (int)((reinterpret_cast<unsigned char*>(data)[size/2+1]));
		for(int i=0; i<size; i=i+150)
			std::cout << (int)((reinterpret_cast<unsigned char*>(data)[i]));
		std::cout << std::endl;
		*/
		texture_object = this->create3DTextureObject(dims[0], dims[1], dims[2], dataType, numComps, data, mContext);
		m3DTextureObjects[image->getUid()] = texture_object;
		success = true;
	}
	else
	{
		//TODO Update image data?
		//CX_LOG_WARNING() << "Image already exists as a texture, it is not uploaded again. This might be an error if you have changed the images data since first upload.";
	}
	return success;
}

bool SharedOpenGLContext::uploadLUT(QString imageUid, vtkUnsignedCharArrayPtr lutTable)
{
	bool success = false;
	if(lutTable->GetSize() == 0)
	{
		CX_LOG_ERROR() << "Cannot upload en empty LUT table as a 1D texture";
		return success;
	}

	vtkTextureObjectPtr texture_object;
	if(!m1DTextureObjects.count(imageUid))
	{

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
			//std::cout << normalizeLUT[i] <<  " = " << ((float) *ptr)  << ", ";
			++ptr;
		}
		//std::cout << std::endl;

		unsigned int width = lutSize;
		int dataType = VTK_FLOAT;
		int numComps = lutTable->GetNumberOfComponents();
		void *data = &(*normalizeLUT.begin());
		texture_object = this->create1DTextureObject(width, dataType, numComps, data, mContext);
		CX_LOG_DEBUG() << "1D texture, handlet: " << texture_object->GetHandle() << " width: " << width << " numComps: " << numComps;
		m1DTextureObjects[imageUid] = texture_object;
		success = true;
	}
	else
	{
		//TODO update lut data????
		//CX_LOG_WARNING() << "LUT table for " << imageUid << " already exists as a texture, it is not uploaded again. This might be an error if you have changed the LUT data since first upload.";
	}
	return success;
}

bool SharedOpenGLContext::hasUploaded1DTexture(QString image_uid) const
{
	return m1DTextureObjects.count(image_uid);
}

vtkTextureObjectPtr SharedOpenGLContext::get1DTexture(QString image_uid) const
{
	vtkTextureObjectPtr retval;
	if(hasUploaded1DTexture(image_uid))
		retval = m1DTextureObjects.at(image_uid);
	else
		CX_LOG_ERROR() << "get1DTexture failed";
	return retval;
}

bool SharedOpenGLContext::upload3DTextureCoordinates(QString uid, vtkFloatArrayPtr texture_coordinates)
{
	bool success = false;
	if(uid.isEmpty())
		return success;

	if(!texture_coordinates)
		return success;

	if(texture_coordinates->GetNumberOfTuples() < 1)
	{
		CX_LOG_ERROR() << "Cannot upload en empty array as 3D texture coordinates";
		return success;
	}
	int numberOfLines = texture_coordinates->GetNumberOfTuples();
	int numberOfComponentsLine = texture_coordinates->GetNumberOfComponents();
	/*
	CX_LOG_DEBUG() << "GetSize: " << texture_coordinates->GetSize()
				   << "\n GetNumberOfTuples: " << numberOfLines
				   << "\n GetNumberOfComponents: " << numberOfComponentsLine
				   << "\n GetNumberOfTuples*GetNumberOfComponents: " << (numberOfLines*numberOfComponentsLine);
				   */

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
		retval = mTextureCoordinateBuffers.at(uid);
	else
		CX_LOG_ERROR() << "getTextureCoordinates failed";
	return retval;
}

vtkTextureObjectPtr SharedOpenGLContext::create3DTextureObject(unsigned int width, unsigned int height,  unsigned int depth, int dataType, int numComps, void *data, vtkOpenGLRenderWindowPtr opengl_renderwindow) const
{
	vtkTextureObjectPtr texture_object = vtkTextureObjectPtr::New();
//	texture_object->DebugOn();

	if(!this->makeCurrent())
	{
		CX_LOG_ERROR() << "Could not make current for 3D texture";
		return texture_object;
	}

	texture_object->SetContext(opengl_renderwindow);

	if(texture_object->Create3DFromRaw(width, height, depth, numComps, dataType, data))
	{
//		glFinish();
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
		texture_object->SetMagnificationFilter(vtkTextureObject::Linear);
		texture_object->SetMinificationFilter(vtkTextureObject::Linear);
		texture_object->SendParameters();


//		glFinish();

		//CX_LOG_DEBUG() << "Texture unit: " << texture_object->GetTextureUnit();
		//texture_object->PrintSelf(std::cout, vtkIndent(4));

		texture_object->Deactivate();

		report_gl_error();
	}
	else
		CX_LOG_ERROR() << "Error creating 3D texture";

	//CX_LOG_DEBUG() << "Created. Handle: " << texture_object->GetHandle() << " target: " << texture_object->GetTarget() << " context: ";
	//texture_object->GetContext()->PrintSelf(std::cout, vtkIndent(9));

	return texture_object;
}

vtkTextureObjectPtr SharedOpenGLContext::create1DTextureObject(unsigned int width, int dataType, int numComps, void *data, vtkOpenGLRenderWindowPtr opengl_renderwindow) const
{

	vtkTextureObjectPtr texture_object = vtkTextureObjectPtr::New();
//	texture_object->DebugOn();

	if(!this->makeCurrent())
	{
		CX_LOG_ERROR() << "Could not make current for 1D texture";
		return texture_object;
	}

	texture_object->SetContext(opengl_renderwindow);

	if(texture_object->Create1DFromRaw(width, numComps, dataType, data))
	{
//		glFinish();
		report_gl_error();

		//6403 == GL_RED 0x1903
		//6407 == GL_RGB 0x1907
		//6408 == GL_RGBA 0x1908
		CX_LOG_DEBUG() << "Creating 1D texture with format: "<< texture_object->GetFormat(dataType, numComps, true);

		texture_object->Activate();
		report_gl_error();

		texture_object->SetWrapS(vtkTextureObject::ClampToEdge);
		texture_object->SetMagnificationFilter(vtkTextureObject::Linear);
		texture_object->SetMinificationFilter(vtkTextureObject::Linear);
		texture_object->SendParameters();


		CX_LOG_DEBUG() << "Texture unit: " << texture_object->GetTextureUnit();
		texture_object->PrintSelf(std::cout, vtkIndent(4));

		texture_object->Deactivate();

		report_gl_error();
	}
	else
		CX_LOG_ERROR() << "Error creating 1D texture";

	//CX_LOG_DEBUG() << "Created. Handle: " << texture_object->GetHandle() << " target: " << texture_object->GetTarget() << " context: ";
	//texture_object->GetContext()->PrintSelf(std::cout, vtkIndent(9));

	return texture_object;
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
		buffer_object = getTextureCoordinates(uid);
	else
		buffer_object = vtkOpenGLBufferObjectPtr::New();

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
		CX_LOG_ERROR() << "Buffer object could not bind";
	report_gl_error();

	return buffer_object;
}


}//cx

