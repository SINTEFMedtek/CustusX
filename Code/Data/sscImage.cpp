#include "sscImage.h"

namespace ssc
{

Image::~Image()
{
}

Image::Image(const std::string& uid, const vtkImageDataPtr& data) : mUid(uid), mName(uid), mImageData(data)
{
}

void Image::setVtkImageData(const vtkImageDataPtr& data)
{
	mImageData = data;
}
void Image::setName(const std::string& name)
{
	mName = name;
}

void Image::setTransform(const Transform3D& trans)
{
	mTransform = trans;
}

std::string Image::getUid() const
{
	return mUid;
}

std::string Image::getName() const
{
	return mName;
}

Transform3D Image::getTransform() const
{
	return mTransform;
}

REGISTRATION_STATUS Image::getRegistrationStatus() const
{
	return rsNOT_REGISTRATED;
}

vtkImageDataPtr Image::getVtkImageData()
{
	return mImageData;
}

vtkPointsPtr Image::getLandmarks()
{
	return mLandmarks;
}
void Image::connectRep(const RepWeakPtr& rep)
{
	mReps.insert(rep);
}
void Image::disconnectRep(const RepWeakPtr& rep)
{
	mReps.erase(rep);
}
} // namespace ssc
