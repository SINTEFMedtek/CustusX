/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxImageEnveloper.h"
#include "cxImage.h"
#include "cxVolumeHelpers.h"
#include "vtkImageData.h"
#include "cxRegistrationTransform.h"
#include "cxTypeConversions.h"
#include "cxBoundingBox3D.h"
#include "cxImageTF3D.h"


namespace cx
{
ImageEnveloperPtr ImageEnveloper::create()
{
	return ImageEnveloperPtr(new ImageEnveloper());
}

ImageEnveloper::ImageEnveloper() : mMaxEnvelopeVoxels(10*1000*1000)
{

}

void ImageEnveloper::setMaxEnvelopeVoxels(long maxVoxels)
{
	mMaxEnvelopeVoxels = maxVoxels;
}

void ImageEnveloper::setImages(std::vector<ImagePtr> images)
{
	mImages = images;
}

ImagePtr ImageEnveloper::getEnvelopingImage()
{
	ImageParameters box = this->createEnvelopeParametersFromImage(mImages[0]);
	for(unsigned i = 1; i < mImages.size(); ++i)
		box = selectParametersWithSmallestExtent(box, this->createEnvelopeParametersFromImage(mImages[i]));

	box.limitVoxelsKeepBounds(mMaxEnvelopeVoxels);

	ImagePtr retval = this->createEnvelopeFromParameters(box);

	return retval;
}

ImageParameters ImageEnveloper::createEnvelopeParametersFromImage(ImagePtr img)
{
	ImageParameters retval;

	DoubleBoundingBox3D bb = findEnclosingBoundingBox(mImages, img->get_rMd().inverse());

	retval.setSpacingKeepDim(this->getMinimumSpacingFromAllImages(img->get_rMd().inverse()));
	retval.setDimKeepBoundsAlignSpacing(bb.range().array());
	retval.mParentVolume = img->getUid();

	Vector3D shift = bb.bottomLeft();

	retval.m_rMd = img->get_rMd() * createTransformTranslate(shift);

	return retval;
}

ImageParameters ImageEnveloper::selectParametersWithSmallestExtent(ImageParameters a, ImageParameters b)
{
	if (a.getVolume() <= b.getVolume())
		return a;
	else
		return b;
}

ImageParameters ImageEnveloper::selectParametersWithFewestVoxels(ImageParameters a, ImageParameters b)
{
	if (a.getNumVoxels() <= b.getNumVoxels())
		return a;
	else
		return b;
}

Eigen::Array3d ImageEnveloper::getMinimumSpacingFromAllImages(Transform3D qMr)
{
	Eigen::Array3d retval;
	retval = this->getTransformedSpacing(mImages[0]->getSpacing(), qMr * mImages[0]->get_rMd());
	for (unsigned i = 1; i < mImages.size(); ++i)
	{
		Eigen::Array3d current = this->getTransformedSpacing(mImages[i]->getSpacing(), qMr * mImages[i]->get_rMd());
		retval = retval.min(current);
	}
	return retval;
}

Eigen::Array3d ImageEnveloper::getTransformedSpacing(Eigen::Array3d spacing, Transform3D qMd)
{
	Eigen::Array3d retval;

	//Create spacing vectors in img coord syst (d)
	Vector3D sx = Vector3D(spacing[0], 0, 0);
	Vector3D sy = Vector3D(0, spacing[1], 0);
	Vector3D sz = Vector3D(0, 0, spacing[2]);

	//Transform to q coord syst
	sx = qMd.vector(sx);
	sy = qMd.vector(sy);
	sz = qMd.vector(sz);

	//Find spacing for each axis
	for (unsigned i = 0; i < 3; ++i)
	{
		retval[i] = std::max(fabs(sx[i]), fabs(sy[i]));
		retval[i] = std::max(retval[i], fabs(sz[i]));
	}

	return retval;
}

ImagePtr ImageEnveloper::createEnvelopeFromParameters(ImageParameters box)
{
	int maxRange = this->getMaxScalarRange();

	vtkImageDataPtr imageData = generateVtkImageDataUnsignedShort(box.getDim(), box.getSpacing(), maxRange, 1);

	QString uid = QString("envelope_image_%1").arg(box.mParentVolume);
	ImagePtr retval(new Image(uid, imageData));
	retval->get_rMd_History()->setRegistration(box.m_rMd);
	retval->get_rMd_History()->setParentSpace(box.mParentVolume);
	retval->setAcquisitionTime(QDateTime::currentDateTime());
	retval->setModality(imSC);

	return retval;
}

int ImageEnveloper::getMaxScalarRange()
{
	int maxRange = 0;
	for (unsigned i=0; i<mImages.size(); ++i)
		maxRange = std::max<int>(maxRange, mImages[i]->getBaseVtkImageData()->GetScalarRange()[1]);
	return maxRange;
}

} // namespace cx
