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
	if(!opengl_renderwindow->SupportsOpenGL())
		valid=false;
	//if(!opengl_renderwindow->IsDrawable())
	//	valid=false;
	//TODO add check to see if OpenGL context is initialized

	if(print)
	{
		CX_LOG_DEBUG() <<  "\n==== START SharedContext ====";
		CX_LOG_DEBUG() <<  "SupportsOpenGL: " << opengl_renderwindow->SupportsOpenGL();
		CX_LOG_DEBUG() <<  "IsDrawable: " << opengl_renderwindow->IsDrawable();
		CX_LOG_DEBUG() <<  "Context support for open gl core 3.2: " << (vtkOpenGLRenderWindow::GetContextSupportsOpenGL32() ? "true" : "false");
		CX_LOG_DEBUG() <<  "Context was created at: " << opengl_renderwindow->GetContextCreationTime();
		const char *renderlib = vtkRenderWindow::GetRenderLibrary();
		CX_LOG_DEBUG() <<  "GetRenderLibrary: " << ((renderlib!=0) ? std::string(renderlib) : "NOT FOUND");
		CX_LOG_DEBUG() <<  "vtkOpenGLRenderWindow:";
		opengl_renderwindow->PrintSelf(std::cout, vtkIndent(5));
		CX_LOG_DEBUG() <<  "==== END SharedContext ====\n";
	}
	report_gl_error();

	return valid;
}

SharedOpenGLContext::SharedOpenGLContext(vtkOpenGLRenderWindowPtr sharedContext) :
	mContext(sharedContext)
{
}

bool SharedOpenGLContext::hasUploadedTexture(QString uid) const
{
	return mTextureObjects.count(uid);
}

vtkTextureObjectPtr SharedOpenGLContext::getTexture(QString uid) const
{
	vtkTextureObjectPtr retval;
	if(hasUploadedTexture(uid))
		retval = mTextureObjects.at(uid);
	return retval;
}

bool SharedOpenGLContext::makeCurrent() const
{
	if(!mContext->IsCurrent())
		mContext->MakeCurrent();
	if(!mContext->IsCurrent())
		CX_LOG_ERROR() << "OpenGL context is not current.";
	return mContext->IsCurrent();
}

bool SharedOpenGLContext::upload3DTexture(ImagePtr image)
{
	bool success = false;
	if(!image)
	{
		CX_LOG_ERROR() << "Cannot upload en empty image as a 3D texture";
		return success;
	}

	vtkImageDataPtr vtkImageData = image->getBaseVtkImageData();
	int* dims = vtkImageData->GetDimensions();
	int dataType = vtkImageData->GetScalarType();
	int numComps = vtkImageData->GetNumberOfScalarComponents();

	CX_LOG_DEBUG() << "dims: " << dims[0] << " " << dims[1] << " " << dims[2] << " " << " dataType: " << vtkImageData->GetScalarTypeAsString() << " numComps: " << numComps;
	report_gl_error();

	vtkTextureObjectPtr texture_object;
	if(!mTextureObjects.count(image->getUid()))
		texture_object = this->createTextureObject(dims[0], dims[1], dims[1], dataType, numComps, vtkImageData->GetScalarPointer(), mContext);

	if(texture_object)
	{
		mTextureObjects[image->getUid()] = texture_object;
		success = true;
	}
	return success;
}

vtkTextureObjectPtr SharedOpenGLContext::createTextureObject(unsigned int width, unsigned int height,  unsigned int depth, int dataType, int numComps, void *data, vtkSmartPointer<vtkOpenGLRenderWindow> opengl_renderwindow)
{
	vtkTextureObjectPtr texture_object = vtkTextureObjectPtr::New();
	texture_object->DebugOn();

	if(!this->makeCurrent())
		return texture_object;

	texture_object->SetContext(opengl_renderwindow);

	if(texture_object->Create3DFromRaw(width, height, depth, numComps, dataType, data))
	{
		report_gl_error();

		//6403 == GL_RED 0x1903
		//6407 == GL_RGB 0x1907
		//6408 == GL_RGBA 0x1908
		CX_LOG_DEBUG() << texture_object->GetFormat(dataType, numComps, true);

		texture_object->Activate();
		report_gl_error();

		texture_object->SetWrapS(vtkTextureObject::ClampToEdge);
		texture_object->SetWrapT(vtkTextureObject::ClampToEdge);
		texture_object->SetWrapR(vtkTextureObject::ClampToEdge);
		texture_object->SetMagnificationFilter(vtkTextureObject::Linear);
		texture_object->SetMinificationFilter(vtkTextureObject::Linear);
		texture_object->SendParameters();

		CX_LOG_DEBUG() << "Texture unit: " << texture_object->GetTextureUnit();
		texture_object->PrintSelf(std::cout, vtkIndent(4));

		report_gl_error();
	}
	else
		CX_LOG_ERROR() << "Error creating 3D texture";

	return texture_object;
}

vtkOpenGLBufferObjectPtr SharedOpenGLContext::allocateAndUploadArrayBuffer(int numberOfLines, int numberOfComponentsLine, const GLfloat *data)
{
	vtkOpenGLBufferObjectPtr buffer_object = vtkOpenGLBufferObjectPtr::New();
	buffer_object->DebugOn();

	if(!this->makeCurrent())
		return buffer_object;

	CX_LOG_DEBUG() << "ALLOCATING BUFFER";
	buffer_object->GenerateBuffer(vtkOpenGLBufferObject::ArrayBuffer);
	if(buffer_object->Bind())
	{
		CX_LOG_DEBUG() <<  "UPLOADING BUFFER";
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
		CX_LOG_DEBUG() << "Buffer object could not bind";
	report_gl_error();

	return buffer_object;
}


}//cx

