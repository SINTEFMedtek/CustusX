// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2012- SINTEF Medical Technology
// Copyright (C) 2012- Sonowand AS
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

#include "sscGPURayCastVolumePainter.h"

#ifndef WIN32
#include <QtCore>
#include <vtkOpenGLExtensionManager.h>
#include <vtkRenderer.h>
#include <vtkUniformVariables.h>
#include <vtkObjectFactory.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLTexture.h>
#include <vtkgl.h>
#include <vtkPlane.h>
#include <vtkProperty.h>
#include <vtkMatrix4x4.h>
#include <vtkShaderProgram2.h>
#include <vtkShader2.h>
#include <vtkShader2Collection.h>
#include <vtkImageData.h>

#ifdef WIN32
#include <windows.h>
#include <GL/glext.h>
#endif

#include "sscGPUImageBuffer.h"
#include "sscTypeConversions.h"
#include "sscGLHelpers.h"
#include "sscSlicePlaneClipper.h"


//---------------------------------------------------------
namespace ssc
{
//---------------------------------------------------------

const unsigned GPURayCastVolumePainter::maxVolumes = 4;

vtkStandardNewMacro(GPURayCastVolumePainter);
vtkCxxRevisionMacro(GPURayCastVolumePainter, "$Revision: 647 $");

class GPURayCastSingleVolumePainterHelper
{
	ssc::GPUImageDataBufferPtr mVolumeBuffer;
	ssc::GPUImageLutBufferPtr mLutBuffer;
	int mIndex;
	float mWindow;
	float mLevel;
	float mLLR;
	float mAlpha;
	Transform3D m_nMr;
	bool mClip;
	float mMaxValue;

public:
	explicit GPURayCastSingleVolumePainterHelper(int index) :
		mIndex(index),
		mWindow(0.0),
		mLevel(0.0),
		mLLR(0.0),
		mAlpha(1.0),
		mClip(false),
		mMaxValue(-1000)
	{
	}
	GPURayCastSingleVolumePainterHelper() :
		mIndex(-1),
		mWindow(0.0),
		mLevel(0.0),
		mLLR(0.0),
		mAlpha(1.0),
		mClip(false),
		mMaxValue(-1000)
	{
	}
	~GPURayCastSingleVolumePainterHelper()
	{
	}
	void SetBuffer(ssc::GPUImageDataBufferPtr buffer, double maxValue)
	{
		mVolumeBuffer = buffer;
		mMaxValue = maxValue;
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
	void set_nMr( Transform3D nMr)
	{
		m_nMr = nMr;
	}
	void setClip( bool clip)
	{
		mClip = clip;
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
		shader->GetUniformVariables()->SetUniformi(cstring_cast(QString("lutSize[%1]").arg(mIndex)), 1, &lutSize);
		shader->GetUniformVariables()->SetUniformi(cstring_cast(QString("lut[%1]").arg(mIndex)), 1, &lut);
		shader->GetUniformVariables()->SetUniformi(cstring_cast(QString("volumeTexture[%1]").arg(mIndex)), 1, &texture);
		shader->GetUniformVariables()->SetUniformf(cstring_cast(QString("window[%1]").arg(mIndex)), 1, &mWindow);
		shader->GetUniformVariables()->SetUniformf(cstring_cast(QString("level[%1]").arg(mIndex)), 1, &mLevel);
		shader->GetUniformVariables()->SetUniformf(cstring_cast(QString("threshold[%1]").arg(mIndex)), 1, &mLLR);
		shader->GetUniformVariables()->SetUniformf(cstring_cast(QString("alpha[%1]").arg(mIndex)), 1, &mAlpha);
		shader->GetUniformVariables()->SetUniformf(cstring_cast(QString("maxValue[%1]").arg(mIndex)), 1, &mMaxValue);
		int clip = mClip;
		shader->GetUniformVariables()->SetUniformi(cstring_cast(QString("useCutPlane[%1]").arg(mIndex)), 1, &clip);
		vtkMatrix4x4Ptr M = m_nMr.getVtkMatrix();
		float matrix[16];
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				matrix[i*4+j] = M->GetElement(j, i);
			}
		}
		shader->GetUniformVariables()->SetUniformMatrix(cstring_cast(QString("M[%1]").arg(mIndex)), 4, 4, matrix);
		report_gl_error();
	}
};

class GPURayCastVolumePainter::vtkInternals
{
public:

	vtkWeakPointer<vtkRenderWindow> LastContext;

	vtkSmartPointer<vtkShaderProgram2> Shader;

	unsigned int mVolumes;

	std::vector<GPURayCastSingleVolumePainterHelper> mElement;

	GPURayCastSingleVolumePainterHelper& safeIndex(int index)
	{
		if ((int)mElement.size() <= index)
		{
			mElement.resize(index+1);
			mElement[index] = GPURayCastSingleVolumePainterHelper(index);
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
GPURayCastVolumePainter::GPURayCastVolumePainter() :
	mDepthBuffer(0),
	mBackgroundBuffer(0),
	mStepSize(1.0),
	mRenderMode(0),
	mFBO(0),
	mDSColorBuffer(0),
	mDSDepthBuffer(0),
	mDownsampleWidth(512),
	mDownsampleHeight(512),
	mDownsamplePixels(0),
	mShouldResample(false),
	mResample(false),
	hasLoadedExtensions(false)
{
	mInternals = new vtkInternals();
}

void GPURayCastVolumePainter::setShaderFolder(QString folder)
{
	if (!folder.endsWith("/"))
	{
		folder +=  "/";
	}
	mVertexShaderFile = folder + "raycasting_shader.vert";
	mFragmentShaderFile = folder + "raycasting_shader.frag";
	mUSVertexShaderFile = folder + "upscale.vert";
	mUSFragmentShaderFile = folder + "upscale.frag";
}

QString GPURayCastVolumePainter::loadShaderFile(QString shaderFile)
{
	QFile fp(shaderFile);
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

GPURayCastVolumePainter::~GPURayCastVolumePainter()
{
	if (mInternals->LastContext)
	{
	    this->ReleaseGraphicsResources(this->LastWindow);
	}

	delete mInternals;
	mInternals = 0;
}

void GPURayCastVolumePainter::ReleaseGraphicsResources(vtkWindow* win)
{
	mInternals->ClearGraphicsResources(); //the shader
	mInternals->LastContext = 0;
	if (mDepthBuffer)
	{
		glDeleteTextures(1, &mDepthBuffer);
		mDepthBuffer = 0;
	}
	if (mBackgroundBuffer)
	{
		glDeleteTextures(1, &mBackgroundBuffer);
		mBackgroundBuffer = 0;
	}
	if (mUpscaleShader)
	{
		mUpscaleShader->ReleaseGraphicsResources();
		mUpscaleShader = 0;
	}
	freeDSBuffers();
	this->Superclass::ReleaseGraphicsResources(win);
}

void GPURayCastVolumePainter::PrepareForRendering(vtkRenderer* renderer, vtkActor* actor)
{
	if (!CanRender(renderer, actor))
	{
		mInternals->ClearGraphicsResources();
		mInternals->LastContext = 0;
		this->Superclass::PrepareForRendering(renderer, actor);
		return;
	}
	GL_TRACE("Prepare for 3D rendering");
	report_gl_error();

	GLint oldTextureUnit;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &oldTextureUnit);

	vtkRenderWindow* renWin = renderer->GetRenderWindow();
	if (mInternals->LastContext != renWin || mInternals->mElement.size() != mInternals->mVolumes)
	{
		mInternals->ClearGraphicsResources();
		hasLoadedExtensions = false; // force a re-check
	}
	mInternals->LastContext = renWin;

	report_gl_error();

	vtkOpenGLRenderWindow *context = vtkOpenGLRenderWindow::SafeDownCast(renWin); 
	if (!hasLoadedExtensions)
	{
		GL_TRACE("Loading 3D extensions");
		if (!LoadRequiredExtensions(context->GetExtensionManager()))
		{
			std::cout << "Missing required EXTENSION!" << endl;
			return;
		}
		hasLoadedExtensions = true;
	}

	if (!mInternals->Shader)
	{
		GL_TRACE("Loading 3D shaders");
		mInternals->mVolumes = mInternals->mElement.size();
		QString vertexShaderSource = this->loadShaderFile(mVertexShaderFile);
		QString fragmentShaderSource = this->loadShaderFile(mFragmentShaderFile);
		fragmentShaderSource = fragmentShaderSource.replace("${NUMBER_OF_VOLUMES}", QString("%1").arg(mInternals->mElement.size()));
		fragmentShaderSource = fragmentShaderSource.replace("${MAX_VOLUMES}", QString("%1").arg(maxVolumes));
		fragmentShaderSource = fragmentShaderSource.replace("${STEPSIZE}", QString("%1").arg(mStepSize));

		vtkShaderProgram2Ptr pgm = vtkShaderProgram2Ptr::New();
		pgm->SetContext(static_cast<vtkOpenGLRenderWindow *> (renWin));

		vtkShader2Ptr s1 = vtkShader2Ptr::New();
		s1->SetType(VTK_SHADER_TYPE_VERTEX);
		s1->SetSourceCode(cstring_cast(vertexShaderSource));
		s1->SetContext(pgm->GetContext());
		pgm->GetShaders()->AddItem(s1);

		vtkShader2Ptr s2 = vtkShader2Ptr::New();
		s2->SetType(VTK_SHADER_TYPE_FRAGMENT);
		s2->SetSourceCode(cstring_cast(fragmentShaderSource));
		s2->SetContext(pgm->GetContext());
		pgm->GetShaders()->AddItem(s2);
		mInternals->Shader = pgm;
		report_gl_error();
		for (unsigned i = 0; i < mInternals->mElement.size(); ++i)
		{
			mInternals->mElement[i].initializeRendering();
		}
		mInternals->Shader->Build();
		if (mInternals->Shader->GetLastBuildStatus() != VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
		{
			vtkErrorMacro("Pass Two failed.");
			abort();
		}
		if (!mInternals->Shader->IsValid())
		{
			vtkErrorMacro(<<" validation of the program failed: "<< mInternals->Shader->GetLastValidateLog());
		}
	}

	QSize size(renderer->GetSize()[0], renderer->GetSize()[1]);
	if (mLastRenderSize != size)
	{
		mResample = (mShouldResample && (size.width() * size.height() > mDownsamplePixels));
		freeDSBuffers();
	}

	if (mResample && !mUpscaleShader)
	{
		mInternals->mVolumes = mInternals->mElement.size();
		QString vertexShaderSource = this->loadShaderFile(mUSVertexShaderFile);
		QString fragmentShaderSource = this->loadShaderFile(mUSFragmentShaderFile);

		vtkShaderProgram2Ptr pgm = vtkShaderProgram2Ptr::New();
		pgm->SetContext(static_cast<vtkOpenGLRenderWindow *> (renWin));

		vtkShader2Ptr s1 = vtkShader2Ptr::New();
		s1->SetType(VTK_SHADER_TYPE_VERTEX);
		s1->SetSourceCode(cstring_cast(vertexShaderSource));
		s1->SetContext(pgm->GetContext());
		pgm->GetShaders()->AddItem(s1);

		vtkShader2Ptr s2 = vtkShader2Ptr::New();
		s2->SetType(VTK_SHADER_TYPE_FRAGMENT);
		s2->SetSourceCode(cstring_cast(fragmentShaderSource));
		s2->SetContext(pgm->GetContext());
		pgm->GetShaders()->AddItem(s2);
		mUpscaleShader = pgm;
		mUpscaleShader->Build();
		if (mUpscaleShader->GetLastBuildStatus() != VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
		{
			vtkErrorMacro("Build of resample shader failed.");
			abort();
		}
		if (!mUpscaleShader->IsValid())
		{
			vtkErrorMacro(<<" validation of the program failed: "<< mUpscaleShader->GetLastValidateLog());
		}
		report_gl_error();
	}

	mInternals->Shader->GetUniformVariables()->SetUniformf("stepsize", 1, &mStepSize);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	if (mResample && !vtkgl::IsFramebuffer(mFBO))
	{
		float factor = (float)mDownsamplePixels/(size.width() * size.height());
		factor = sqrt(factor);
		mDownsampleWidth = size.width() * factor;
		mDownsampleHeight = size.height() * factor;
		createDSBuffers();
	}

	QPoint origin(renderer->GetOrigin()[0], renderer->GetOrigin()[1]);
	float viewport[4];
	viewport[0] = origin.x();
	viewport[1] = origin.y();
	viewport[2] = size.width();
	viewport[3] = size.height();

	if (mResample)
	{
		viewport[0] = 0;
		viewport[1] = 0;
		viewport[2] = mDownsampleWidth;
		viewport[3] = mDownsampleHeight;
	}
	mInternals->Shader->GetUniformVariables()->SetUniformf("viewport", 4, viewport);

	report_gl_error();
	for (unsigned i = 0; i < mInternals->mElement.size(); ++i)
	{
		mInternals->mElement[i].eachPrepareRendering();
	}
	mInternals->Shader->Use();
	this->Superclass::PrepareForRendering(renderer, actor);

	mInternals->Shader->Restore();
	glBindTexture(GL_TEXTURE_3D, 0);
	glDisable(vtkgl::TEXTURE_3D);

	glActiveTexture(oldTextureUnit);
	report_gl_error();
	GL_TRACE("Prepare for 3D rendering complete");
}

void GPURayCastVolumePainter::createDSBuffers()
{
	vtkgl::GenFramebuffers(1, &mFBO);

	glGenTextures(1, &mDSColorBuffer);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, mDSColorBuffer);
	report_gl_error();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mDownsampleWidth, mDownsampleHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	report_gl_error();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenTextures(1, &mDSDepthBuffer);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, mDSDepthBuffer);
	report_gl_error();
	glTexImage2D(GL_TEXTURE_2D, 0, vtkgl::DEPTH_COMPONENT32, mDownsampleWidth, mDownsampleHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	report_gl_error();
	glTexParameteri(GL_TEXTURE_2D, vtkgl::TEXTURE_COMPARE_MODE, GL_NONE);
	glTexParameteri(GL_TEXTURE_2D, vtkgl::DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	vtkgl::BindFramebuffer(vtkgl::DRAW_FRAMEBUFFER, mFBO);
	vtkgl::FramebufferTexture2D(vtkgl::DRAW_FRAMEBUFFER, vtkgl::COLOR_ATTACHMENT0, GL_TEXTURE_2D, mDSColorBuffer, 0);
	vtkgl::FramebufferTexture2D(vtkgl::DRAW_FRAMEBUFFER, vtkgl::DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDSDepthBuffer, 0);

	vtkgl::BindFramebuffer(vtkgl::DRAW_FRAMEBUFFER, 0);
}

void GPURayCastVolumePainter::freeDSBuffers()
{
	if (mDSDepthBuffer)
	{
		glDeleteTextures(1, &mDSDepthBuffer);
		mDSDepthBuffer = 0;
	}
	if (mDSColorBuffer)
	{
		glDeleteTextures(1, &mDSColorBuffer);
		mDSColorBuffer = 0;
	}
	if (mFBO)
	{
		vtkgl::DeleteFramebuffers(1, &mFBO);
		mFBO = 0;
	}
}

void GPURayCastVolumePainter::RenderInternal(vtkRenderer* renderer, vtkActor* actor, unsigned long typeflags,
                                             bool forceCompileOnly)
{
	if (!CanRender(renderer, actor))
	{
		return;
	}
	GL_TRACE("3D rendering");

	GLint oldTextureUnit;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &oldTextureUnit);

	for (unsigned i = 0; i < mInternals->mElement.size(); ++i)
	{
		mInternals->mElement[i].eachRenderInternal(mInternals->Shader);
	}

	if (mClipper)
	{
		float vector[3];
		double dvector[3];
		mClipper->getClipPlaneCopy()->GetNormal(dvector);
		vector[0] = dvector[0];
		vector[1] = dvector[1];
		vector[2] = dvector[2];
		mInternals->Shader->GetUniformVariables()->SetUniformfv("cutPlaneNormal", 3, 1, vector);
		mClipper->getClipPlaneCopy()->GetOrigin(dvector);
		vector[0] = dvector[0];
		vector[1] = dvector[1];
		vector[2] = dvector[2];
		mInternals->Shader->GetUniformVariables()->SetUniformfv("cutPlaneOffset", 3, 1, vector);
	}

	glDepthMask(1);
	QSize size(renderer->GetSize()[0], renderer->GetSize()[1]);
	if (mLastRenderSize != size)
	{
		if (glIsTexture(mDepthBuffer))
		{
			glDeleteTextures(1, &mDepthBuffer);
			mDepthBuffer = 0;
		}
		if (glIsTexture(mBackgroundBuffer))
		{
			glDeleteTextures(1, &mBackgroundBuffer);
			mBackgroundBuffer = 0;
		}
	}
	if (!glIsTexture(mDepthBuffer))
	{
		glGenTextures(1, &mDepthBuffer);
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, mDepthBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, vtkgl::DEPTH_COMPONENT32, size.width(), size.height(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, vtkgl::TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(GL_TEXTURE_2D, vtkgl::DEPTH_TEXTURE_MODE, GL_LUMINANCE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		report_gl_error();
	}
	if (!glIsTexture(mBackgroundBuffer))
	{
		glGenTextures(1, &mBackgroundBuffer);
		glActiveTexture(GL_TEXTURE11);
		glBindTexture(GL_TEXTURE_2D, mBackgroundBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.width(), size.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		mLastRenderSize = size;
		report_gl_error();
	}

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, mDepthBuffer);
	QPoint origin(renderer->GetOrigin()[0], renderer->GetOrigin()[1]);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, origin.x(), origin.y(), size.width(), size.height());
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, mBackgroundBuffer);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, origin.x(), origin.y(), size.width(), size.height());
	int depthTexture = 10;
	mInternals->Shader->GetUniformVariables()->SetUniformi("depthBuffer", 1, (int*)&depthTexture);
	int backgroundTexture = 11;
	mInternals->Shader->GetUniformVariables()->SetUniformi("backgroundBuffer", 1, (int*)&backgroundTexture);
	mInternals->Shader->Use();
	report_gl_error();

	if (mResample)
	{
		vtkgl::BindFramebuffer( vtkgl::DRAW_FRAMEBUFFER, mFBO);
		GLenum status = vtkgl::CheckFramebufferStatus(vtkgl::DRAW_FRAMEBUFFER);
		switch (status)
		{
		case vtkgl::FRAMEBUFFER_COMPLETE:
			break;
		default:
			std::cout << "other framebuffer problem: " << status << std::endl;
			break;
		}
		report_gl_error();
		glViewport(0, 0, mDownsampleWidth, mDownsampleHeight);
		glDisable(GL_SCISSOR_TEST);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}
	this->Superclass::RenderInternal(renderer, actor, typeflags, forceCompileOnly);

	mInternals->Shader->Restore();

	if (mResample)
	{
		vtkgl::BindFramebuffer( vtkgl::DRAW_FRAMEBUFFER, 0);
		report_gl_error();
		GLenum buffer = GL_BACK;
		vtkgl::DrawBuffers(1, &buffer);
		vtkgl::BindFramebuffer( vtkgl::READ_FRAMEBUFFER, mFBO);
		report_gl_error();
		glReadBuffer(vtkgl::COLOR_ATTACHMENT0);

		GL_TRACE("Before upscale");
		glViewport(origin.x(), origin.y(), size.width(), size.height());
		glDisable(GL_SCISSOR_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mDSDepthBuffer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mDSColorBuffer);
		int var = 1;
		mUpscaleShader->GetUniformVariables()->SetUniformi("colors", 1, &var);
		var = 0;
		mUpscaleShader->GetUniformVariables()->SetUniformi("depth", 1, &var);
		mUpscaleShader->Use();
		glBegin(GL_QUADS);
		glVertex3f(-1,-1,0);
		glVertex3f(-1,1,0);
		glVertex3f(1,1,0);
		glVertex3f(1,-1,0);
		glEnd();
		report_gl_error();

		vtkgl::BindFramebuffer( vtkgl::READ_FRAMEBUFFER, 0);
		mUpscaleShader->Restore();
		report_gl_error();
	}
	glDisable(vtkgl::TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);
	glActiveTexture(oldTextureUnit);
	report_gl_error();
	GL_TRACE("3D rendering complete");
}

bool GPURayCastVolumePainter::CanRender(vtkRenderer*, vtkActor*)
{
	return !mInternals->mElement.empty();
}

bool GPURayCastVolumePainter::LoadRequiredExtension(vtkOpenGLExtensionManager* mgr, QString id)
{
	bool loaded = mgr->LoadSupportedExtension(cstring_cast(id));
	if (!loaded)
		std::cout << "GPURayCastVolumePainter Error: GL extension " + id + " not found" << std::endl;
	return loaded;
}

bool GPURayCastVolumePainter::LoadRequiredExtensions(vtkOpenGLExtensionManager* mgr)
{
	GLint value[2];
	glGetIntegerv(vtkgl::MAX_TEXTURE_COORDS,value);
	if( value[0] < 8)
	{
		std::cout<<"GL_MAX_TEXTURE_COORDS="<<value[0]<<" . Number of texture coordinate sets. Min is 2."<<std::endl;
	}
	mgr->LoadSupportedExtension("GL_GREMEDY_string_marker");
	return (LoadRequiredExtension(mgr, "GL_VERSION_2_0")
			&& LoadRequiredExtension(mgr, "GL_VERSION_1_5")
			&& LoadRequiredExtension(mgr, "GL_VERSION_1_3")
	        && LoadRequiredExtension(mgr, "GL_ARB_framebuffer_object")
	        && LoadRequiredExtension(mgr, "GL_ARB_texture_rectangle")
			&& LoadRequiredExtension(mgr, "GL_ARB_vertex_buffer_object")
			&& LoadRequiredExtension(mgr, "GL_EXT_texture_buffer_object"));
}

void GPURayCastVolumePainter::SetVolumeBuffer(int index, ssc::GPUImageDataBufferPtr buffer, double maxValue)
{
	mInternals->safeIndex(index).SetBuffer(buffer, maxValue);
}

void GPURayCastVolumePainter::SetLutBuffer(int index, ssc::GPUImageLutBufferPtr buffer)
{
	mInternals->safeIndex(index).SetBuffer(buffer);
}

void GPURayCastVolumePainter::SetColorAttribute(int index, float window, float level, float llr,float alpha)
{
	mInternals->safeIndex(index).SetColorAttribute(window, level, llr, alpha);
}

void GPURayCastVolumePainter::setClipVolume(int index, bool clip)
{
	mInternals->safeIndex(index).setClip(clip);
}

void GPURayCastVolumePainter::PrintSelf(ostream& os, vtkIndent indent)
{
}

void GPURayCastVolumePainter::set_nMr(int index, Transform3D nMr)
{
	mInternals->safeIndex(index).set_nMr(nMr);
}

void GPURayCastVolumePainter::setClipper(SlicePlaneClipperPtr clipper)
{
	mClipper = clipper;
}

void GPURayCastVolumePainter::setStepSize(double stepsize)
{
	mStepSize = stepsize;
}

void GPURayCastVolumePainter::enableImagePlaneDownsampling(int maxPixels)
{
	mShouldResample = true;
	mDownsamplePixels = maxPixels;
}

void GPURayCastVolumePainter::disableImagePlaneDownsampling()
{
	mShouldResample = false;
	mResample = false;
}

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif //WIN32
