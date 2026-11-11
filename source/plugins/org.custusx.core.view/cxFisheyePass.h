/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXFISHEYEPASS_H
#define CXFISHEYEPASS_H

#include "org_custusx_core_view_Export.h"
#include <vtkImageProcessingPass.h>
#include <vtkSmartPointer.h>

class vtkOpenGLFramebufferObject;
class vtkOpenGLQuadHelper;
class vtkTextureObject;
class vtkRenderStepsPass;
class vtkDualDepthPeelingPass;
class vtkOrderIndependentTranslucentPass;
class vtkOverlayPass;
class vtkOpenGLRenderWindow;
class vtkOpenGLState;

namespace cx
{

/**
 * \file
 * \addtogroup org_custusx_core_view
 * @{
 */

/**
 * Render pass imitating the wide angle lens of an endoscope.
 *
 * The scene is rendered with the normal (rectilinear) perspective projection
 * into a texture, which is then warped towards an equidistant fisheye projection:
 * The center of the image is magnified and the periphery compressed, while the
 * image corners keep the same view direction. With a wide view angle, this avoids
 * the "long hallway" look of a rectilinear projection.
 *
 * Overlays (2D actors) are rendered on top of the warped image, without distortion.
 * Translucent geometry uses depth peeling if enabled on the renderer.
 */
class org_custusx_core_view_EXPORT FisheyePass : public vtkImageProcessingPass
{
public:
	static FisheyePass* New();
	vtkTypeMacro(FisheyePass, vtkImageProcessingPass);

	void Render(const vtkRenderState* s) override;
	void ReleaseGraphicsResources(vtkWindow* w) override;

	/**
	 * Amount of distortion: 0 gives the undistorted rectilinear projection,
	 * 1 gives an equidistant fisheye projection. Default is 1.
	 */
	vtkSetClampMacro(Strength, double, 0.0, 1.0);
	vtkGetMacro(Strength, double);

protected:
	FisheyePass();
	~FisheyePass() override;

private:
	FisheyePass(const FisheyePass&) = delete;
	void operator=(const FisheyePass&) = delete;

	void updateTranslucentPass(vtkRenderer* renderer);
	void createRenderTarget(vtkOpenGLRenderWindow* renWin);
	bool readyWarpShader(vtkOpenGLRenderWindow* renWin);
	void renderWarp(vtkRenderer* renderer, vtkOpenGLState* ostate, int x, int y, int w, int h);

	double Strength;
	int Supersampling; ///< render the scene at this multiple of the viewport size, as the center is magnified
	vtkSmartPointer<vtkRenderStepsPass> mSceneSteps;
	vtkSmartPointer<vtkDualDepthPeelingPass> mDepthPeelingPass;
	vtkSmartPointer<vtkOrderIndependentTranslucentPass> mTranslucentPass;
	vtkSmartPointer<vtkOverlayPass> mOverlayPass;
	vtkOpenGLFramebufferObject* mFrameBufferObject;
	vtkTextureObject* mColorTexture;
	vtkOpenGLQuadHelper* mQuadHelper;
};

/**
 * @}
 */
} //namespace cx

#endif // CXFISHEYEPASS_H
