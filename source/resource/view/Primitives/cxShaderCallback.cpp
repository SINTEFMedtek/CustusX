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
const std::string ShaderCallback::FS_Uniform_1DTexture = "cx_fs_uniform_1Dtexture";
const std::string ShaderCallback::FS_Uniform_Window = "cx_fs_uniform_window";
const std::string ShaderCallback::FS_Uniform_Level = "cx_fs_uniform_level";
const std::string ShaderCallback::FS_Uniform_LLR = "cx_fs_uniform_llr";
const std::string ShaderCallback::FS_Uniform_Alpha = "cx_fs_uniform_alpha";
const std::string ShaderCallback::FS_Out_Vec4_Color = "cx_fs_out_color";

const int ShaderCallback::Const_Int_NumberOfTextures = 1; //TODO - hva skal det stå her?	static const int FS_Const_Int_NumberOfTextures;

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

//void ShaderCallback::SetBuffer(GPUImageDataBufferPtr buffer)
//{
//	mVolumeBuffer = buffer;
//}

//void ShaderCallback::SetBuffer(GPUImageLutBufferPtr buffer)
//{
//	mLutBuffer = buffer;
//}

//void ShaderCallback::SetColorAttribute(float window, float level, float llr,float alpha)
//{
//	mWindow = window;
//	mLevel = level;
//	mLLR = llr;
//	mAlpha = alpha;
//}

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

			//	texture coordinates (glsl: vec3)
			vtkOpenGLBufferObjectPtr texture_coordinates = mShaderItems.at(i)->mTextureCoordinates;
			if(texture_coordinates)
			{
				if(!texture_coordinates->Bind())
					CX_LOG_WARNING() << "Could not bind texture coordinates";
				this->addArrayToAttributeArray(OpenGLHelper->Program, texture_coordinates, VS_In_Vec3_TextureCoordinate, i);
			}
			else
			{
				CX_LOG_WARNING() << "NO TEXTURE COORDINATES!";
			}
			report_gl_error();

			//	3D texture pointer (glsl: sampler3D)
			vtkTextureObjectPtr texture = mShaderItems.at(i)->mTexture;
			if(texture)
			{
				texture->Activate();
				this->addUniform(OpenGLHelper->Program, generateFSUniform3DTextureVectorName(i), texture->GetTextureUnit());

			}
			else
			{
				CX_LOG_WARNING() << "NO 3D TEXTURE!";
			}
			report_gl_error();

			//	1D texture pointer (glsl: sampler1D)
			vtkTextureObjectPtr lut = mShaderItems.at(i)->mLUT;
			if(lut)
			{
				lut->Activate();
				this->addUniform(OpenGLHelper->Program, generateFSUniform1DTextureVectorName(i), lut->GetTextureUnit());

			}
			else
			{
				CX_LOG_WARNING() << "NO 1D TEXTURE!";
			}
			report_gl_error();
		}
	}
	report_gl_error();

}

ShaderCallback::ShaderItemPtr ShaderCallback::getShaderItem(QString image_uid) const
{
	ShaderItemPtr item;
	for(int i=0; i<mShaderItems.size(); ++i)
	{
		item = mShaderItems.at(i);
		if(item->mTextureUid == image_uid)
			return item;
	}
	CX_LOG_ERROR() << "COULD NOT FIND THE SHADERITEM";
	return item;
}

int ShaderCallback::getNumberOfUploadedTextures() const
{
	CX_LOG_DEBUG() << "Number of shaderitems" << mShaderItems.size();
	return mShaderItems.size();
}

//TODO move to SharedOpenGLContext?
void ShaderCallback::addArrayToAttributeArray(vtkShaderProgram *program, vtkOpenGLBufferObjectPtr buffer, std::string name, int vector_index)
{
	//vao->DebugOn();
	//--------
	//TODO extract to struct?
	int offset = 0;
	int vec_size = 3;
	size_t stride = sizeof(float)*vec_size; //is this correct? was *3;
	int elementType = GL_FLOAT; //VTK_FLOAT
	bool normalize = false;

	//CX_LOG_DEBUG() << "Adding attribute called: " << name << " to vector_index: " << vector_index;
	//--------


	const GLchar *namePtr = static_cast<const GLchar *>(name.c_str());
	GLint start_of_vector_index = glGetAttribLocation(program->GetHandle(), namePtr);
	report_gl_error();

	//CX_LOG_DEBUG() << "start_of_vector_index: " << start_of_vector_index;

	if(start_of_vector_index != -1)
	{
		GLint position_in_vector_index = start_of_vector_index + vector_index;
		//CX_LOG_DEBUG() << "position_in_vector_index: " << position_in_vector_index;
		buffer->Bind();
		//report_gl_error();

		glEnableVertexAttribArray(position_in_vector_index);
		//report_gl_error();

		glVertexAttribPointer(position_in_vector_index,
		  vec_size,
		  elementType,
		  normalize,
		  static_cast<GLsizei>(stride),
		  ((char *)NULL + (offset))
		);
		report_gl_error();
	}
	else
		CX_LOG_ERROR() << "Error setting attribute " << name << " with vector_index " << vector_index;

}

//TODO Remove? Does not work with in variables in glsl that are vectors.
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

	CX_LOG_DEBUG() << "Adding attribute called: " << name;
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
		CX_LOG_ERROR() << "Error setting attribute " << name <<" in shader VAO.";
	}
	report_gl_error();
}

//TODO move to SharedOpenGLContext?
void ShaderCallback::addUniform(vtkShaderProgram *program, std::string name, int value)
{
	//Note: Set uniform will fail if the uniform is not present OR active (used inside the program).
	report_gl_error();
	CX_LOG_DEBUG() << "Adding uniform called: " << name << " with value " << value;
	//if(!program->SetUniformi(name.c_str(), value))
	if(!program->SetUniform1iv(name.c_str(), 1, &value))
		CX_LOG_ERROR() << "Could not set uniform named " << name;
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

std::string ShaderCallback::generateFSUniform3DTextureVectorName(int index_of_vector) const
{
	return getVectorNameFromName(FS_Uniform_3DTexture, index_of_vector);
}

std::string ShaderCallback::generateFSUniform1DTextureVectorName(int index_of_vector) const
{
	return getVectorNameFromName(FS_Uniform_1DTexture, index_of_vector);
}

std::string ShaderCallback::getVectorNameFromName(std::string name, int index_of_vector) const
{
	QString fullName = QString("%1[%2]").arg(QString(name.c_str())).arg(index_of_vector);
	return fullName.toStdString();
}

}//namespace cx

