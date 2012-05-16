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
 * vmTextureVolumePainter.cpp
 *
 *  Created on: Oct 13, 2009
 *      Author: petterw
 */

#include "sscTextureVolumePainter.h"

#ifndef WIN32
#include <QtCore>
#include <vtkOpenGLExtensionManager.h>
#include <vtkRenderer.h>
#include <vtkUniformVariables.h>
#include <vtkObjectFactory.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLTexture.h>
#include <vtkgl.h>
#include <vtkProperty.h>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#include "/usr/include/X11/Xlib.h"
#else
#include <GL/glu.h>
#endif

#include <Cg/cg.h>
#include <Cg/cgGL.h>


#ifdef WIN32
#include <windows.h>
#include <GL/glext.h>
#endif

#include "sscGPUImageBuffer.h"
#include "sscTypeConversions.h"
#include "sscGLHelpers.h"


//---------------------------------------------------------
namespace ssc
{
//---------------------------------------------------------

vtkStandardNewMacro(TextureVolumePainter);
vtkCxxRevisionMacro(TextureVolumePainter, "$Revision: 647 $");

class HackGLTexture: public vtkOpenGLTexture
{
public:
	HackGLTexture() {}
	virtual ~HackGLTexture() {}
	virtual long GetIndex() {return mIndex;}
	virtual void SetIndex(long index) {mIndex = index;}
private:
	long mIndex;
};
	
class SingleVolumePainterHelper
{
	ssc::GPUImageDataBufferPtr mVolumeBuffer;
	ssc::GPUImageLutBufferPtr mLutBuffer;
	int mIndex;
	float mWindow;
	float mLevel;
	float mLLR;
	float mAlpha;
	HackGLTexture *mTexture;
	bool mTextureLoaded;

public:
	explicit SingleVolumePainterHelper(int index)
	{
		mIndex = index;
		mTexture = new HackGLTexture();
		int texture = 2*mIndex;
		mTexture->SetIndex(texture);
		mTextureLoaded = false;
	}
	SingleVolumePainterHelper()
	{
		mIndex = -1;
		mTexture = NULL;
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
		if (mVolumeBuffer)
		{
			mVolumeBuffer->updateTexture();
		}
	}
	void eachRenderInternal(vtkSmartPointer<vtkProperty> prop)
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
		prop->AddShaderVariable("lutSize", lutSize);
		prop->AddShaderVariable("lut", lut);
		prop->AddShaderVariable("volumeTexture", texture);
		prop->AddShaderVariable("window", mWindow);
		prop->AddShaderVariable("level", mLevel);
		prop->AddShaderVariable("threshold", mLLR);
		prop->AddShaderVariable("transparency", mAlpha);
		report_gl_error();
	}
};

class TextureVolumePainter::vtkInternals
{
public:
	Display* mCurrentContext;

	vtkWeakPointer<vtkRenderWindow> LastContext;
	bool mInitialized;


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
		mInitialized = false;
	}
	~vtkInternals()
	{
		mElement.clear();
	}
	void ClearGraphicsResources()
	{
		mInitialized = false;
	}
};

//---------------------------------------------------------
TextureVolumePainter::TextureVolumePainter()
{
	mInternals = new vtkInternals();
}

void TextureVolumePainter::setShaderFile(QString shaderFile)
{
	mShaderFile = shaderFile;
}

TextureVolumePainter::~TextureVolumePainter()
{
	if (mInternals->LastContext)
	{
	    this->ReleaseGraphicsResources(this->LastWindow);
	}

	delete mInternals;
	mInternals = 0;
}

void TextureVolumePainter::ReleaseGraphicsResources(vtkWindow* win)
{
	mInternals->ClearGraphicsResources(); //the shader
	mInternals->LastContext = 0;
	this->Superclass::ReleaseGraphicsResources(win);
}

void TextureVolumePainter::PrepareForRendering(vtkRenderer* renderer, vtkActor* actor)
{
	if (!CanRender(renderer, actor))
	{
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
	if (!LoadRequiredExtensions(context->GetExtensionManager()))
	{
		std::cout << "Missing required EXTENSION!!!!!!!!!!!." << endl;
		return;
	}

	if (!mInternals->mInitialized)
	{
		actor->GetProperty()->LoadMaterial(mShaderFile.toUtf8().constData());
		actor->GetProperty()->ShadingOn();
		for (unsigned i = 0; i < mInternals->mElement.size(); ++i)
		{
			mInternals->mElement[i].initializeRendering();
		}
		mInternals->mInitialized = true;
	}
	float color[4] = {0,1,0,1};
	actor->GetProperty()->AddShaderVariable("renderMode", 6);
	actor->GetProperty()->AddShaderVariable("stepsize", 1.0/50.0);
	
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

void TextureVolumePainter::RenderInternal(vtkRenderer* renderer, vtkActor* actor, unsigned long typeflags,
		bool forceCompileOnly)
{
	report_gl_error();

	if (!CanRender(renderer, actor))
	{
		return;
	}

	for (unsigned i = 0; i < mInternals->mElement.size(); ++i)
	{
		mInternals->mElement[i].eachRenderInternal(actor->GetProperty());
	}

	report_gl_error();

	this->Superclass::RenderInternal(renderer, actor, typeflags, forceCompileOnly);

	glDisable(vtkgl::TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);
	report_gl_error();
}

bool TextureVolumePainter::CanRender(vtkRenderer*, vtkActor*)
{
	return !mInternals->mElement.empty();
}

bool TextureVolumePainter::LoadRequiredExtension(vtkOpenGLExtensionManager* mgr, QString id)
{
	bool loaded = mgr->LoadSupportedExtension(cstring_cast(id));
	if (!loaded)
		std::cout << "TextureVolumePainter Error: GL extension " + id + " not found" << std::endl;
	return loaded;
}

bool TextureVolumePainter::LoadRequiredExtensions(vtkOpenGLExtensionManager* mgr)
{
	GLint value[2];
	glGetIntegerv(vtkgl::MAX_TEXTURE_COORDS,value);
	if( value[0] < 8)
	{
		std::cout<<"GL_MAX_TEXTURE_COORDS="<<value[0]<<" . Number of texture coordinate sets. Min is 2."<<std::endl;
	}
	return (LoadRequiredExtension(mgr, "GL_VERSION_2_0")
			&& LoadRequiredExtension(mgr, "GL_VERSION_1_5")
			&& LoadRequiredExtension(mgr, "GL_VERSION_1_3")
			&& LoadRequiredExtension(mgr, "GL_ARB_vertex_buffer_object")
			&& LoadRequiredExtension(mgr, "GL_EXT_texture_buffer_object"));
}

void TextureVolumePainter::SetVolumeBuffer(int index, ssc::GPUImageDataBufferPtr buffer)
{
	mInternals->safeIndex(index).SetBuffer(buffer);
}

void TextureVolumePainter::SetLutBuffer(int index, ssc::GPUImageLutBufferPtr buffer)
{
	mInternals->safeIndex(index).SetBuffer(buffer);
}

void TextureVolumePainter::SetColorAttribute(int index, float window, float level, float llr,float alpha)
{
	mInternals->safeIndex(index).SetColorAttribute(window, level, llr, alpha);
}

void TextureVolumePainter::releaseGraphicsResources(int index)
{
}

void TextureVolumePainter::PrintSelf(ostream& os, vtkIndent indent)
{
}

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif //WIN32
