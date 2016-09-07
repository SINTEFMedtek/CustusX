/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#ifndef CXTEXTURE3DSLICERREP_H_
#define CXTEXTURE3DSLICERREP_H_

#include "cxResourceVisualizationExport.h"

#include "cxRepImpl.h"
#include <vector>
#include "cxTransform3D.h"
#include "cxBoundingBox3D.h"

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"

//---------------------------------------------------------
namespace cx
{

typedef boost::shared_ptr<class Texture3DSlicerProxy> Texture3DSlicerProxyPtr;

/**
 * \brief Display overlayed image slices in 2D.
 *
 * The functionality is similar to SlicerRepSW, but the actual slicing
 * is performed by loading the image into the GPU as a 3D texture and
 * slicing it there, using the GPU.
 *
 * Several images are sliced simultaneously, and are overlayed using the
 * alpha transfer function (it is assumed to be a step function).
 *
 *
 *
 * The Texture3DSlicerRep draws several image slices in an overlay.
 * Opacity and LLR are used to draw several images on top of each other.
 * Despite its name is this a 2D Rep (it is drawn in a plane).
 *
 * The Texture3DSlicerRep class itself does very little real work:
 * It inherits from RepImpl and Rep, thus it can be added to a View.
 * The drawing functionality is embedded in Texture3DSlicerProxy, while
 * the slicer functionality is embedded in SliceProxy.
 *
 * SliceProxy determines the slice matrix and viewport from a Tool and
 * the slice definition (fex Axial, Sagittal, etc.). It uses a SliceComputer
 * to do the math. Texture3DSlicerProxy is a base class with default dummy
 * behaviour. This is solely a hack to get the code to compile on Windows.
 * The real behaviour lie in  Texture3DSlicerProxyImpl. It assembles images
 * and a slice definition, which is sent to the TextureSlicePainter to be
 * renderered. Most of the actual OpenGL code in embedded inside
 * TextureSlicePainter.
 *
 * \image html sscArchitecture_slice.png "Texture3DSlicerRep and surrounding classes."
 *
 *
 * Used by Sonowand and CustusX.
 *
 * \date Oct 13, 2009
 * \date Jan 8, 2012
 * \author petterw
 * \author christiana
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep2D
 */
class cxResourceVisualization_EXPORT Texture3DSlicerRep: public RepImpl
{
Q_OBJECT
public:
	static Texture3DSlicerRepPtr New(const QString& uid="");
	virtual ~Texture3DSlicerRep();
	virtual QString getType() const
	{
		return "Texture3DSlicerRep";
	}
	void setShaderPath(QString path);
	virtual void printSelf(std::ostream & os, Indent indent);
	void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp); // DEPRECATED: use zoomfactor in View and the object will auto-update
	void setImages(std::vector<ImagePtr> images);
	std::vector<ImagePtr> getImages();
	void setSliceProxy(SliceProxyPtr slicer);
//	void update();
	void setTargetSpaceToR();
	static bool isSupported(vtkRenderWindowPtr window);

protected:
	Texture3DSlicerRep();
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

private slots:
	void viewChanged();
private:
	Texture3DSlicerProxyPtr mProxy;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* CXTEXTURE3DSLICERREP_H_ */

