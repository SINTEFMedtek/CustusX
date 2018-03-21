/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	static Texture3DSlicerRepPtr New(SharedOpenGLContextPtr context, const QString& uid="");

	virtual ~Texture3DSlicerRep();

	virtual QString getType() const;
	void setShaderPath(QString path);
	virtual void printSelf(std::ostream & os, Indent indent);
	void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp); // DEPRECATED: use zoomfactor in View and the object will auto-update
	void setImages(std::vector<ImagePtr> images);
	std::vector<ImagePtr> getImages();
	void setSliceProxy(SliceProxyPtr slicer);
	void setTargetSpaceToR();
	static bool isSupported(vtkRenderWindowPtr window);
	void setRenderWindow(vtkRenderWindowPtr window);

protected:
	Texture3DSlicerRep(SharedOpenGLContextPtr context);
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

