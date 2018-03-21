/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

#include "cxGLHelpers.h"
#include "cxLogger.h"
#include "GL/glew.h"

#ifdef __APPLE__
#include <glu.h>
#else
#include <GL/glu.h>
#endif

#include "cxOpenGLShaders.h"

namespace cx
{


ShaderCallback::ShaderCallback()
{
}

ShaderCallback::~ShaderCallback()
{
}

ShaderCallback *ShaderCallback::New()
{
	return new ShaderCallback;
}

void ShaderCallback::printDebugInfo(vtkOpenGLHelper *OpenGLHelper)
{
	if(!OpenGLHelper)
	{
		return;
	}

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

	vtkOpenGLHelper *OpenGLHelper = reinterpret_cast<vtkOpenGLHelper*>(cbo);

	if(!OpenGLHelper || !OpenGLHelper->VAO || !OpenGLHelper->Program)
	{
		return;
	}


	if(eventId == vtkCommand::UpdateShaderEvent)
	{
		report_gl_error();


		int textures_to_add = this->getNumberOfUploadedTextures();

		//Bind fragmentshader output variable
		// (glsl: vec4)
		if(textures_to_add != 0)
		{
			this->bindFSOutputVariable(OpenGLHelper->Program);
		}

		report_gl_error();

		//Bind VAO (Vertext Array Object - aka saved input to the vertex shader)
		OpenGLHelper->VAO->Bind();
		report_gl_error();

		for(int i=0; i< textures_to_add; ++i)
		{
			ShaderItemPtr shaderItem = mShaderItems.at(i);
			//	texture coordinates (glsl: vec3)
			vtkOpenGLBufferObjectPtr texture_coordinates = shaderItem->mTextureCoordinates;

			if(texture_coordinates)
			{
				if(!texture_coordinates->Bind())
				{
					CX_LOG_WARNING() << "Could not bind texture coordinates";
				}

				this->addArrayToAttributeArray(OpenGLHelper->Program, texture_coordinates, VS_In_Vec3_TextureCoordinate, i);
			}
			else
			{
				CX_LOG_WARNING() << "NO TEXTURE COORDINATES!";
			}

			report_gl_error();

			//	3D texture pointer (glsl: sampler3D)
			vtkTextureObjectPtr texture = shaderItem->mTexture;

			if(texture)
			{
				texture->Activate();
				this->addUniformiArray(OpenGLHelper->Program, getVectorNameFromName(FS_Uniform_3DTexture_Volume, i), texture->GetTextureUnit());
			}
			else
			{
				CX_LOG_WARNING() << "NO 3D TEXTURE!";
			}

			report_gl_error();

			//	1D texture pointer (glsl: sampler1D)
			vtkTextureObjectPtr lut = shaderItem->mLUT;

			if(lut)
			{
				lut->Activate();
				this->addUniformiArray(OpenGLHelper->Program, getVectorNameFromName(FS_Uniform_1DTexture_LUT, i), lut->GetTextureUnit());
			}
			else
			{
				CX_LOG_WARNING() << "NO 1D TEXTURE!";
			}

			report_gl_error();

			this->addUniformfArray(OpenGLHelper->Program, getVectorNameFromName(FS_Uniform_Window,i), shaderItem->mWindow);
			this->addUniformfArray(OpenGLHelper->Program, getVectorNameFromName(FS_Uniform_Level,i), shaderItem->mLevel);
			this->addUniformfArray(OpenGLHelper->Program, getVectorNameFromName(FS_Uniform_LLR,i), shaderItem->mLLR);
			this->addUniformfArray(OpenGLHelper->Program, getVectorNameFromName(FS_Uniform_Alpha,i), shaderItem->mAlpha);
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
		{
			return item;
		}
	}

	CX_LOG_ERROR() << "COULD NOT FIND THE SHADERITEM";
	return item;
}

int ShaderCallback::getNumberOfUploadedTextures() const
{
	return mShaderItems.size();
}

void ShaderCallback::add(ShaderCallback::ShaderItemPtr item)
{
	mShaderItems.push_back(item);
}

void ShaderCallback::clearShaderItems()
{
        mShaderItems.clear();
}

/**
 * @brief ShaderCallback::addArrayToAttributeArray
 * In the vertex shader there is an in array of vec3 which will contain texture coordinates per uploaded texture. This function populates this array.
 * @param program The shader program to work with.
 * @param buffer The buffer containing the texture coordinates.
 * @param name The name of the array variable in the vertex shader which will point to the texture coordinates.
 * @param vector_index The place in the
 */
void ShaderCallback::addArrayToAttributeArray(vtkShaderProgram *program, vtkOpenGLBufferObjectPtr buffer, std::string name, int vector_index)
{
	//--------
	//This is information about how the texture coordinates are uploaded.
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


	if(start_of_vector_index != -1)
	{
		GLint position_in_vector_index = start_of_vector_index + vector_index;
		buffer->Bind();

		glEnableVertexAttribArray(position_in_vector_index);

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
	{
		CX_LOG_ERROR() << "Error setting attribute " << name << " with vector_index " << vector_index;
	}

}

void ShaderCallback::addUniformiArray(vtkShaderProgram *program, std::string name, int value)
{
	//Note: Set uniform will fail if the uniform is not present OR active (used inside the program).
	report_gl_error();

	//CX_LOG_DEBUG() << "Adding uniform called: " << name << " with value " << value;
	if(!program->SetUniform1iv(name.c_str(), 1, &value))
	{
		CX_LOG_ERROR() << "Could not set uniform named " << name;
	}

	report_gl_error();
}

void ShaderCallback::addUniformfArray(vtkShaderProgram *program, std::string name, float value)
{
	//Note: Set uniform will fail if the uniform is not present OR active (used inside the program).
	report_gl_error();

	//CX_LOG_DEBUG() << "Adding uniform called: " << name << " with value " << value;
	if(!program->SetUniform1fv(name.c_str(), 1, &value))
	{
		CX_LOG_ERROR() << "Could not set uniform named " << name;
	}

	report_gl_error();
}

void ShaderCallback::bindFSOutputVariable(vtkShaderProgram *program)
{
	GLint color_frag_out_index = glGetFragDataLocation(program->GetHandle(), FS_Out_Vec4_Color.c_str());

	if(color_frag_out_index != -1)
	{
		glBindFragDataLocation(program->GetHandle(), color_frag_out_index, FS_Out_Vec4_Color.c_str()); //setting output of fragment shader
		//CX_LOG_DEBUG() << "Binding fragmentshader output to " << FS_Out_Vec4_Color << " at index "<< color_frag_out_index;
	}
	else
	{
		CX_LOG_ERROR() << "Could not find glGetFragDataLocation for " << FS_Out_Vec4_Color;
	}
}

std::string ShaderCallback::getVectorNameFromName(std::string name, int index_of_vector) const
{
	QString fullName = QString("%1[%2]").arg(QString(name.c_str())).arg(index_of_vector);
	return fullName.toStdString();
}

}//namespace cx

