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

#include "cxShaderCallback.h"

#include "cxGPUImageBuffer.h"
#include "cxTypeConversions.h"
#include "cxGLHelpers.h"
#include "cxLogger.h"

#include <vtkShaderProgram.h>
#include <vtkOpenGLHelper.h>


#ifdef __APPLE__
#include <OpenGL/glu.h>
//#include <OpenGL/gl.h>
#include "X11/Xlib.h"
//#include "/usr/include/X11/Xlib.h"
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/glu.h>
#include <GL/glext.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <GL/glext.h>
#endif


namespace cx
{


ShaderCallback::ShaderCallback(int index)
{
	this->init(index);
}

ShaderCallback::ShaderCallback()
{
	this->init(-1);
}

void ShaderCallback::init(int index)
{
	mWindow = (0.0);
	mLevel = (0.0);
	mLLR = (0.0);
	mAlpha = (1.0);

	mIndex = index;
//	this->Renderer = 0;
}

ShaderCallback::~ShaderCallback()
{

}

void ShaderCallback::SetBuffer(GPUImageDataBufferPtr buffer)
{
	mVolumeBuffer = buffer;
}

void ShaderCallback::SetBuffer(GPUImageLutBufferPtr buffer)
{
	mLutBuffer = buffer;
}

void ShaderCallback::SetColorAttribute(float window, float level, float llr,float alpha)
{
	mWindow = window;
	mLevel = level;
	mLLR = llr;
	mAlpha = alpha;
}

void ShaderCallback::initializeRendering()
{
	if (mVolumeBuffer)
		mVolumeBuffer->allocate(mIndex);
	if (mLutBuffer)
		mLutBuffer->allocate();
}

void ShaderCallback::setUniformiArray(vtkOpenGLHelper *cellBO, QString name, int val)
{
	QString fullName = QString("%1[%2]").arg(name).arg(mIndex);
//	uniforms->SetUniformi(cstring_cast(fullName), 1, &val);
	cellBO->Program->SetUniform1iv(cstring_cast(fullName), 1, &val);
}

void ShaderCallback::setUniformfArray(vtkOpenGLHelper *cellBO, QString name, float val)
{
	QString fullName = QString("%1[%2]").arg(name).arg(mIndex);
//	uniforms->SetUniformf(cstring_cast(fullName), 1, &val);
	cellBO->Program->SetUniform1fv(cstring_cast(fullName), 1, &val);
}

//void SingleVolumePainterHelper::eachRenderInternal(vtkSmartPointer<vtkShaderProgram2> shader)
void ShaderCallback::eachRenderInternal(vtkOpenGLHelper *cellBO)
{
	if (!mVolumeBuffer)
		return;

	mVolumeBuffer->bind(mIndex);

	int texture = 2*mIndex; //texture unit 1
	int lut = 2*mIndex+1; //texture unit 1

	int lutsize = 0;
	if (mLutBuffer)
	{
//		mLutBuffer->bind(mIndex);
		lutsize = mLutBuffer->getLutSize();
	}

	this->setUniformiArray(cellBO, "test", 1);

	this->setUniformiArray(cellBO, "cx_texture", texture);
	this->setUniformiArray(cellBO, "cxTextureSamplers", texture);
//	this->setUniformiArray(cellBO, "lut", lut);
//	this->setUniformiArray(cellBO, "lutsize", lutsize);
//	this->setUniformfArray(cellBO, "llr", mLLR);
//	this->setUniformfArray(cellBO, "level", mLevel);
//	this->setUniformfArray(cellBO, "window", mWindow);
//	this->setUniformfArray(cellBO, "alpha", mAlpha);

	report_gl_error();
}

void ShaderCallback::Execute(vtkObject *, unsigned long eventId, void *cbo)
{
	vtkOpenGLHelper *cellBO = reinterpret_cast<vtkOpenGLHelper*>(cbo);

//    float diffuseColor[3];


//    diffuseColor[0] = 0.4;
//    diffuseColor[1] = 0.7;
//    diffuseColor[2] = 0.6;
//    cellBO->Program->SetUniform3f("diffuseColorUniform", diffuseColor);

	if(eventId == vtkCommand::UpdateShaderEvent)
	{
		if(mVolumeBuffer)
		{
			mVolumeBuffer->allocate(mIndex);
		}
		this->eachRenderInternal(cellBO);
	}
	else if(eventId == vtkCommand::EndEvent) //pre render
	{
//		GLint oldTextureUnit;
//		glGetIntegerv(GL_ACTIVE_TEXTURE, &oldTextureUnit);
//		CX_LOG_DEBUG() << "oldTextureUnit: " << oldTextureUnit;

		if(mVolumeBuffer)
		{
			mVolumeBuffer->allocate(mIndex);
		}
//		if (mLutBuffer)
//		{
//			mLutBuffer->allocate();
//		}

//		glActiveTexture(oldTextureUnit);
	}
}

}//cx

