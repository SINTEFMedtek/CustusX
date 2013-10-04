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


#include "cxImageEnveloper.h"
#include "sscImage.h"
#include "sscVolumeHelpers.h"
#include "vtkImageData.h"
#include "sscRegistrationTransform.h"
#include "sscTypeConversions.h"
#include "sscBoundingBox3D.h"


namespace cx
{

ImageEnveloperMocPtr ImageEnveloperMoc::create()
{
	return ImageEnveloperMocPtr(new ImageEnveloperMoc());
}

void ImageEnveloperMoc::setImages(std::vector<ImagePtr> images)
{
	mImages = images;
}

ImagePtr ImageEnveloperMoc::getEnvelopingImage()
{
	return mImages[0];
}


///--------------------------------------------------------

ImageEnveloperImplPtr ImageEnveloperImpl::create()
{
	return ImageEnveloperImplPtr(new ImageEnveloperImpl());
}

void ImageEnveloperImpl::setImages(std::vector<ImagePtr> images)
{
	mImages = images;
}

ImagePtr ImageEnveloperImpl::getEnvelopingImage()
{
	ImagePtr img = mImages[0];

	Parameters box = createEnvelopeParametersFromImage(img);

	ImagePtr retval = createEnvelopeFromParameters(box);

	return retval;
}

ImageEnveloperImpl::Parameters ImageEnveloperImpl::createEnvelopeParametersFromImage(ImagePtr img)
{
	Parameters retval;

	DoubleBoundingBox3D bb = findEnclosingBoundingBox(mImages, img->get_rMd().inverse());

	//Testvalues
	retval.mDim = Eigen::Array3i(img->getBaseVtkImageData()->GetDimensions());
//	retval.m_rMd =
//	retval.mDim = ;
	retval.mSpacing = Vector3D(img->getBaseVtkImageData()->GetSpacing());
	retval.mParentVolume = img->getUid();
	retval.m_rMd = img->get_rMd();
	return retval;
}

ImagePtr ImageEnveloperImpl::createEnvelopeFromParameters(Parameters box)
{
	vtkImageDataPtr imageData = generateVtkImageData(box.mDim, box.mSpacing, 0, 1);
	QString uid = QString("envelope_image_%1").arg(box.mParentVolume);
	ImagePtr retval(new Image(uid, imageData));
	retval->get_rMd_History()->setRegistration(box.m_rMd);
	retval->get_rMd_History()->setParentSpace(box.mParentVolume);
	retval->setAcquisitionTime(QDateTime::currentDateTime());
	retval->setModality("SC");
	return retval;
}

//ImagePtr ImageEnveloperImpl::createEnvelopeFromImage(ImagePtr img)
//{
//	vtkImageDataPtr imageData = generateVtkImageData(Eigen::Array3i(img->getBaseVtkImageData()->GetDimensions()),
//												Vector3D(img->getBaseVtkImageData()->GetSpacing()), 0, 1);
//	QString uid = QString("envelope_image_%1").arg(img->getUid());
//	ImagePtr retval(new Image(uid, imageData));
//	retval->get_rMd_History()->setRegistration(img->get_rMd());
//	retval->get_rMd_History()->setParentSpace(img->getUid());
//	retval->setAcquisitionTime(QDateTime::currentDateTime());
//	retval->setModality("SC");
//	return retval;
//}


} // namespace cx
