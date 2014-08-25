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


#ifndef CXNAVIGATEDVIDEOIMAGE_H_
#define CXNAVIGATEDVIDEOIMAGE_H_

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
class NavigatedVideoImage: public Image
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
