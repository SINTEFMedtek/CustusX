/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFisheyePass.h"

#include <algorithm>
#include <cmath>
#include <vtkObjectFactory.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderState.h>
#include <vtkRenderStepsPass.h>
#include <vtkDualDepthPeelingPass.h>
#include <vtkOrderIndependentTranslucentPass.h>
#include <vtkTranslucentPass.h>
#include <vtkOverlayPass.h>
#include <vtkOpenGLError.h>
#include <vtkOpenGLFramebufferObject.h>
#include <vtkOpenGLQuadHelper.h>
#include <vtkOpenGLRenderUtilities.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLShaderCache.h>
#include <vtkOpenGLState.h>
#include <vtkShaderProgram.h>
#include <vtkTextureObject.h>

namespace cx
{

vtkStandardNewMacro(FisheyePass);

FisheyePass::FisheyePass() :
	Strength(1.0),
	Supersampling(2),
	mFrameBufferObject(nullptr),
	mColorTexture(nullptr),
	mQuadHelper(nullptr)
{
	mSceneSteps = vtkSmartPointer<vtkRenderStepsPass>::New();
	// Overlays are rendered after the warp, so that they are not distorted
	mSceneSteps->SetOverlayPass(nullptr);
	mOverlayPass = vtkSmartPointer<vtkOverlayPass>::New();

	// Same translucent passes as vtkOpenGLRenderer uses when no render pass is set
	vtkSmartPointer<vtkTranslucentPass> translucent = vtkSmartPointer<vtkTranslucentPass>::New();
	mDepthPeelingPass = vtkSmartPointer<vtkDualDepthPeelingPass>::New();
	mDepthPeelingPass->SetTranslucentPass(translucent);
	mTranslucentPass = vtkSmartPointer<vtkOrderIndependentTranslucentPass>::New();
	mTranslucentPass->SetTranslucentPass(translucent);

	this->SetDelegatePass(mSceneSteps);
}

FisheyePass::~FisheyePass()
{
	delete mQuadHelper;
	if (mFrameBufferObject)
	{
		mFrameBufferObject->Delete();
	}
	if (mColorTexture)
	{
		mColorTexture->Delete();
	}
}

void FisheyePass::updateTranslucentPass(vtkRenderer* renderer)
{
	if (renderer->GetUseDepthPeeling())
	{
		mDepthPeelingPass->SetMaximumNumberOfPeels(renderer->GetMaximumNumberOfPeels());
		mDepthPeelingPass->SetOcclusionRatio(renderer->GetOcclusionRatio());
		mSceneSteps->SetTranslucentPass(mDepthPeelingPass);
	}
	else
	{
		mSceneSteps->SetTranslucentPass(mTranslucentPass);
	}
}

void FisheyePass::Render(const vtkRenderState* s)
{
	vtkOpenGLClearErrorMacro();

	this->NumberOfRenderedProps = 0;

	vtkRenderer* r = s->GetRenderer();
	vtkOpenGLRenderWindow* renWin = static_cast<vtkOpenGLRenderWindow*>(r->GetRenderWindow());
	vtkOpenGLState* ostate = renWin->GetState();

	vtkOpenGLState::ScopedglEnableDisable bsaver(ostate, GL_BLEND);
	vtkOpenGLState::ScopedglEnableDisable dsaver(ostate, GL_DEPTH_TEST);

	this->updateTranslucentPass(r);

	int x, y, w, h;
	r->GetTiledSizeAndOrigin(&w, &h, &x, &y);
	if (w > 0 && h > 0 && this->createWarpShader(renWin))
	{
		this->createRenderTarget(renWin);

		// Render the scene into the texture. Using the same size as both old and new size keeps the view angle.
		double scale = this->getSceneScale(renWin, w, h);
		int sceneWidth = static_cast<int>(w * scale);
		int sceneHeight = static_cast<int>(h * scale);
		ostate->PushFramebufferBindings();
		this->RenderDelegate(s, sceneWidth, sceneHeight, sceneWidth, sceneHeight, mFrameBufferObject, mColorTexture);
		ostate->PopFramebufferBindings();

		this->renderWarp(r, renWin, x, y, w, h);
	}
	else if (w > 0 && h > 0)
	{
		// Without the warp shader: render the scene undistorted
		mSceneSteps->Render(s);
		this->NumberOfRenderedProps += mSceneSteps->GetNumberOfRenderedProps();
	}

	if (w > 0 && h > 0)
	{
		ostate->vtkglEnable(GL_BLEND);
		mOverlayPass->Render(s);
		this->NumberOfRenderedProps += mOverlayPass->GetNumberOfRenderedProps();
	}

	vtkOpenGLCheckErrorMacro("failed after Render");
}

void FisheyePass::createRenderTarget(vtkOpenGLRenderWindow* renWin)
{
	if (!mColorTexture)
	{
		mColorTexture = vtkTextureObject::New();
		mColorTexture->SetContext(renWin);
		mColorTexture->SetMinificationFilter(vtkTextureObject::Linear);
		mColorTexture->SetMagnificationFilter(vtkTextureObject::Linear);
		mColorTexture->SetWrapS(vtkTextureObject::ClampToEdge);
		mColorTexture->SetWrapT(vtkTextureObject::ClampToEdge);
	}
	if (!mFrameBufferObject)
	{
		mFrameBufferObject = vtkOpenGLFramebufferObject::New();
		mFrameBufferObject->SetContext(renWin);
	}
}

/** Supersampling factor, reduced if the scene texture would exceed the maximum texture size. */
double FisheyePass::getSceneScale(vtkOpenGLRenderWindow* renWin, int w, int h) const
{
	double maxSize = vtkTextureObject::GetMaximumTextureSize(renWin);
	double scale = Supersampling;
	if (maxSize > 0)
	{
		scale = std::min(scale, maxSize / std::max(w, h));
	}
	return scale;
}

/** Create the warp shader the first time. Returns false if it could not be built. */
bool FisheyePass::createWarpShader(vtkOpenGLRenderWindow* renWin)
{
	if (!mQuadHelper && !mWarpShaderFailed)
	{
		std::string FSSource = vtkOpenGLRenderUtilities::GetFullScreenQuadFragmentShaderTemplate();
		vtkShaderProgram::Substitute(FSSource, "//VTK::FSQ::Decl",
			"uniform sampler2D source;\n"
			"uniform vec2 halfSize;\n" // half image size, in units of half the image height: (aspect, 1)
			"uniform float tanHalfViewAngle;\n" // of the rendered rectilinear image, vertically
			"uniform float strength;\n");
		// For an output point at radius r from the center, find the radius in the rectilinear image to sample from.
		// Equidistant fisheye: the view direction angle is proportional to r, and is equal to the
		// rectilinear image in the corners. The rectilinear image has radius tan(angle)/tanHalfViewAngle.
		vtkShaderProgram::Substitute(FSSource, "//VTK::FSQ::Impl",
			"  vec2 p = (2.0*texCoord - 1.0) * halfSize;\n"
			"  float r = length(p);\n"
			"  float rCorner = length(halfSize);\n"
			"  float angleCorner = atan(tanHalfViewAngle * rCorner);\n"
			"  float scale = angleCorner / (rCorner * tanHalfViewAngle);\n" // limit at r=0
			"  if (r > 1.0e-6)\n"
			"    scale = tan(angleCorner * r / rCorner) / (tanHalfViewAngle * r);\n"
			"  scale = mix(1.0, scale, strength);\n"
			"  vec2 q = p * scale;\n"
			"  gl_FragData[0] = texture2D(source, 0.5 + 0.5*q/halfSize);\n");

		mQuadHelper = new vtkOpenGLQuadHelper(renWin,
			vtkOpenGLRenderUtilities::GetFullScreenQuadVertexShader().c_str(), FSSource.c_str(), "");
		mWarpShaderFailed = !mQuadHelper->Program || !mQuadHelper->Program->GetCompiled();
		if (mWarpShaderFailed)
		{
			vtkErrorMacro("Couldn't build the fisheye shader program. Rendering without the fisheye lens.");
		}
	}
	return !mWarpShaderFailed;
}

/** Draw the rendered scene texture into the viewport, warped by the fisheye shader. */
void FisheyePass::renderWarp(vtkRenderer* renderer, vtkOpenGLRenderWindow* renWin, int x, int y, int w, int h)
{
	vtkOpenGLState* ostate = renWin->GetState();
	renWin->GetShaderCache()->ReadyShaderProgram(mQuadHelper->Program);
	double viewAngle = renderer->GetActiveCamera()->GetViewAngle() / 180.0 * M_PI;
	mColorTexture->Activate();
	mQuadHelper->Program->SetUniformi("source", mColorTexture->GetTextureUnit());
	float halfSize[2] = { static_cast<float>(w) / static_cast<float>(h), 1.0f };
	mQuadHelper->Program->SetUniform2f("halfSize", halfSize);
	mQuadHelper->Program->SetUniformf("tanHalfViewAngle", static_cast<float>(tan(viewAngle / 2.0)));
	mQuadHelper->Program->SetUniformf("strength", static_cast<float>(Strength));

	ostate->vtkglDisable(GL_BLEND);
	ostate->vtkglDisable(GL_DEPTH_TEST);
	ostate->vtkglClear(GL_DEPTH_BUFFER_BIT);
	ostate->vtkglViewport(x, y, w, h);
	ostate->vtkglScissor(x, y, w, h);

	mQuadHelper->Render();

	mColorTexture->Deactivate();
}

void FisheyePass::ReleaseGraphicsResources(vtkWindow* w)
{
	this->Superclass::ReleaseGraphicsResources(w);

	// Only one of the translucent passes is in use by the delegate at a time
	mDepthPeelingPass->ReleaseGraphicsResources(w);
	mTranslucentPass->ReleaseGraphicsResources(w);
	mOverlayPass->ReleaseGraphicsResources(w);

	delete mQuadHelper;
	mQuadHelper = nullptr;
	mWarpShaderFailed = false; // Try again in a new context
	if (mFrameBufferObject)
	{
		mFrameBufferObject->Delete();
		mFrameBufferObject = nullptr;
	}
	if (mColorTexture)
	{
		mColorTexture->Delete();
		mColorTexture = nullptr;
	}
}

} //namespace cx
