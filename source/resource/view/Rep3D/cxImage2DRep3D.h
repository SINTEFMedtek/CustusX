/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXIMAGE2DREP3D_H_
#define CXIMAGE2DREP3D_H_

#include "cxResourceVisualizationExport.h"

#include "cxRepImpl.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "cxTransform3D.h"

namespace cx
{
typedef boost::shared_ptr<class ApplyLUTToImage2DProxy> ApplyLUTToImage2DProxyPtr;
}

namespace cx
{

typedef boost::shared_ptr<class Image2DProxy> Image2DProxyPtr;

/**
 * \brief Display a 2D image in 3D space using a vtkImageSlice.
 *
 * Positions the image using the image's rMd transform plus an optional offset,
 * and applies the image's LUT via ApplyLUTToImage2DProxy.
 * Only accepts already-2D images (asserts image->is2D()).
 *
 * \ingroup cx_resource_view
 */
class cxResourceVisualization_EXPORT Image2DProxy : public QObject
{
Q_OBJECT
public:
	static Image2DProxyPtr New();
	virtual ~Image2DProxy();
	void setImage(ImagePtr image);
	void setTargetSpaceToR(); ///< use to draw the slice in 3D r space instead of in 2D s space.
	vtkImageSlicePtr getSlice();

	void setTransformOffset(Transform3D rMrr);
protected:
	Image2DProxy();

private slots:
	void transformChangedSlot();
	void transferFunctionsChangedSlot();
	void vtkImageDataChangedSlot();
private:
	ApplyLUTToImage2DProxyPtr mImageWithLUTProxy;
	ImagePtr mImage;
	vtkImageSlicePtr mSlice;
	Transform3D m_rMrr;
};




typedef boost::shared_ptr<class Image2DRep3D> Image2DRep3DPtr;

/** \brief Display a 2D Volume in 3D
 *
 * The standard VolumetricRep does not handle 2D volumes,
 * because the vtkVolume does not handle it.
 * This rep handles it though the use of a vtkImageSlice.
 *
 * \ingroup cx_resource_view
 * \date March 12, 2013
 * \author christiana
 */
class cxResourceVisualization_EXPORT Image2DRep3D: public RepImpl
{
Q_OBJECT
public:
	static Image2DRep3DPtr New(QString uid="") { return wrap_new(new Image2DRep3D(), uid); }
	virtual ~Image2DRep3D();
	virtual QString getType() const { return "cx::Image2DRep3D"; }

	void setImage(ImagePtr image);

protected:
	Image2DRep3D();
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

private:
	Image2DProxyPtr mProxy;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------

#endif // CXIMAGE2DREP3D_H_
