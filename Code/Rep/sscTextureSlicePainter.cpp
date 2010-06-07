/*
 * vmTextureSlicePainter.cpp
 *
 *  Created on: Oct 13, 2009
 *      Author: petterw
 */

#include "sscTextureSlicePainter.h"

#include <QtCore>
//#include <vtkProperty.h>
#include <vtkOpenGLExtensionManager.h>
//#include <vtkOpenGLRepresentationPainter.h>
//#include <vtkActor.h>
#include <vtkRenderer.h>
//
//#include "sscImage.h"
//#include "sscGPUImageBuffer.h"
//
//
//#include <vtkActor.h>
#include <vtkShaderProgram2.h>
#include <vtkShader2.h>
#include <vtkShader2Collection.h>
#include <vtkUniformVariables.h>
#include <vtkObjectFactory.h>
//#include <vtkPolyData.h>
//#include <vtkProperty.h>
//#include <vtkRenderer.h>
#include <vtkOpenGLRenderWindow.h>
//#include <vtkSmartPointer.h>
//#include <vtkImageData.h>
#include <vtkgl.h>
//#include <vtkPointData.h>
//#include <vtkUnsignedCharArray.h>
//#include <vtkUnsignedShortArray.h>

#include "/usr/include/X11/Xlib.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <GL/glext.h>
#endif

#include "sscGPUImageBuffer.h"
//#include "SonoWand.h"
//#include "QtUtilities.h"
#include "sscTypeConversions.h"
#include "sscGLHelpers.h"


//---------------------------------------------------------
namespace ssc
{
//---------------------------------------------------------

vtkStandardNewMacro(TextureSlicePainter)
;
vtkCxxRevisionMacro(TextureSlicePainter, "$Revision: 647 $")
;

class SingleVolumePainterHelper
{
	ssc::GPUImageDataBufferPtr mVolumeBuffer;
	ssc::GPUImageLutBufferPtr mLutBuffer;
	int mIndex;
	float mWindow;
	float mLevel;
	float mLLR;
	float mAlpha;
	//int mLutSize;

public:
	explicit SingleVolumePainterHelper(int index)
	{
		//GPUImageBufferPtr createGPUImageBuffer(vtkImageDataPtr volume, vtkUnsignedCharArrayPtr lut);
		//mBuffer.reset(new GPUImageBufferImpl);
		mIndex = index;
		//mLutSize = -1;
	}
	SingleVolumePainterHelper()
	{
		mIndex = -1;
		//mLutSize = -1;
	}
	~SingleVolumePainterHelper()
	{

	}
	void SetBuffer(ssc::GPUImageDataBufferPtr buffer)
	{
		mVolumeBuffer = buffer;
	}
	void SetBuffer(ssc::GPUImageLutBufferPtr buffer)
	{
		mLutBuffer = buffer;
	}
	void SetColorAttribute(float window, float level, float llr,float alpha)
	{
		mWindow = window;
		mLevel = level;
		mLLR = llr;
		mAlpha = alpha;
		//Logger::log("vm.log", " mWindow: "+ string_cast(mWindow)+ ", mLevel: "+ string_cast(mLevel)+ ", mLLR: "+ string_cast(mLLR)+", mAlpha: "+ string_cast(mAlpha)  );
	}
	void initializeRendering()
	{
		if (mVolumeBuffer)
		{
			mVolumeBuffer->allocate();
		}
	}
	void eachPrepareRendering()
	{
		if (mLutBuffer)
		{
			mLutBuffer->allocate();
		}
	}
	void eachRenderInternal(vtkSmartPointer<vtkShaderProgram2> shader)
	{
		if (!mVolumeBuffer)
		{
			return;
		}

		mVolumeBuffer->bind(mIndex);

		int texture = 2*mIndex; //texture unit 1
		int lut = 2*mIndex+1; //texture unit 1
		int lutSize = 0;

		if (mLutBuffer)
		{
			mLutBuffer->bind(mIndex);
			lutSize = mLutBuffer->getLutSize();
		}

		shader->GetUniformVariables()->SetUniformi(cstring_cast("texture"+string_cast(mIndex)), 1, &texture);
		shader->GetUniformVariables()->SetUniformi(cstring_cast("lut"+string_cast(mIndex)), 1, &lut);
		shader->GetUniformVariables()->SetUniformi(cstring_cast("lutsize"+string_cast(mIndex)), 1, &lutSize);
		shader->GetUniformVariables()->SetUniformf(cstring_cast("llr"+string_cast(mIndex)), 1, &mLLR);
		shader->GetUniformVariables()->SetUniformf(cstring_cast("level"+string_cast(mIndex)), 1, &mLevel);
		shader->GetUniformVariables()->SetUniformf(cstring_cast("window"+string_cast(mIndex)), 1, &mWindow);
		shader->GetUniformVariables()->SetUniformf(cstring_cast("alpha"+string_cast(mIndex)), 1, &mAlpha);
		report_gl_error();
	}

};

class TextureSlicePainter::vtkInternals
{
public:
	Display* mCurrentContext;

	vtkWeakPointer<vtkRenderWindow> LastContext;
	int LastViewportSize[2];
	int ViewportExtent[4];

	vtkSmartPointer<vtkShaderProgram2> Shader;
	vtkSmartPointer<vtkGLSLShaderDeviceAdapter2> ShaderDevice;

	std::vector<SingleVolumePainterHelper> mElement;

	SingleVolumePainterHelper& safeIndex(int index)
	{
		if ((int)mElement.size() <= index)
		{
			mElement.resize(index+1);
			mElement[index] = SingleVolumePainterHelper(index);
		}
		return mElement[index];
	}

	vtkInternals()
	{

	}
	~vtkInternals()
	{
		mElement.clear();
	}
	void ClearGraphicsResources()
	{
		if (this->Shader != 0)
		{
			this->Shader->ReleaseGraphicsResources();
			this->Shader = 0;
		}
	}
};
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------
TextureSlicePainter::TextureSlicePainter()
{
	mInternals = new vtkInternals();
	QFile fp(QString("/Data/Resources/Shaders/Texture3DOverlay.frag"));
	//QFile fp(QString("/home/chrask/workspace/ssc/Sandbox/Texture3DOverlay.frag"));

	if (fp.exists())
	{
		fp.open(QFile::ReadOnly);
		QTextStream shaderfile(&fp);
		mSource = shaderfile.readAll().toStdString();
		fp.close();
	}
	else
	{
		std::cout << "TextureSlicer can't read shaderfile [" << fp.fileName() << "]" << std::endl;
		//SW_ERROR("TextureSlicer can't read shaderfile ");
	}

}

TextureSlicePainter::~TextureSlicePainter()
{
	if (mInternals->LastContext)
	{
	    this->ReleaseGraphicsResources(this->LastWindow);
	}
	//Logger::log("vm.log", "::~TextureSlicePainter()");

	delete mInternals;
	mInternals = 0;
}

void TextureSlicePainter::ReleaseGraphicsResources(vtkWindow* win)
{
	mInternals->ClearGraphicsResources(); //the shader
	mInternals->LastContext = 0;
	this->Superclass::ReleaseGraphicsResources(win);
}

void TextureSlicePainter::PrepareForRendering(vtkRenderer* renderer, vtkActor* actor)
{

	if (!CanRender(renderer, actor))
	{
		std::cout << "Can not Render !!!!!!!!!!!." << endl;
		mInternals->ClearGraphicsResources();
		mInternals->LastContext = 0;
		this->Superclass::PrepareForRendering(renderer, actor);
		return;
	}

	vtkRenderWindow* renWin = renderer->GetRenderWindow();
	if (mInternals->LastContext != renWin)
	{
		mInternals->ClearGraphicsResources();
	}
	mInternals->LastContext = renWin;

	report_gl_error();

	vtkOpenGLRenderWindow *context = vtkOpenGLRenderWindow::SafeDownCast(renWin);
	//std::cout << "Load extension!" << endl;
	if (!LoadRequiredExtensions(context->GetExtensionManager()))
	{
		std::cout << "Missing required EXTENSION!!!!!!!!!!!." << endl;
		return;
	}

	if (!mInternals->Shader)
	{

		vtkShaderProgram2Ptr pgm = vtkShaderProgram2Ptr::New();
		pgm->SetContext(static_cast<vtkOpenGLRenderWindow *> (renWin));

		vtkShader2Ptr s2 = vtkShader2Ptr::New();
		s2->SetType(VTK_SHADER_TYPE_FRAGMENT);
		s2->SetSourceCode(mSource.c_str());
		s2->SetContext(pgm->GetContext());
		pgm->GetShaders()->AddItem(s2);
		mInternals->Shader = pgm;
		report_gl_error();
		for (unsigned i = 0; i < mInternals->mElement.size(); ++i)
		{
			mInternals->mElement[i].initializeRendering();
		}
	}

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

	report_gl_error();
	for (unsigned i = 0; i < mInternals->mElement.size(); ++i)
	{
		mInternals->mElement[i].eachPrepareRendering();
	}
	report_gl_error();

	this->Superclass::PrepareForRendering(renderer, actor);
	report_gl_error();

}

void TextureSlicePainter::RenderInternal(vtkRenderer* renderer, vtkActor* actor, unsigned long typeflags,
		bool forceCompileOnly)
{
	// Save context state to be able to restore.
	mInternals->Shader->Build();
	if (mInternals->Shader->GetLastBuildStatus() != VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
	{
		vtkErrorMacro("Pass Two failed.");
		abort();
	}

	int layers = mInternals->mElement.size();
	mInternals->Shader->GetUniformVariables()->SetUniformi("layers", 1, &layers);

	for (unsigned i = 0; i < mInternals->mElement.size(); ++i)
	{
		mInternals->mElement[i].eachRenderInternal(mInternals->Shader);
	}

	mInternals->Shader->Use();
	report_gl_error();

	if (!mInternals->Shader->IsValid())
	{
		vtkErrorMacro(<<" validation of the program failed: "<< mInternals->Shader->GetLastValidateLog());
	}

	this->Superclass::RenderInternal(renderer, actor, typeflags, forceCompileOnly);

	mInternals->Shader->Restore();
	//mInternals->Shader->RestoreFixedPipeline();

	glDisable(vtkgl::TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);

}

bool TextureSlicePainter::CanRender(vtkRenderer*, vtkActor*)
{
	return true;
}

bool TextureSlicePainter::LoadRequiredExtension(vtkOpenGLExtensionManager* mgr, std::string id)
{
	bool loaded = mgr->LoadSupportedExtension(cstring_cast(id));
	if (!loaded)
		std::cout << "TextureSlicePainter Error: GL extension " + id + " not found" << std::endl;
	return loaded;
}

bool TextureSlicePainter::LoadRequiredExtensions(vtkOpenGLExtensionManager* mgr)
{
	GLint value[2];
	glGetIntegerv(vtkgl::MAX_TEXTURE_COORDS,value);
	if( value[0] < 8)
	{
		std::cout<<"GL_MAX_TEXTURE_COORDS="<<value[0]<<" . Number of texture coordinate sets. Min is 2."<<std::endl;
	}
	return (LoadRequiredExtension(mgr, "GL_VERSION_2_0")
			&& LoadRequiredExtension(mgr, "GL_VERSION_1_5")
//			&& mgr->LoadSupportedExtension(mgr, "GL_EXT_texture3D")
			&& LoadRequiredExtension(mgr, "GL_ARB_vertex_buffer_object")
			&& LoadRequiredExtension(mgr, "GL_EXT_texture_buffer_object"));
}

void TextureSlicePainter::SetVolumeBuffer(int index, ssc::GPUImageDataBufferPtr buffer)
{
	mInternals->safeIndex(index).SetBuffer(buffer);
}

void TextureSlicePainter::SetLutBuffer(int index, ssc::GPUImageLutBufferPtr buffer)
{
	mInternals->safeIndex(index).SetBuffer(buffer);
}

void TextureSlicePainter::SetColorAttribute(int index, float window, float level, float llr,float alpha)
{
	mInternals->safeIndex(index).SetColorAttribute(window, level, llr, alpha);
}
void TextureSlicePainter::releaseGraphicsResources(int index)
{
	//mInternals->safeIndex(index).eachRealaseResourses();
}
////intput lookuptable is raw imported table
//void TextureSlicePainter::SetColorMap(int index, vtkUnsignedCharArrayPtr table)
//{
//	mInternals->safeIndex(index).SetColorMap(table);
//}

void TextureSlicePainter::PrintSelf(ostream& os, vtkIndent indent)
{

}
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------


