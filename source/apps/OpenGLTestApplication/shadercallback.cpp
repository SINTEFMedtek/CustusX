#include "shadercallback.h"

#include <vtkOpenGLHelper.h>
#include <vtkShaderProgram.h>
#include <vtkOpenGLBufferObject.h>
#include <vtkNew.h>
#include <vtkOpenGLVertexArrayObject.h>
#include <vtkOpenGLIndexBufferObject.h>
#include <vtkShader.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkTextureObject.h>
//#include <vtkCubeSource.h>

#include "vtkfixture.h"

#include "cxGLHelpers.h"


ShaderCallback *ShaderCallback::New()
{ return new ShaderCallback; }

/*
void ShaderCallback::test2(unsigned long event, void *cbo)
{
	std::cout << "START TEST 2" << std::endl;

	if(event == vtkCommand::UpdateShaderEvent)
	{
		std::cout << "--- START UpdateShaderEvent" << std::endl;
		vtkOpenGLHelper *cellBO = reinterpret_cast<vtkOpenGLHelper*>(cbo);
		report_gl_error();
		std::cout << "--- 1" << std::endl;

		if(cellBO && cellBO->VAO)
		{
			std::cout << "IBO count " << cellBO->IBO->IndexCount << std::endl;

			cellBO->VAO->Bind();

			GLint color_frag_out_index = glGetFragDataLocation(cellBO->Program->GetHandle(), "color");
			std::cout << "color_frag_out_index " << color_frag_out_index << std::endl;
			glBindFragDataLocation(cellBO->Program->GetHandle(), color_frag_out_index, "color");

			std::cout << "--- 2" << std::endl;


			// An array of 3 vectors which represents 3 vertices
			// One color for each vertex. They were generated randomly.
			std::cout << "--- 3" << std::endl;

			//GLuint VertexArrayID;
			//glGenVertexArrays(1, &VertexArrayID);
			//glBindVertexArray(cellBO->IBO->GetHandle());

			cellBO->IBO->Bind();

			// This will identify our color buffer
			GLuint colorbuffer;

			// Generate 1 buffer, put the resulting identifier in colorbuffer
			glGenBuffers(1, &colorbuffer);
			std::cout << "--- 4" << std::endl;

			// The following commands will talk about our 'colorbuffer' buffer
			glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
			std::cout << "--- 5" << std::endl;

			// Give our color to OpenGL.
			glBufferData(GL_ARRAY_BUFFER, sizeof(color_data), color_data, GL_STATIC_DRAW);
			report_gl_error();
			std::cout << "--- 6" << std::endl;

			GLint index_color = glGetAttribLocation(cellBO->Program->GetHandle(), "COLOR_VSIN");
			std::cout << "index_color " << index_color << std::endl;
			report_gl_error();


			// 2nd attribute buffer : color
			glEnableVertexAttribArray(index_color);
			glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
			glVertexAttribPointer(
				index_color,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// Cleanup VBO and shader
			//glDeleteBuffers(1, &colorbuffer);
			//glDeleteProgram(programID);
			//glDeleteVertexArrays(1, &VertexArrayID);
			//cellBO->IBO->Release();


			glDisableVertexAttribArray(index_color);
			report_gl_error();


		}
	std::cout << "--- END UpdateShaderEvent" << std::endl;
	}
	std::cout << " END TEST 2" << std::endl;
}
*/

void ShaderCallback::test( unsigned long event, void *cbo)
{
	std::cout << " START TEST" << std::endl;
	if(event == vtkCommand::UpdateShaderEvent)
	{
		std::cout << "--- START UpdateShaderEvent" << std::endl;
		vtkOpenGLHelper *cellBO = reinterpret_cast<vtkOpenGLHelper*>(cbo);
		report_gl_error();

		if(cellBO && cellBO->VAO)
		{
			report_gl_error();

			vtkShaderProgram *program = cellBO->Program;
			vtkOpenGLVertexArrayObject *vao = cellBO->VAO;

			std::cout << "Program is compiled? " << program->GetCompiled() << std::endl;
			std::cout << "Program is bound? " << program->isBound() << std::endl;
			std::cout << "IBO index count " << cellBO->IBO->IndexCount << std::endl;
			report_gl_error();


			GLint color_frag_out_index = glGetFragDataLocation(cellBO->Program->GetHandle(), "color");
			std::cout << "color index " << color_frag_out_index << std::endl;
			glBindFragDataLocation(cellBO->Program->GetHandle(), color_frag_out_index, "color"); //setting output of fragment shader


			std::cout << "ADDING ATTRIBUTE ARRAY" << std::endl;
			/*
			vao->Bind();
			if (!vao->AddAttributeArray(program, vbo.Get(), "vertexMC", 0, sizeof(float)*3, VTK_FLOAT, 3, false))
			{
				vtkGenericWarningMacro(<< "Error setting 'vertexMC' in shader VAO.");
			}
			 */
			vao->Bind();
			//Input color
			int vec_size = 3;
			if (!vao->AddAttributeArray(
						program,		//vtkShaderProgram
						mColorBufferObject.Get(),	//vtkOpenGLBufferObject
						"COLOR_VSIN",	//std::string
						0,				//int (offset)				where to start reading g_color_buffer_data, offset != 0 == discard some of the first values
						sizeof(float)*3,				//size_t (stride)			If stride is 0, the generic vertex attributes are understood to be tightly packed in the array.
						VTK_FLOAT,		//int (elementType)			Specifies the data type of each component in the array
						vec_size,				//int (elementTupleSize)	Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
						false			//bool (normalize)
						))
			{
				vtkGenericWarningMacro(<< "Error setting 'COLOR' in shader VAO.");
			}

			//Input texture coordinates
			vec_size = 3;
			if (!vao->AddAttributeArray(
						program,		//vtkShaderProgram
						mTextureBufferObject.Get(),	//vtkOpenGLBufferObject
						"TEXTURE_COORDINATE_VSIN",	//std::string
						0,				//int (offset)				where to start reading g_color_buffer_data, offset != 0 == discard some of the first values
						sizeof(float)*3,				//size_t (stride)			If stride is 0, the generic vertex attributes are understood to be tightly packed in the array.
						VTK_FLOAT,		//int (elementType)			Specifies the data type of each component in the array
						vec_size,				//int (elementTupleSize)	Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
						false			//bool (normalize)
						))
			{
				vtkGenericWarningMacro(<< "Error setting 'COLOR' in shader VAO.");
			}

			//Uniform texture sampler id
			if(!program->SetUniformi("my_texture_1", mTextureObject1->GetTextureUnit())) //not the handle
				std::cout << "my_texture_1 -------------------------------------> ERROR!!!" << std::endl;

			//Uniform texture sampler id
			if(!program->SetUniformi("my_texture_2", mTextureObject2->GetTextureUnit())) //not the handle
				std::cout << "my_texture_2 -------------------------------------> ERROR!!!" << std::endl;

			report_gl_error();
			std::cout << "--- END UpdateShaderEvent" << std::endl;

		}
		report_gl_error();
	}
	std::cout << "END TEST" << std::endl;
}

void ShaderCallback::Execute(vtkObject *, unsigned long event, void *cbo)
{
	if(!mRenderWindow)
		std::cout << "ERROR NO CONTEXT!!!" << std::endl;
	mRenderWindow->MakeCurrent();

	//test2(event, cbo)
	test(event, cbo); //WORKS!

}

ShaderCallback::ShaderCallback()
{
}

