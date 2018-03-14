/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXNAVIGATEDVIDEOIMAGE_H_
#define CXNAVIGATEDVIDEOIMAGE_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxImage.h"

namespace cx
{

/**
 * \brief Image subclass for live updated 2D images tied to a tool
 *
 * This class can be used to wrap a VideoSource in an Image
 * that can be used in 2D views to combine the video source with
 * slices of other images. This image is allways registered in a
 * particular way to a tool specified using a SliceProxy.
 *
 * \ingroup cx_resource_core_data
 *
 */
class cxResource_EXPORT NavigatedVideoImage: public Image
{
	Q_OBJECT;
public:
	NavigatedVideoImage(QString uid, VideoSourcePtr source, SliceProxyPtr sliceProxy, QString name="");
	virtual Transform3D get_rMd() const; ///< \return the transform M_rd from the data object's space (d) to the reference space (r).
	virtual double computeFullViewZoomFactor(DoubleBoundingBox3D viewport) const;
	virtual void setToolPosition(double, double);
private slots:
	void newFrame();
private:
	SliceProxyPtr mSliceProxy;
	double mSpacing[3];
	double mToolPositionX;
	double mToolPositionY;
};

} // end namespace cx

#endif /*CXNAVIGATEDVIDEOIMAGE_H_*/
