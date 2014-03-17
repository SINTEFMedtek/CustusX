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

#ifndef CXTEXTURE3DSLICERREP_H_
#define CXTEXTURE3DSLICERREP_H_

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
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep2D
 */
class Texture3DSlicerRep: public RepImpl
{
Q_OBJECT
public:
	static Texture3DSlicerRepPtr New(const QString& uid);
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
	void update();
	void setTargetSpaceToR();
	static bool isSupported(vtkRenderWindowPtr window);

protected:
	Texture3DSlicerRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);

private slots:
	void viewChanged();
private:
	View *mView;
	Texture3DSlicerProxyPtr mProxy;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* CXTEXTURE3DSLICERREP_H_ */

