#include "shadercallback.h"

#include <vtkOpenGLHelper.h>
#include <vtkShaderProgram.h>
#include <vtkOpenGLBufferObject.h>
#include <vtkNew.h>
#include <vtkOpenGLVertexArrayObject.h>
#include <vtkShader.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkCubeSource.h>

#include "vtkfixture.h"

#include "cxGLHelpers.h"

//OpenGL
#include <GL/glew.h>
#include <GL/glut.h> //Framework on Ma


ShaderCallback *ShaderCallback::New()
{ return new ShaderCallback; }

void ShaderCallback::test2(unsigned long event, void *cbo)
{
	if(event == vtkCommand::UpdateShaderEvent)
	{
		std::cout << "--- START UpdateShaderEvent" << std::endl;
		vtkOpenGLHelper *cellBO = reinterpret_cast<vtkOpenGLHelper*>(cbo);
		report_gl_error();
		std::cout << "--- 1" << std::endl;

		if(cellBO && cellBO->VAO)
		{
			//Init
			//glutInit(&argc, argv);
			//glutCreateWindow("GLEW Test");
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
			  /* Problem: glewInit failed, something is seriously wrong. */
			  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			}
			fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

			std::cout << "--- 2" << std::endl;


			// An array of 3 vectors which represents 3 vertices
			// One color for each vertex. They were generated randomly.
			static const GLfloat g_color_buffer_data[] = {
				0.583f,  0.771f,  0.014f,
				0.609f,  0.115f,  0.436f,
				0.327f,  0.483f,  0.844f,
				0.822f,  0.569f,  0.201f,
				0.435f,  0.602f,  0.223f,
				0.310f,  0.747f,  0.185f,
				0.597f,  0.770f,  0.761f,
				0.559f,  0.436f,  0.730f,
				0.359f,  0.583f,  0.152f,
				0.483f,  0.596f,  0.789f,
				0.559f,  0.861f,  0.639f,
				0.195f,  0.548f,  0.859f,
				0.014f,  0.184f,  0.576f,
				0.771f,  0.328f,  0.970f,
				0.406f,  0.615f,  0.116f,
				0.676f,  0.977f,  0.133f,
				0.971f,  0.572f,  0.833f,
				0.140f,  0.616f,  0.489f,
				0.997f,  0.513f,  0.064f,
				0.945f,  0.719f,  0.592f,
				0.543f,  0.021f,  0.978f,
				0.279f,  0.317f,  0.505f,
				0.167f,  0.620f,  0.077f,
				0.347f,  0.857f,  0.137f,
				0.055f,  0.953f,  0.042f,
				0.714f,  0.505f,  0.345f,
				0.783f,  0.290f,  0.734f,
				0.722f,  0.645f,  0.174f,
				0.302f,  0.455f,  0.848f,
				0.225f,  0.587f,  0.040f,
				0.517f,  0.713f,  0.338f,
				0.053f,  0.959f,  0.120f,
				0.393f,  0.621f,  0.362f,
				0.673f,  0.211f,  0.457f,
				0.820f,  0.883f,  0.371f,
				0.982f,  0.099f,  0.879f
			};
			std::cout << "--- 3" << std::endl;


			// This will identify our vertex buffer
			GLuint colorbuffer;
			// Generate 1 buffer, put the resulting identifier in vertexbuffer
			glGenBuffers(1, &colorbuffer);
			std::cout << "--- 4" << std::endl;
			// The following commands will talk about our 'vertexbuffer' buffer
			glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
			std::cout << "--- 5" << std::endl;
			// Give our vertices to OpenGL.
			glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
			report_gl_error();
			std::cout << "--- 6" << std::endl;

			GLint index_color = glGetAttribLocation(cellBO->Program->GetHandle(), "COLOR");
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

			glDisableVertexAttribArray(index_color);
			report_gl_error();

			GLint color_frag_out_index = glGetFragDataLocation(cellBO->Program->GetHandle(), "color");
			std::cout << "color_frag_out_index " << color_frag_out_index << std::endl;
			glBindFragDataLocation(cellBO->Program->GetHandle(), color_frag_out_index, "color");




		}
	std::cout << "--- END UpdateShaderEvent" << std::endl;
	}
}


void ShaderCallback::test( unsigned long event, void *cbo)
{
	std::cout << "--- START UpdateShaderEvent" << std::endl;
	vtkOpenGLHelper *cellBO = reinterpret_cast<vtkOpenGLHelper*>(cbo);
	report_gl_error();
	if(cellBO && cellBO->VAO)
	{
		//cellBO->Program->SetUniform1iv("my_texture[0]", 1, &texture_0);
		//cellBO->Program->SetUniform1iv("my_texture[1]", 1, &texture_1);

		float verts[24] = {0,0,0, 1,0,0, 1,0,1, 1,1,1, 0,1,1, 0,1,0, 1,1,0, 0,0,1};
		unsigned int numVerts = 8;
		//GLuint *iboData;
		//unsigned int numIndices;
		//float *tcoords = (float*)&tcoords_v;
		//char *tcoords = 0;
		report_gl_error();
		vtkShaderProgram *program = cellBO->Program;
		vtkOpenGLVertexArrayObject *vao = cellBO->VAO;

		//std::cout << "Vertex shader: " << program->GetVertexShader()->GetSource() << std::endl;

		/*vtkNew<vtkOpenGLBufferObject> vbo;
		vbo->Upload(&verts, numVerts*3, vtkOpenGLBufferObject::ArrayBuffer);
		vao->Bind();
		if (!vao->AddAttributeArray(program, vbo.Get(), "vertexMC", 0, sizeof(float)*3, VTK_FLOAT, 3, false))
		{
			vtkGenericWarningMacro(<< "Error setting 'vertexMC' in shader VAO.");
		}*/
		report_gl_error();
		//if (tcoords)
		//{
		float tcoords_v[4] = {1.0f, 0.0f, 0.0f, 1.0f};

		vtkNew<vtkOpenGLBufferObject> tvbo;
		report_gl_error();
		tvbo->Upload(&tcoords_v, sizeof(float)*4, vtkOpenGLBufferObject::ArrayBuffer);
		if (!vao->AddAttributeArray(program, tvbo.Get(), "TexCoords", 0, 0, VTK_FLOAT, 4, false))
		{
			vtkGenericWarningMacro(<< "Error setting 'TexCoords' in shader VAO.");
		}
		//}
		report_gl_error();





		std::cout << "--- END UpdateShaderEvent" << std::endl;

	}
}



void ShaderCallback::Execute(vtkObject *, unsigned long event, void *cbo)
{

	mRenderWindow->MakeCurrent();
	test2(event, cbo);

}
/*
 *   bool AddAttributeArray(vtkShaderProgram *program,
						 vtkOpenGLBufferObject *buffer,
						 const std::string &name,
						 int offset, size_t stride,
						 int elementType, int elementTupleSize, bool normalize)
*/

ShaderCallback::ShaderCallback()
{
}

