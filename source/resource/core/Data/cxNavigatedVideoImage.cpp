/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
