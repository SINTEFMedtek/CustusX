// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef SSCIMAGE2DREP3D_H
#define SSCIMAGE2DREP3D_H

#include "sscRepImpl.h"
#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class ApplyLUTToImage2DProxy> ApplyLUTToImage2DProxyPtr;
}

namespace cx
{

typedef boost::shared_ptr<class Image2DProxy> Image2DProxyPtr;

/**
 * \brief Slice volumes using a SliceProxy.
 *
 * The functionality is equal to SlicedImageProxy, but the actual slicing
 * is performed by loading the image into the GPU as a 3D texture and
 * slicing it there, using the GPU.
 *
 * Used by Sonowand and Sintef.
 *
 *  Created on: Oct 13, 2011
 *      Author: christiana
 *
 * \ingroup cx_resource_visualization
 */
class Image2DProxy: public QObject
{
Q_OBJECT
public:
	static Image2DProxyPtr New();
	virtual ~Image2DProxy();
//	void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp); // DEPRECATED: use zoomfactor in View and the object will auto-update
	void setImage(ImagePtr image);
//	void setSliceProxy(SliceProxyPtr slicer);
//	SliceProxyPtr getSliceProxy() { return mSliceProxy; }
//	void update();
	void setTargetSpaceToR(); ///< use to draw the slice in 3D r space instead of in 2D s space.
	vtkImageActorPtr getActor();

protected:
	Image2DProxy();

private slots:
	void transformChangedSlot();
	void transferFunctionsChangedSlot();
	void vtkImageDataChangedSlot();
private:
	ApplyLUTToImage2DProxyPtr mImageWithLUTProxy;
	ImagePtr mImage;
	vtkImageActorPtr mActor;
};




typedef boost::shared_ptr<class Image2DRep3D> Image2DRep3DPtr;

/** \brief Display a 2D Volume in 3D
 *
 * The standard VolumetricRep does not handle 2D volumes,
 * because the vtkVolume does not handle it.
 * This rep handles it though the use of a vtkImageActor.
 *
 * \ingroup cx_service_visualization
 * \date March 12, 2013
 * \author christiana
 */
class Image2DRep3D: public RepImpl
{
Q_OBJECT
public:
	static Image2DRep3DPtr New(QString uid="") { return wrap_new(new Image2DRep3D(), uid); }
	virtual ~Image2DRep3D();
	virtual QString getType() const { return "cx::Image2DRep3D"; }

	void setImage(ImagePtr image);

protected:
	Image2DRep3D();
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);

private:
	View *mView;
	Image2DProxyPtr mProxy;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------

#endif // SSCIMAGE2DREP3D_H
