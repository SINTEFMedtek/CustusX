#include "vtkfixture.h"

//OpenGL
#include <GL/glew.h>
#include <GL/glut.h> //Framework on Mac

//VTK
#include <vtkCubeSource.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkNew.h>
#include <vtkTextureObject.h>
#include <vtkOpenGLVertexArrayObject.h>
#include <vtkOpenGLProperty.h>
#include <vtkOpenGLBufferObject.h>

#include "cxGLHelpers.h"

#include "texture.h"
#include "shadercallback.h"

vtkfixture::vtkfixture()
{

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


void vtkfixture::printVtkOpenGLRenderWindowInfo(vtkSmartPointer<vtkOpenGLRenderWindow> opengl_renderwindow)
{
	std::cout << "context support for open gl core 3.2: " << vtkOpenGLRenderWindow::GetContextSupportsOpenGL32() << std::endl;
	std::cout << "context was created at: " << opengl_renderwindow->GetContextCreationTime() << std::endl;
	report_gl_error();
}

vtkSmartPointer<vtkTextureObject> vtkfixture::createTextureObject(unsigned int depth, unsigned int width, int dataType, int numComps, unsigned int height, vtkSmartPointer<vtkOpenGLRenderWindow> opengl_renderwindow, void *data)
{
	vtkNew<vtkTextureObject> texture_object;
	texture_object->SetContext(opengl_renderwindow);

	if(!texture_object->Create3DFromRaw(width, height, depth, numComps, dataType, data))
		std::cout << "---------------------------------------- > Error creating 3D texture" << std::endl;

	//6403 == GL_RED 0x1903
	//6407 == GL_RGB 0x1907
	//6408 == GL_RGBA 0x1908
	std::cout << texture_object->GetFormat(dataType, numComps, true) << std::endl;

	texture_object->Activate();

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

void vtkfixture::createVTKWindowWithCylinderSourceWith3DTexture()
{
	// --------------------------------------------------------------------------------

	//===========
	// Create mesh
	//===========
	vtkSmartPointer<vtkCubeSource> cube = vtkSmartPointer<vtkCubeSource>::New();

	// --------------------------------------------------------------------------------

	//===========
	// Create mapper
	//===========
	// The mapper is responsible for pushing the geometry into the graphics library.
	// It may also do color mapping, if scalars or other attributes are defined.
	vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper = vtkSmartPointer<vtkOpenGLPolyDataMapper>::New();
	mapper->SetInputConnection(cube->GetOutputPort());

	// --------------------------------------------------------------------------------

	//===========
	// Modify vertex shader declarations
	//===========
	mapper->AddShaderReplacement(
		vtkShader::Vertex,
		"//VTK::PositionVC::Dec", // replace the normal block
		true, // before the standard replacements
		"//VTK::PositionVC::Dec\n" // we still want the default
		"attribute vec3 COLOR_VSIN;\n"
		"attribute vec3 TEXTURE_COORDINATE_VSIN;\n"
		"varying vec3 COLOR_VSOUT;\n"
		"varying vec3 TEXTURE_COORDINATE_VSOUT;\n",
		false // only do it once
	);

	//===========
	// Modify vertex shader implementations
	//===========
	mapper->AddShaderReplacement(
		vtkShader::Vertex,
		"//VTK::PositionVC::Impl", // replace the normal block
		true, // before the standard replacements
		"//VTK::PositionVC::Impl\n" // we still want the default
		"COLOR_VSOUT = COLOR_VSIN;\n"
		"TEXTURE_COORDINATE_VSOUT = TEXTURE_COORDINATE_VSIN;\n",
		false // only do it once
	);

	// --------------------------------------------------------------------------------

	//===========
	// Replace the fragment shader
	//===========
	std::string fragment_shader =
		"//VTK::System::Dec\n"  // always start with this line
		"//VTK::Output::Dec\n"  // always have this line in your FS
		"in vec3 COLOR_VSOUT;\n"
		"in vec3 TEXTURE_COORDINATE_VSOUT;\n"
		"uniform sampler3D my_texture_1;\n"
		"uniform sampler3D my_texture_2;\n"
		"out vec4 color;\n"
		"void main () {\n"
		"	vec4 color_1 = texture(my_texture_1, TEXTURE_COORDINATE_VSOUT);\n"
		"	vec4 color_2 = texture(my_texture_2, TEXTURE_COORDINATE_VSOUT);\n"
		"	color = vec4(color_1.xy, color_2.zw);\n"
		"}\n";
	mapper->SetFragmentShaderCode(fragment_shader.c_str());
	//printActiveVertexAndFragmentShader(mapper);

	// --------------------------------------------------------------------------------

	//===========
	// Create an actor for the mesh
	//===========
	// The actor is a grouping mechanism: besides the geometry (mapper), it
	// also has a property, transformation matrix, and/or texture map.
	// Here we set its color and rotate it around the X and Y axes.
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	//actor->GetProperty()->SetColor(1.0000, 0.3882, 0.2784);
	actor->RotateX(30.0);
	actor->RotateY(-45.0);

	// --------------------------------------------------------------------------------

	//===========
	// Create the renderer
	//===========
	// The renderer generates the image
	// which is then displayed on the render window.
	// It can be thought of as a scene to which the actor is added
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(actor);
	renderer->SetBackground(0.1, 0.2, 0.4);
	// Zoom in a little by accessing the camera and invoking its "Zoom" method.
	renderer->ResetCamera();
	renderer->GetActiveCamera()->Zoom(1.5);

	// --------------------------------------------------------------------------------

	//===========
	// Create the render window
	//===========
	// The render window is the actual GUI window
	// that appears on the computer screen
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetSize(200, 200);
	renderWindow->AddRenderer(renderer);

	// --------------------------------------------------------------------------------

	//===========
	// Create the opengl context
	//===========
	// We call Render to create the OpenGL context as it will
	// be needed by the texture object
	//http://www.vtk.org/gitweb?p=VTK.git;a=blob;f=Rendering/OpenGL2/Testing/Cxx/TestCubeMap.cxx
	renderWindow->Render();

	// --------------------------------------------------------------------------------

	//===========
	// Init GLEW
	//===========
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  // Problem: glewInit failed, something is seriously wrong.
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	report_gl_error();

	printOpenGLVersion();

	// --------------------------------------------------------------------------------

	//===========
	// Allocate buffer and upload color data to opengl
	//===========
	//Only need to allocate and upload once
	std::cout << "ALLOCATING BUFFER FOR COLOR" << std::endl;
	vtkNew<vtkOpenGLBufferObject> tvbo;
	tvbo->GenerateBuffer(vtkOpenGLBufferObject::ArrayBuffer);
	if(!tvbo->Bind())
		std::cout << "tvbo not bind" << std::endl;
	report_gl_error();

	std::cout << "UPLOADING COLOR DATA" << std::endl;
	if(!tvbo->Upload(
				color_data,
				numberOfColors*numberOfComponentsPerColor,  //how many floats to upload! (aka number of floats in the vector)
				vtkOpenGLBufferObject::ArrayBuffer
				))
	{
		vtkGenericWarningMacro(<< "Error uploading 'g_color_buffer_data'.");
	}
	report_gl_error();

	// --------------------------------------------------------------------------------

	//===========
	// Allocate buffer and upload texture coordinate data to opengl
	//===========
	//Only need to allocate and upload once
	std::cout << "ALLOCATING BUFFER FOR TEXTURE COORDINATES" << std::endl;
	vtkNew<vtkOpenGLBufferObject> texvbo;
	texvbo->GenerateBuffer(vtkOpenGLBufferObject::ArrayBuffer);
	if(!texvbo->Bind())
		std::cout << "texvbo not bind" << std::endl;
	report_gl_error();

	std::cout << "UPLOADING TEXTURE DATA" << std::endl;
	numberOfComponentsPerTexture = 3;
	if(!texvbo->Upload(
				texture_data,
				numberOfTextureCoordinates*numberOfComponentsPerTexture,  //how many floats to upload! (aka number of floats in the vector)
				vtkOpenGLBufferObject::ArrayBuffer
				))
	{
		vtkGenericWarningMacro(<< "Error uploading 'texture_data'.");
	}
	report_gl_error();

	// --------------------------------------------------------------------------------

	//===========
	// Get OpenGLRenderWindow to be able to make its context current
	//===========
	vtkSmartPointer<vtkOpenGLRenderWindow> opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(renderWindow.Get());
	if(!opengl_renderwindow)
		std::cout << "not opengl_renderwindow" << std::endl;
	else
	{
		printVtkOpenGLRenderWindowInfo(opengl_renderwindow);
	}
	//opengl_renderwindow->MakeCurrent(); //set current context

	// --------------------------------------------------------------------------------

	//===========
	// Create 3D texture objects
	//===========
	std::cout << "ALLOCATING AND UPLOADING TEXTURE OBJECT" << std::endl;
	unsigned int width = 4;
	unsigned int height = 4;
	unsigned int depth = 4;
	int dataType = VTK_FLOAT;
	//void *data = generateTexture<unsigned char>(width, height, depth, 200, 140, 0, 200);  //numComps=4, dataType=VTK_UNSIGNED_CHAR (WORKS!!!)

	int numComps = 3;
	void *data1 = (void*)color_data; //numComps=3, dataType = VTK_FLOAT  //4*4*4*3 = 192 < 243 (see shadercallback.h) (WORKS!!!)
	vtkSmartPointer<vtkTextureObject> texture_object_1 = createTextureObject(depth, width, dataType, numComps, height, opengl_renderwindow, data1);

	numComps = 4;
	void *data2 = generateTexture<float>(width, height, depth, 0.5f, 0.1f, 0.2f, 1.0f);  //numComps=4, dataType=VTK_FLOAT (WORKS!!!)
	vtkSmartPointer<vtkTextureObject> texture_object_2 = createTextureObject(depth, width, dataType, numComps, height, opengl_renderwindow, data2);

	// --------------------------------------------------------------------------------

	//===========
	// Setup a callback to change some uniforms and attributes
	//===========
	vtkSmartPointer<ShaderCallback> callback = vtkSmartPointer<ShaderCallback>::New();
	callback->mRenderWindow = opengl_renderwindow; //used to set current context
	//callback->mCube = cube; // not used
	callback->mTvbo = texvbo.Get(); //used to set in/attribute COLOR_VSIN in vertex shader
	callback->mTexvbo = texvbo.Get(); //used to set in/attribute TEXTURE_COORDINATE_VSIN in vertex shader
	callback->mTextureObject1 = texture_object_1.Get(); //used to set sampler in frament shader
	callback->mTextureObject2 = texture_object_2.Get(); //used to set sampler in frament shader

	mapper->AddObserver(vtkCommand::UpdateShaderEvent,callback);

	// --------------------------------------------------------------------------------

	//===========
	// Setup a render window interactor
	//===========
	// The render window interactor captures mouse events
	// and will perform appropriate camera or actor manipulation
	// depending on the nature of the events.
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	// --------------------------------------------------------------------------------

	//===========
	// Render
	//===========
	// This starts the event loop and as a side effect causes an initial render.
	renderWindowInteractor->Start();

	// --------------------------------------------------------------------------------

	//===========
	// Clean up
	//===========
	renderer->ReleaseGraphicsResources(renderWindow);

	//TODO:
	//delete texture coordinate buffer
	//delete color buffer
	//delete texture object

	// --------------------------------------------------------------------------------
}

