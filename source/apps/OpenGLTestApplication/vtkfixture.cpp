#include "vtkfixture.h"

//OpenGL
#include <GL/glew.h>
#include <GL/glut.h> //Framework on Mac

//VTK
#include <vtkCylinderSource.h>
#include <vtkCubeSource.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkCocoaRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkNew.h>
#include <vtkTextureObject.h>

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

void vtkfixture::printActiveVertexAndFragmentShader(vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper)
{
	char *vertexShader = 	mapper->GetVertexShaderCode();
	if(vertexShader)
	{
		std::cout << "VERTEX SHADER:" << std::endl;
		std::cout << vertexShader << std::endl;
	}

	char *fragmentShader = mapper->GetFragmentShaderCode();
	if(fragmentShader)
	{
		std::cout << "FRAGMENT SHADER:" << std::endl;
		std::cout << fragmentShader << std::endl;
	}
}

void vtkfixture::printOpenGLVersion()
{
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "OpenGL version " << version << std::endl;
}

void vtkfixture::opengl_upload3dTextures()
{
	std::cout << "1" << std::endl;
	GLuint textures[2];
	std::cout << "2" << std::endl;
	glGenTextures(2, textures);
	std::cout << "3" << std::endl;
	report_gl_error();

	std::cout << "4" << std::endl;
	int width, height, depth = 30;
	unsigned char* image1 = generateTexture(width, height, depth, 0.2f);
	unsigned char* image2 = generateTexture(width, height, depth, 0.8f);

	//----- TEXTURE1 -----
	std::cout << "5" << std::endl;
	GLint current_active_texture;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &current_active_texture);
	std::cout << "5.1, active_texture is: " << current_active_texture << ", tryin to set it to " << GL_TEXTURE0 << std::endl;
	if(!current_active_texture != GL_TEXTURE0)
		glActiveTexture(GL_TEXTURE0);
	std::cout << "5.5" << std::endl;
	glBindTexture(GL_TEXTURE_3D, textures[0]);
	std::cout << "6, bindTexture is now: " << textures[0] << std::endl;
	//image = SOIL_load_image("sample.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, image1);
	//SOIL_free_image_data(image);
	//glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

	std::cout << "7" << std::endl;
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	std::cout << "8" << std::endl;
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	std::cout << "9" << std::endl;
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	std::cout << "10" << std::endl;
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//----- TEXTURE2 -----
	std::cout << "11" << std::endl;
	glActiveTexture(GL_TEXTURE1);
	std::cout << "12" << std::endl;
	glBindTexture(GL_TEXTURE_3D, textures[1]);
	std::cout << "13" << std::endl;
	//image = SOIL_load_image("sample.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, image2);
	//SOIL_free_image_data(image);
	//glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 1);

	std::cout << "14" << std::endl;
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	std::cout << "15" << std::endl;
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	std::cout << "16" << std::endl;
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	std::cout << "17" << std::endl;
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	std::cout << "18" << std::endl;

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

void vtkfixture::printVtkOpenGLRenderWindowInfo(vtkSmartPointer<vtkOpenGLRenderWindow> opengl_renderwindow)
{
	std::cout << "context support for open gl core 3.2: " << vtkOpenGLRenderWindow::GetContextSupportsOpenGL32() << std::endl;
	std::cout << "context was created at: " << opengl_renderwindow->GetContextCreationTime() << std::endl;
	report_gl_error();
}

void vtkfixture::createVTKWindowWithCylinderSourceWith3DTexture(int argc, char *argv[])
{
	vtkSmartPointer<vtkCubeSource> cube = vtkSmartPointer<vtkCubeSource>::New();
	//cube->SetBounds(0,1,0,1,0,1);

	vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper = createOpenGLPolyDataMapper();
	mapper->SetInputConnection(cube->GetOutputPort());

	// Modify vertex shader
	// Add new code in default VTK vertex shader
	mapper->AddShaderReplacement(
		vtkShader::Vertex,
		"//VTK::PositionVC::Dec", // replace the normal block
		true, // before the standard replacements
		"//VTK::PositionVC::Dec\n" // we still want the default
		"in vec3 COLOR;\n"
		"in vec3 VERTICES;\n"
		"//attribute vec3 TexCoords;\n"
		"out vec3 COLOR_OUT;\n",
		false // only do it once
	);

	mapper->AddShaderReplacement(
		vtkShader::Vertex,
		"//VTK::PositionVC::Impl", // replace the normal block
		true, // before the standard replacements
		"//VTK::PositionVC::Impl\n" // we still want the default
		"COLOR_OUT = COLOR;\n"
		"//COLOR_OUT = vec4(1.0, 0.0, 0.0, 1.0);\n", //this works
		false // only do it once
	);

	// Replace the fragment shader
	std::string fragment_shader =
		"//VTK::System::Dec\n"  // always start with this line
		"//VTK::Output::Dec\n"  // always have this line in your FS
		"//uniform sampler3D my_texture[2];\n"
		"in vec3 COLOR_OUT;\n"
		"out vec3 color;\n"
		"void main () {\n"
		"//gl_FragData[0] = texture3D(my_texture[0], vec3(0.5,0.5,0.5));\n"
		"//gl_FragData[0] = vec4(0.8f,0.8f,0.8f,1);\n"
		"//gl_FragData[0] = vec4(COLOR_OUT, 1.0f);\n"
		"color = COLOR_OUT;\n"
		"//color = vec3(1.0,0.0,0.0);\n"
		"}\n";
	mapper->SetFragmentShaderCode(fragment_shader.c_str());
	printActiveVertexAndFragmentShader(mapper);

	vtkSmartPointer<vtkActor> actor = createActor(mapper);

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
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetSize(200, 200);
	renderWindow->AddRenderer(renderer);

	// We call Render to create the OpenGL context as it will
	// be needed by the texture object
	//http://www.vtk.org/gitweb?p=VTK.git;a=blob;f=Rendering/OpenGL2/Testing/Cxx/TestCubeMap.cxx
	renderWindow->Render();
	report_gl_error();

	//Clear graphics resources
	//renderer->ReleaseGraphicsResources(renderWindow);

	printOpenGLVersion();

	vtkSmartPointer<vtkOpenGLRenderWindow> opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(renderWindow.Get());
	if(!opengl_renderwindow)
		std::cout << "not opengl_renderwindow" << std::endl;
	else
	{
		printVtkOpenGLRenderWindowInfo(opengl_renderwindow);
		//opengl_upload3dTextures();
		//vtk_upload3dTextures();
	}
	opengl_renderwindow->MakeCurrent();

	/*
	// Create a texture object from our set of cube map images
	vtkNew<vtkTextureObject> texObject;
	texObject->SetContext(opengl_renderwindow);
	//texObject->CreateCubeFromRaw(dims[0], dims[1], 3, imgs[0]->GetScalarType(), images);
	unsigned int width = 3;
	unsigned int height = 3;
	unsigned int depth = 3;
	int numComps = 1;
	int dataType = VTK_FLOAT;
	void *data = generateTexture(width, height, depth, 0.2f);
	texObject->Create3DFromRaw(width, height, depth, numComps, dataType, data);
	*/

	// Setup a callback to change some uniforms
	vtkSmartPointer<ShaderCallback> callback = vtkSmartPointer<ShaderCallback>::New();
	callback->mRenderWindow = opengl_renderwindow;
	callback->mCube = cube;
	//mapper->AddObserver(vtkCommand::EndEvent,callback);

	mapper->AddObserver(vtkCommand::UpdateShaderEvent,callback);

	//TODO
	// 1: upload vertexMC to VAO
	// 2: modify vertex shader to pass on vertexMC
	// 3: read vertexMC in fragment shader


	// The render window interactor captures mouse events
	// and will perform appropriate camera or actor manipulation
	// depending on the nature of the events.
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
	  vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	// This starts the event loop and as a side effect causes an initial render.
	renderWindowInteractor->Start();

}

