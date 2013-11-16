// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * vmTextureSlicePainter.cpp
 *
 *  Created on: Oct 13, 2009
 *      Author: petterw
 */

#include "sscTextureSlicePainter.h"

#ifndef WIN32
#include <QtCore>
#include <vtkOpenGLExtensionManager.h>
#include <vtkRenderer.h>
#include <vtkShaderProgram2.h>
#include <vtkShader2.h>
#include <vtkShader2Collection.h>
#include <vtkUniformVariables.h>
#include <vtkObjectFactory.h>
#include <vtkOpenGLRenderWindow.h>
#include "sscLogger.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#include "X11/Xlib.h"
//#include "/usr/include/X11/Xlib.h"
#else
#include <GL/glu.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <GL/glext.h>
#endif

#include "sscGPUImageBuffer.h"
#include "sscTypeConversions.h"
#include "sscGLHelpers.h"

//---------------------------------------------------------
namespace cx
{
//---------------------------------------------------------

vtkStandardNewMacro(TextureSlicePainter);
vtkCxxRevisionMacro(TextureSlicePainter, "$Revision: 647 $");

class SingleVolumePainterHelper
{
	GPUImageDataBufferPtr mVolumeBuffer;
	GPUImageLutBufferPtr mLutBuffer;
	int mIndex;
	float mWindow;
	float mLevel;
	float mLLR;
	float mAlpha;

public:
	explicit SingleVolumePainterHelper(int index) :
		mWindow(0.0),
		mLevel(0.0),
		mLLR(0.0),
		mAlpha(1.0)
	{
		mIndex = index;
	}
	SingleVolumePainterHelper() :
		mWindow(0.0),
		mLevel(0.0),
		mLLR(0.0),
		mAlpha(1.0)
	{
		mIndex = -1;
	}
	~SingleVolumePainterHelper()
	{

	}
	void SetBuffer(GPUImageDataBufferPtr buffer)
	{
		mVolumeBuffer = buffer;
	}
	void SetBuffer(GPUImageLutBufferPtr buffer)
	{
		mLutBuffer = buffer;
	}
	void SetColorAttribute(float window, float level, float llr,float alpha)
	{
		mWindow = window;
		mLevel = level;
		mLLR = llr;
		mAlpha = alpha;
	}
	void initializeRendering()
	{
		if (mVolumeBuffer)
			mVolumeBuffer->allocate();
		if (mLutBuffer)
			mLutBuffer->allocate();
	}
	void setUniformiArray(vtkUniformVariables* uniforms, QString name, int val)
	{
		QString fullName = QString("%1[%2]").arg(name).arg(mIndex);
		uniforms->SetUniformi(cstring_cast(fullName), 1, &val);
	}
	void setUniformfArray(vtkUniformVariables* uniforms, QString name, float val)
	{
		QString fullName = QString("%1[%2]").arg(name).arg(mIndex);
		uniforms->SetUniformf(cstring_cast(fullName), 1, &val);
	}

	void eachRenderInternal(vtkSmartPointer<vtkShaderProgram2> shader)
	{
		if (!mVolumeBuffer)
			return;

		mVolumeBuffer->bind(mIndex);

		int texture = 2*mIndex; //texture unit 1
		int lut = 2*mIndex+1; //texture unit 1

		if (mLutBuffer)
		{
			mLutBuffer->bind(mIndex);
		}

		vtkUniformVariables* uniforms = shader->GetUniformVariables();
		this->setUniformiArray(uniforms, "texture", texture);
		this->setUniformiArray(uniforms, "lut", lut);
		this->setUniformfArray(uniforms, "llr", mLLR);
		this->setUniformfArray(uniforms, "level", mLevel);
		this->setUniformfArray(uniforms, "window", mWindow);
		this->setUniformfArray(uniforms, "alpha", mAlpha);

		report_gl_error();
	}
};

class TextureSlicePainter::vtkInternals
{
public:
	Display* mCurrentContext;
	vtkWeakPointer<vtkRenderWindow> LastContext;
	vtkSmartPointer<vtkShaderProgram2> Shader;
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
TextureSlicePainter::TextureSlicePainter() :
	hasLoadedExtensions(false)
{
	mInternals = new vtkInternals();
}

void TextureSlicePainter::setShaderPath(QString path)
{
	mShaderPath = path;
}

QString TextureSlicePainter::loadShaderFile()
{	
	QString filepath = mShaderPath + "/cxOverlayLUT2D_frag.glsl";
	QFile fp(filepath);
	if (fp.exists())
	{
		fp.open(QFile::ReadOnly);
		QTextStream shaderfile(&fp);
		return shaderfile.readAll();
	}
	else
	{
		std::cout << "TextureSlicer can't read shaderfile [" << fp.fileName() << "]" << std::endl;
	}
	return "";
}

TextureSlicePainter::~TextureSlicePainter()
{
	if (mInternals->LastContext)
	{
	    this->ReleaseGraphicsResources(this->LastWindow);
	}

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
	report_gl_error();
	if (!CanRender(renderer, actor))
	{
		mInternals->ClearGraphicsResources();
		mInternals->LastContext = 0;
		this->Superclass::PrepareForRendering(renderer, actor);
		return;
	}
	GL_TRACE("Prepare for 2D rendering");
	GLint oldTextureUnit;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &oldTextureUnit);

	vtkRenderWindow* renWin = renderer->GetRenderWindow();
	if (mInternals->LastContext != renWin)
	{
		mInternals->ClearGraphicsResources();
		hasLoadedExtensions = false; // force re-check
	}
	mInternals->LastContext = renWin;

	vtkOpenGLRenderWindow *context = vtkOpenGLRenderWindow::SafeDownCast(renWin);
	if (!hasLoadedExtensions)
	{
		GL_TRACE("Loading 2D extensions");
		if (!LoadRequiredExtensions(context->GetExtensionManager()))
		{
			std::cout << "Missing required EXTENSION!!!!!!!!!!!." << endl;
			return;
		}
		hasLoadedExtensions = true;
	}

	for (unsigned i = 0; i < mInternals->mElement.size(); ++i)
	{
		mInternals->mElement[i].initializeRendering();
	}

	if (!mInternals->Shader)
	{
		report_gl_error();
		GL_TRACE("Loading 2D shaders");
		QString shaderSource = this->loadShaderFile();
		int layers = mInternals->mElement.size();
		shaderSource = shaderSource.replace("${LAYERS}", QString("%1").arg(layers));

		vtkShaderProgram2Ptr pgm = vtkShaderProgram2Ptr::New();
		pgm->SetContext(static_cast<vtkOpenGLRenderWindow *> (renWin));

		vtkShader2Ptr s2 = vtkShader2Ptr::New();
		s2->SetType(VTK_SHADER_TYPE_FRAGMENT);
		s2->SetSourceCode(cstring_cast(shaderSource));
		s2->SetContext(pgm->GetContext());
		pgm->GetShaders()->AddItem(s2);
		mInternals->Shader = pgm;
		report_gl_error();
//		for (unsigned i = 0; i < mInternals->mElement.size(); ++i)
//		{
//			mInternals->mElement[i].initializeRendering();
//		}
		mInternals->Shader->Build();
		if (mInternals->Shader->GetLastBuildStatus() != VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
		{
			vtkErrorMacro("Pass Two failed.");
			abort();
		}
	}

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

//	for (unsigned i = 0; i < mInternals->mElement.size(); ++i)
//	{
//		mInternals->mElement[i].eachPrepareRendering();
//	}

	this->Superclass::PrepareForRendering(renderer, actor);
	report_gl_error();
	glActiveTexture(oldTextureUnit);
	GL_TRACE("Prepare for 2D rendering complete");
}

void TextureSlicePainter::RenderInternal(vtkRenderer* renderer, vtkActor* actor, unsigned long typeflags,
		bool forceCompileOnly)
{
	if (!CanRender(renderer, actor))
	{
		return;
	}
	GL_TRACE("2D rendering");
	GLint oldTextureUnit;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &oldTextureUnit);

	int layers = mInternals->mElement.size();
	mInternals->Shader->GetUniformVariables()->SetUniformi("layers", 1, &layers);

	for (unsigned i = 0; i < mInternals->mElement.size(); ++i)
	{
		mInternals->mElement[i].eachRenderInternal(mInternals->Shader);
	}

	mInternals->Shader->Use();

	if (!mInternals->Shader->IsValid())
	{
		vtkErrorMacro(<<" validation of the program failed: "<< mInternals->Shader->GetLastValidateLog());
	}

	this->Superclass::RenderInternal(renderer, actor, typeflags, forceCompileOnly);

	mInternals->Shader->Restore();
	glBindTexture(GL_TEXTURE_3D, 0);
	glDisable(vtkgl::TEXTURE_3D);
	report_gl_error();
	glActiveTexture(oldTextureUnit);
	GL_TRACE("2D rendering complete");
}

bool TextureSlicePainter::CanRender(vtkRenderer*, vtkActor*)
{
	return !mInternals->mElement.empty();
}

bool TextureSlicePainter::LoadRequiredExtension(vtkOpenGLExtensionManager* mgr, QString id)
{
	bool loaded = mgr->LoadSupportedExtension(cstring_cast(id));
	if (!loaded)
		std::cout << "TextureSlicePainter Error: GL extension " + id + " not found" << std::endl;
	return loaded;
}

bool TextureSlicePainter::LoadRequiredExtensions(vtkOpenGLExtensionManager* mgr)
{
	return (LoadRequiredExtension(mgr, "GL_VERSION_2_0")
			&& LoadRequiredExtension(mgr, "GL_VERSION_1_5")
			&& LoadRequiredExtension(mgr, "GL_ARB_vertex_buffer_object"));
}

void TextureSlicePainter::SetVolumeBuffer(int index, GPUImageDataBufferPtr buffer)
{
	mInternals->safeIndex(index).SetBuffer(buffer);
}

void TextureSlicePainter::SetLutBuffer(int index, GPUImageLutBufferPtr buffer)
{
	mInternals->safeIndex(index).SetBuffer(buffer);
}

void TextureSlicePainter::SetColorAttribute(int index, float window, float level, float llr,float alpha)
{
	mInternals->safeIndex(index).SetColorAttribute(window, level, llr, alpha);
}

void TextureSlicePainter::releaseGraphicsResources(int index)
{
}

void TextureSlicePainter::PrintSelf(ostream& os, vtkIndent indent)
{
}

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif //WIN32
