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
 * \ingroup cx_resource_view
 */
class cxResourceVisualization_EXPORT Image2DProxy : public QObject
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
	vtkImageActorPtr mActor;
	Transform3D m_rMrr;
};




typedef boost::shared_ptr<class Image2DRep3D> Image2DRep3DPtr;

/** \brief Display a 2D Volume in 3D
 *
 * The standard VolumetricRep does not handle 2D volumes,
 * because the vtkVolume does not handle it.
 * This rep handles it though the use of a vtkImageActor.
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
