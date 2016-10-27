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

namespace cx
{

SharedOpenGLContext::SharedOpenGLContext(vtkOpenGLRenderWindowPtr openGLRenderWindow) :
	mContext(openGLRenderWindow)
{
	vtkObject::GlobalWarningDisplayOn();
}

void SharedOpenGLContext::upload(ImagePtr image)
{
	if(!image)
	{
		CX_LOG_ERROR() << "SharedOpenGLContext::upload: Not an image";
		return;
	}

	vtkImageDataPtr vtkImageData = image->getBaseVtkImageData();
	int* dims = vtkImageData->GetDimensions();
	int dataType = vtkImageData->GetScalarType();
	int numComps = vtkImageData->GetNumberOfScalarComponents();
	CX_LOG_DEBUG() << "dims: " << dims[0] << " " << dims[1] << " " << dims[2] << " " << " dataType: " << vtkImageData->GetScalarTypeAsString() << " numComps: " << numComps;
//	mContext->Render();
	mContext->OpenGLInitContext();
	mContext->MakeCurrent();
	report_gl_error();
	this->createTextureObject(dims[2], dims[0], dataType, numComps, dims[1], mContext, vtkImageData->GetScalarPointer());
}

vtkSmartPointer<vtkTextureObject> SharedOpenGLContext::createTextureObject(unsigned int depth, unsigned int width, int dataType, int numComps, unsigned int height, vtkSmartPointer<vtkOpenGLRenderWindow> opengl_renderwindow, void *data)
{
	vtkNew<vtkTextureObject> texture_object;
//	texture_object.Get()->DebugOn();
	texture_object->SetContext(opengl_renderwindow);

	if(!texture_object->Create3DFromRaw(width, height, depth, numComps, dataType, data))
		std::cout << "---------------------------------------- > Error creating 3D texture" << std::endl;

	report_gl_error();
	//6403 == GL_RED 0x1903
	//6407 == GL_RGB 0x1907
	//6408 == GL_RGBA 0x1908
	std::cout << texture_object->GetFormat(dataType, numComps, true) << std::endl;

	texture_object->Activate();
	report_gl_error();

	texture_object->SetWrapS(vtkTextureObject::ClampToEdge);
	texture_object->SetWrapT(vtkTextureObject::ClampToEdge);
	texture_object->SetWrapR(vtkTextureObject::ClampToEdge);
	texture_object->SetMagnificationFilter(vtkTextureObject::Linear);
	texture_object->SetMinificationFilter(vtkTextureObject::Linear);
	texture_object->SendParameters();

	std::cout << "Texture unit: " << texture_object->GetTextureUnit() << std::endl;
	texture_object->PrintSelf(std::cout, vtkIndent(4));

	report_gl_error();

	return texture_object.Get();
}

vtkSmartPointer<vtkOpenGLBufferObject> SharedOpenGLContext::allocateAndUploadArrayBuffer(int numberOfLines, int numberOfComponentsLine, const GLfloat *data)
{
	vtkNew<vtkOpenGLBufferObject> buffer_object;
	std::cout << "ALLOCATING BUFFER" << std::endl;
	buffer_object->GenerateBuffer(vtkOpenGLBufferObject::ArrayBuffer);
	if(!buffer_object->Bind())
		std::cout << "buffer object not bind" << std::endl;
	report_gl_error();

	std::cout << "UPLOADING" << std::endl;
	if(!buffer_object->Upload(
				data,
				numberOfLines*numberOfComponentsLine,  //how many floats to upload! (aka number of floats in the vector)
				vtkOpenGLBufferObject::ArrayBuffer
				))
	{
		vtkGenericWarningMacro(<< "Error uploading buffer object data.");
	}
	report_gl_error();

	return buffer_object.Get();
}


}//cx

