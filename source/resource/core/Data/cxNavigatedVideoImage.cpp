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


#include "cxNavigatedVideoImage.h"
#include <vtkImageData.h>
#include "cxVideoSource.h"
#include "cxImageLUT2D.h"
#include "cxSliceProxy.h"
namespace cx
{
NavigatedVideoImage::NavigatedVideoImage(QString uid, VideoSourcePtr source, SliceProxyPtr sliceProxy, QString name):
	Image(uid, source->getVtkImageData(), name),
	mSliceProxy(sliceProxy)
{
	mToolPositionX = -1;
	mToolPositionY = -1;
	connect(source.get(), &VideoSource::newFrame, this, &NavigatedVideoImage::newFrame);
	getLookupTable2D()->setFullRangeWinLevel(source->getVtkImageData());
}

static double computeEffectiveOffset(double offset, double size)
{
	if (offset >= 0)
	{
		return offset;
	}
	return size/2;
}

/**
 * Overriden from Image to make sure this image is allways fixed
 * in the slice plane of the given slice proxy and positioned
 * according to the given tool position offset
 */
Transform3D NavigatedVideoImage::get_rMd() const
{
	int size[3];
	double spacing[3];
	mBaseImageData->GetDimensions(size);
	mBaseImageData->GetSpacing(spacing);
	double offsetX = computeEffectiveOffset( mToolPositionX, size[0] );
	double offsetY = computeEffectiveOffset( mToolPositionY, size[1] );
	Transform3D T = createTransformTranslate(Vector3D(-(size[0]-offsetX)*spacing[0], -offsetY*spacing[1], 0));
	Transform3D Flip = createTransformScale(Vector3D(1, -1, 1)); // Video data has inverted Y axis compared to other data
	return mSliceProxy->get_sMr().inv() * Flip * T;
}

/**
 * \brief Compute the largest zoom factor that will still fit the entire
 * video image in the given viewport
 */
double NavigatedVideoImage::computeFullViewZoomFactor(DoubleBoundingBox3D viewport) const
{
	int size[3];
	mBaseImageData->GetDimensions(size);
	double scalefactorX =(viewport[1]-viewport[0])/(double)size[0];
	double scalefactorY = (viewport[3]-viewport[2])/(double)size[1];
	return std::min(scalefactorX, scalefactorY);
}

void NavigatedVideoImage::newFrame()
{
	double spacing[3];
	mBaseImageData->GetSpacing(spacing);
	if (spacing[0] != mSpacing[0] || spacing[1] != mSpacing[1] ||
	    spacing[2] != mSpacing[2] )
	{
		emit transformChanged();
	}
	emit vtkImageDataChanged();
	mSpacing[0] = spacing[0];
	mSpacing[1] = spacing[1];
	mSpacing[2] = spacing[2];
}

/**
 * \brief Set the position of the tool point in the video image.
 *
 * The arguments are in pixels of the video image. A negative value
 * sets the position to be in the centre of the image along that axis
 */
void NavigatedVideoImage::setToolPosition(double positionX, double positionY)
{
	mToolPositionX = positionX;
	mToolPositionY = positionY;
	emit transformChanged();
}

};
