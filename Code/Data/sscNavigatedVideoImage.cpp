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

#include "sscNavigatedVideoImage.h"
#include <vtkImageData.h>
#include "sscVideoSource.h"
#include "sscImageLUT2D.h"
#include "sscSliceProxy.h"
namespace ssc
{
NavigatedVideoImage::NavigatedVideoImage(QString uid, ssc::VideoSourcePtr source, ssc::SliceProxyPtr sliceProxy, QString name):
	ssc::Image(uid, source->getVtkImageData(), name),
	mSliceProxy(sliceProxy)
{
	mToolPositionX = -1;
	mToolPositionY = -1;
	connect(source.get(), SIGNAL(newFrame()),
		this, SLOT(newFrame()));
	getLookupTable2D()->setFullRangeWinLevel();
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
 * Overriden from ssc::Image to make sure this image is allways fixed
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
