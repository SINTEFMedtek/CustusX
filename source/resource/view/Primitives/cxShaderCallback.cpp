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

#include <vtkShaderProgram.h>
#include <vtkShader.h>
#include <vtkOpenGLHelper.h>
#include <vtkOpenGLVertexArrayObject.h>
#include <vtkOpenGLBufferObject.h>
#include <vtkTextureObject.h>
#include <vtkOpenGLIndexBufferObject.h>
#include <vtkOpenGLRenderWindow.h>

#include "cxLogger.h"
#include "cxSharedOpenGLContext.h"

//-----DELETE?-----
#include "cxGPUImageBuffer.h"
#include "cxTypeConversions.h"
#include "cxGLHelpers.h"
//-----DELETE?-----

namespace cx
{

const std::string ShaderCallback::VS_In_Vec3_TextureCoordinate = "cx_vs_in_texture_coordinates";
const std::string ShaderCallback::VS_Out_Vec3_TextureCoordinate = "cx_vs_out_texture_coordinates";
const std::string ShaderCallback::FS_In_Vec3_TextureCoordinate = "cx_fs_in_texture_coordinates";
const std::string ShaderCallback::FS_Uniform_3DTexture = "cx_fs_uniform_3Dtexture";
const std::string ShaderCallback::FS_Out_Vec4_Color = "cx_fs_out_color";
const int ShaderCallback::Const_Int_NumberOfTextures = 4; //TODO - hva skal det stå her?	static const int FS_Const_Int_NumberOfTextures;

/*ShaderCallback::ShaderCallback(int index)
{
	this->init(index);
}*/

ShaderCallback::ShaderCallback()
{
	//this->init(-1);
}

/*
void ShaderCallback::init(int index)
{
	mWindow = (0.0);
	mLevel = (0.0);
	mLLR = (0.0);
	mAlpha = (1.0);

	mIndex = index;
	//	this->Renderer = 0;
}*/

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

//void ShaderCallback::initializeRendering()
//{
	/*
	if (mVolumeBuffer)
		mVolumeBuffer->allocate(mIndex);
	if (mLutBuffer)
		mLutBuffer->allocate();
		*/
//}

//void ShaderCallback::setUniformiArray(vtkOpenGLHelper *cellBO, QString name, int val)
//{
//	QString fullName = QString("%1[%2]").arg(name).arg(mIndex);
//	//	uniforms->SetUniformi(cstring_cast(fullName), 1, &val);
//	cellBO->Program->SetUniform1iv(cstring_cast(fullName), 1, &val);
//}

//void ShaderCallback::setUniformfArray(vtkOpenGLHelper *cellBO, QString name, float val)
//{
//	QString fullName = QString("%1[%2]").arg(name).arg(mIndex);
//	//	uniforms->SetUniformf(cstring_cast(fullName), 1, &val);
//	cellBO->Program->SetUniform1fv(cstring_cast(fullName), 1, &val);
//}


//void SingleVolumePainterHelper::eachRenderInternal(vtkSmartPointer<vtkShaderProgram2> shader)
//void ShaderCallback::eachRenderInternal(vtkOpenGLHelper *cellBO)
//{
	/*
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
*/

	//this->setUniformiArray(cellBO, "test", 1);

	//this->setUniformiArray(cellBO, "cx_texture", texture);
	//this->setUniformiArray(cellBO, "cxTextureSamplers", texture);
	//	this->setUniformiArray(cellBO, "lut", lut);
	//	this->setUniformiArray(cellBO, "lutsize", lutsize);
	//	this->setUniformfArray(cellBO, "llr", mLLR);
	//	this->setUniformfArray(cellBO, "level", mLevel);
	//	this->setUniformfArray(cellBO, "window", mWindow);
	//	this->setUniformfArray(cellBO, "alpha", mAlpha);


	//	report_gl_error();
	//	float values[9] = {0, 0, 0,
	//					   0, 0, 1,
	//					   0, 1, 0};
	//	cellBO->Program->SetUniformMatrix3x3("cx_tcoordMat", values);

	//	cellBO->Program->SetAttributeArray("cx_tcoordMC", values, 3, vtkShaderProgram::Normalize);

//	report_gl_error();
//}


ShaderCallback *ShaderCallback::New()
{
	return new ShaderCallback;
}

void ShaderCallback::printDebugInfo(vtkOpenGLHelper *OpenGLHelper)
{
	if(!OpenGLHelper)
		return;
	vtkShaderProgram *program = OpenGLHelper->Program;
	std::cout << "Program is compiled? " << program->GetCompiled() << std::endl;
	std::cout << "Program is bound? " << program->isBound() << std::endl;
	std::cout << "IBO index count " << OpenGLHelper->IBO->IndexCount << std::endl;
	std::string vertexshader = program->GetVertexShader()->GetSource();
	std::cout << "Vertexshader:\n " << vertexshader << std::endl;
	std::string fragmentshader = program->GetFragmentShader()->GetSource();
	std::cout << "Fragmentshader:\n " << fragmentshader << std::endl;
	report_gl_error();
}

void ShaderCallback::Execute(vtkObject *, unsigned long eventId, void *cbo)
{
	report_gl_error();
	//CX_LOG_DEBUG_CHECKPOINT() << "START";
	vtkOpenGLHelper *OpenGLHelper = reinterpret_cast<vtkOpenGLHelper*>(cbo);
	if(!OpenGLHelper || !OpenGLHelper->VAO || !OpenGLHelper->Program)
		return;

	//this->printDebugInfo(OpenGLHelper);

	if(eventId == vtkCommand::UpdateShaderEvent)
	{
		// THIS DID NOT WORK!!!
		/*
		if(!mContext)
			return;
		mContext->makeCurrent();
		report_gl_error();
		*/

		//Bind fragmentshader output variable
		// (glsl: vec4)
		this->bindFSOutputVariable(OpenGLHelper->Program);
		report_gl_error();

		//Bind VAO (Vertext Array Object - aka saved input to the vertex shader)
		OpenGLHelper->VAO->Bind();
		report_gl_error();

		int number_to_add = mShaderItems.size();
		for(int i=0; i< number_to_add; ++i)
		{
			std::string name = VS_In_Vec3_TextureCoordinate+"_0";//generateVSAttributeTextureCoordinateVectorName(i);

			//Upload attribute arrays
			//	texture coordinates (glsl: vec3)
			vtkOpenGLBufferObjectPtr texture_coordinates = mTextureCoordinates;
			if(texture_coordinates)
			{
				//CX_LOG_DEBUG() << "1 Using texture coordinates with handle: " << mTextureCoordinates->GetHandle();
				if(!texture_coordinates->Bind())
					CX_LOG_WARNING() << "Could not bind texture coordinates";
				//CX_LOG_DEBUG() << "2 Using texture coordinates with handle: " << mTextureCoordinates->GetHandle();
				this->addToAttributeArray(OpenGLHelper->VAO, OpenGLHelper->Program, texture_coordinates, name);
			}
			else
			{
				CX_LOG_WARNING() << "NO TEXTURE COORDINATES!";
			}
			report_gl_error();

			//Upload uniforms
			//Note: Texture is already uploaded by SharedOpenGLContext
			//	texture pointer (glsl: sampler3D)
			vtkTextureObjectPtr texture = mShaderItems.at(i)->mTexture;
			if(texture)
			{
				/*
				texture->SetContext(opengl_renderwindow);
				texture->Activate();
				*/

				//GLuint tex;
				//glGenTextures(1, &tex);
				//glActiveTexture(texture->GetTextureUnit());

				//CX_LOG_DEBUG() << "1. Handle: " << texture->GetHandle() << " target: " << texture->GetTarget() << " context: " << texture->GetContext();

				//texture->Activate();
				//CX_LOG_DEBUG() << "2. Handle: " << texture->GetHandle() << " target: " << texture->GetTarget() << " context: " << texture->GetContext();
				//mCurrentContext->MakeCurrent();
				//glBindTexture(GL_TEXTURE_3D, texture->GetTextureUnit());

				//target:
				//GL_TEXTURE_3D 0x806F = 32879
				//this->addUniform(OpenGLHelper->Program, FS_Uniform_3DTexture, opengl_renderwindow->GetTextureUnitForTexture(texture.Get()));
				//this->addUniform(OpenGLHelper->Program, FS_Uniform_3DTexture, mSharedOpenGLContext->mContext->GetTextureUnitForTexture(texture.Get()));

				//report_gl_error();
				texture->Activate(); //failed ???
				//report_gl_error();
				this->addUniform(OpenGLHelper->Program, FS_Uniform_3DTexture, texture->GetTextureUnit());

				//this->addUniform(OpenGLHelper->Program, generateFSUniformTextureVectorName(i), texture->GetTextureUnit());

				/*
				mSharedOpenGLContext->makeCurrent();
				texture->Activate();
				glActiveTexture(texture->GetTextureUnit());
				mCurrentContext->MakeCurrent();
				glActiveTexture(texture->GetTextureUnit());
				glBindTexture(texture->GetTarget(), texture->GetHandle());
				*/
				//texture->Activate();

				vtkOpenGLRenderWindowPtr opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(mCurrentContext.Get());
				opengl_renderwindow->ActivateTexture(texture.Get());
				mSharedOpenGLContext->mContext->ActivateTexture(texture.Get());
				this->addUniform(OpenGLHelper->Program, FS_Uniform_3DTexture, texture->GetTextureUnit());
			}
			report_gl_error();
		}
	}
	report_gl_error();
	//CX_LOG_DEBUG_CHECKPOINT() << "END";
	//mCurrentContext->Modified();
}

//TODO move to SharedOpenGLContext?
void ShaderCallback::addToAttributeArray(vtkOpenGLVertexArrayObject *vao, vtkShaderProgram *program, vtkOpenGLBufferObjectPtr buffer, std::string name)
{
	//vao->DebugOn();
	//--------
	//TODO extract to struct?
	int offset = 0;
	int vec_size = 3;
	size_t stride = sizeof(float)*vec_size; //is this correct? was *3;
	int elementType = VTK_FLOAT;
	bool normalize = false;

	//CX_LOG_DEBUG() << "Adding attribute called: " << name;
	//--------

	if (!vao->AddAttributeArray(
				program,			//vtkShaderProgram
				buffer.Get(),		//vtkOpenGLBufferObject
				name,				//std::string
				offset,				//int (offset)				where to start reading g_color_buffer_data, offset != 0 == discard some of the first values
				stride,				//size_t (stride)			If stride is 0, the generic vertex attributes are understood to be tightly packed in the array.
				elementType,		//int (elementType)			Specifies the data type of each component in the array
				vec_size,			//int (elementTupleSize)	Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
				normalize			//bool (normalize)
				))
	{
		CX_LOG_ERROR() << "Error setting attribute \'" << name <<"\' in shader VAO.";
	}
	report_gl_error();
}

//TODO move to SharedOpenGLContext?
void ShaderCallback::addUniform(vtkShaderProgram *program, std::string name, int value)
{
	report_gl_error();
	//CX_LOG_DEBUG() << "Adding uniform called: " << name << " with value " << value;
	if(!program->SetUniformi(name.c_str(), value))
		CX_LOG_ERROR() << "Could not set uniform named \'" << name << "\'.";
	report_gl_error();
}

//TODO move to SharedOpenGLContext?
void ShaderCallback::bindFSOutputVariable(vtkShaderProgram *program)
{
	GLint color_frag_out_index = glGetFragDataLocation(program->GetHandle(), FS_Out_Vec4_Color.c_str());
	if(color_frag_out_index != -1)
	{
		glBindFragDataLocation(program->GetHandle(), color_frag_out_index, FS_Out_Vec4_Color.c_str()); //setting output of fragment shader
		//CX_LOG_DEBUG() << "Binding fragmentshader output to " << FS_Out_Vec4_Color << " at index "<< color_frag_out_index;
	} else
		CX_LOG_ERROR() << "Could not find glGetFragDataLocation for " << FS_Out_Vec4_Color;
}

std::string ShaderCallback::generateVSAttributeTextureCoordinateVectorName(int index_of_vector) const
{
	return getVectorNameFromName(VS_In_Vec3_TextureCoordinate, index_of_vector);
}

std::string ShaderCallback::generateFSUniformTextureVectorName(int index_of_vector) const
{
	return getVectorNameFromName(FS_Uniform_3DTexture, index_of_vector);
}

std::string ShaderCallback::getVectorNameFromName(std::string name, int index_of_vector) const
{
	QString fullName = QString("%1[%2]").arg(QString(name.c_str())).arg(index_of_vector);
	return fullName.toStdString();
}

}//namespace cx

