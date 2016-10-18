#include "openglfixture.h"

#include <iostream>

//OpenGL
#include <GL/glew.h>
#include <GL/glut.h> //Framework on Mac

OpenGLFixture::OpenGLFixture(int argc, char *argv[])
{
	//Source: http://glew.sourceforge.net/basic.html

	//Init
	glutInit(&argc, argv);
	glutCreateWindow("GLEW Test");
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	//Checking OpenGL Version
	if (GLEW_VERSION_2_1)
	{
	   std::cout << "Core extensions of OpenGL 1.1 to 2.1 are available!" << std::endl;
	}

	//Checking Presence of GLSL
	if (glewIsSupported("GL_ARB_shading_language_100"))
	{
	   int major, minor, revision;
	   const GLubyte* sVersion = glGetString(GL_SHADING_LANGUAGE_VERSION_ARB);
	   if (glGetError() == GL_INVALID_ENUM)
	   {
		  major = 1; minor = 0; revision=51;
		  fprintf(stdout, "GLSL version %s\n", "1.051");
	   }
	   else
	   {
		  // parse string sVersion to get major, minor, revision
		   fprintf(stdout, "GLSL version %s\n", sVersion);
	   }
	}
}


