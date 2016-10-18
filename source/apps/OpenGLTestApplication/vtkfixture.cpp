#include "vtkfixture.h"

//OpenGL
#include <GL/glew.h>
#include <GL/glut.h> //Framework on Mac

//VTK
#include <vtkCylinderSource.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include "cxGLHelpers.h"

#include "texture.h"
#include "shadercallback.h"

vtkfixture::vtkfixture()
{

}

vtkSmartPointer<vtkCylinderSource> vtkfixture::createPolygonalCylinderModel()
{
	// This creates a polygonal cylinder model with eight circumferential facets
	// (i.e, in practice an octagonal prism).
	vtkSmartPointer<vtkCylinderSource> cylinder =
	  vtkSmartPointer<vtkCylinderSource>::New();
	cylinder->SetResolution(8);

	return cylinder;
}

vtkSmartPointer<vtkOpenGLPolyDataMapper> vtkfixture::createOpenGLPolyDataMapper()
{
	// The mapper is responsible for pushing the geometry into the graphics library.
	// It may also do color mapping, if scalars or other attributes are defined.
	vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper =
	  vtkSmartPointer<vtkOpenGLPolyDataMapper>::New();

	return mapper;
}

vtkSmartPointer<vtkActor> vtkfixture::createActor(vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper)
{
	// The actor is a grouping mechanism: besides the geometry (mapper), it
	// also has a property, transformation matrix, and/or texture map.
	// Here we set its color and rotate it around the X and Y axes.
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	//actor->GetProperty()->SetColor(1.0000, 0.3882, 0.2784);
	actor->RotateX(30.0);
	actor->RotateY(-45.0);

	return actor;
}

void vtkfixture::startRender(vtkSmartPointer<vtkActor> actor)
{

	// The renderer generates the image
	// which is then displayed on the render window.
	// It can be thought of as a scene to which the actor is added
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(actor);
	renderer->SetBackground(0.1, 0.2, 0.4);
	// Zoom in a little by accessing the camera and invoking its "Zoom" method.
	renderer->ResetCamera();
	renderer->GetActiveCamera()->Zoom(1.5);

	// The render window is the actual GUI window
	// that appears on the computer screen
	vtkSmartPointer<vtkRenderWindow> renderWindow =
	  vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetSize(200, 200);
	renderWindow->AddRenderer(renderer);

	// The render window interactor captures mouse events
	// and will perform appropriate camera or actor manipulation
	// depending on the nature of the events.
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
	  vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	// This starts the event loop and as a side effect causes an initial render.
	renderWindowInteractor->Start();
}

std::string vtkfixture::getCustomFragmentShader()
{
	std::string fragment_shader =
	"//VTK::System::Dec\n"  // always start with this line
	"//VTK::Output::Dec\n"  // always have this line in your FS
	"uniform sampler3D my_texture[2];\n"
	"void main () {\n"
	"gl_FragData[0] = texture3D(my_texture[0], vec3(0.5,0.5,0.5));\n"
	"}\n";

	return fragment_shader;
}

void vtkfixture::printActiveFragmentShader(vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper)
{
	char *fragmentShader = mapper->GetFragmentShaderCode();
	if(fragmentShader)
		std::cout << mapper->GetFragmentShaderCode() << std::endl;
}

void vtkfixture::upload3dTextures()
{
	GLuint textures[2];
	glGenTextures(2, textures);
	report_gl_error();

	int width, height, depth = 30;
	unsigned char* image1 = generateTexture(width, height, depth, 0.2);
	unsigned char* image2 = generateTexture(width, height, depth, 0.8);

	//----- TEXTURE1 -----
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, textures[0]);
	//image = SOIL_load_image("sample.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_UNSIGNED_BYTE, image1);
	//SOIL_free_image_data(image);
	//glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//----- TEXTURE2 -----
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, textures[1]);
	//image = SOIL_load_image("sample.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	//SOIL_free_image_data(image);
	//glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 1);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

void vtkfixture::createVTKWindowWithCylinderSource()
{
	//Source: http://www.vtk.org/Wiki/VTK/Examples/Cxx/Rendering/Cylinder

	vtkSmartPointer<vtkCylinderSource> cylinder = createPolygonalCylinderModel();

	vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper = createOpenGLPolyDataMapper();
	mapper->SetInputConnection(cylinder->GetOutputPort());

	vtkSmartPointer<vtkActor> actor = createActor(mapper);

	startRender(actor);
}

void vtkfixture::createVTKWindowWithCylinderSourceWith3DTexture(int argc, char *argv[])
{
	//TODO OpenGL context is not created yet.
	/*
	glutInit(&argc, argv);
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  // Problem: glewInit failed, something is seriously wrong.
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	*/

	//upload3dTextures();

	vtkSmartPointer<vtkCylinderSource> cylinder = createPolygonalCylinderModel();

	vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper = createOpenGLPolyDataMapper();
	mapper->SetInputConnection(cylinder->GetOutputPort());


	// Setup a callback to change some uniforms
	vtkSmartPointer<ShaderCallback> callback = vtkSmartPointer<ShaderCallback>::New();
	mapper->AddObserver(vtkCommand::UpdateShaderEvent,callback);
	//mapper->AddObserver(vtkCommand::EndEvent,callback);

	// Modify the fragmentshader
	std::string fragment_shader = getCustomFragmentShader();
	mapper->SetFragmentShaderCode(fragment_shader.c_str());

	printActiveFragmentShader(mapper);

	vtkSmartPointer<vtkActor> actor = createActor(mapper);

	startRender(actor);

}

