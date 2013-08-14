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

#ifndef SSCNAVIGATEDVIDEOIMAGE_H_
#define SSCNAVIGATEDVIDEOIMAGE_H_

#include "sscImage.h"

namespace ssc
{

/**
 * \brief ssc::Image subclass for live updated 2D images tied to a tool
 *
 * This class can be used to wrap a ssc::VideoSource in an ssc::Image
 * that can be used in 2D views to combine the video source with
 * slices of other images. This image is allways registered in a
 * particular way to a tool specified using a ssc::SliceProxy.
 */
class NavigatedVideoImage: public ssc::Image
{
	Q_OBJECT;
public:
	NavigatedVideoImage(QString uid, ssc::VideoSourcePtr source, ssc::SliceProxyPtr sliceProxy, QString name="");
	virtual ssc::Transform3D get_rMd() const; ///< \return the transform M_rd from the data object's space (d) to the reference space (r).
	virtual double computeFullViewZoomFactor(DoubleBoundingBox3D viewport) const;
	virtual void setToolPosition(double, double);
private slots:
	void newFrame();
private:
	ssc::SliceProxyPtr mSliceProxy;
	double mSpacing[3];
	double mToolPositionX;
	double mToolPositionY;
};

} // end namespace ssc

#endif /*SSCNAVIGATEDVIDEOIMAGE_H_*/
