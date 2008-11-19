#include "sscImage.h"

#include <vtkImageReslice.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>

//#define USE_TRANSFORM_RESCLICER

namespace ssc
{

Image::~Image()
{
}

Image::Image(const std::string& uid, const vtkImageDataPtr& data) : 
	mUid(uid), mName(uid), mBaseImageData(data)
{
	mOutputImageData = mBaseImageData;
#ifdef USE_TRANSFORM_RESCLICER
	mOrientator = vtkImageReslicePtr::New();
	mOrientator->SetInput(mBaseImageData);
	mOutputImageData = mOrientator->GetOutput();
	mOutputImageData->Update();
	mOutputImageData->UpdateInformation();	
#endif
}

void Image::setVtkImageData(const vtkImageDataPtr& data)
{
	mBaseImageData = data;
}
void Image::setName(const std::string& name)
{
	mName = name;
}

void Image::setTransform(const Transform3D& trans)
{
	mTransform = trans;
	
#ifdef USE_TRANSFORM_RESCLICER
	mOrientator->SetResliceAxes(mTransform.matrix());	
	mOutputImageData->Update();
	mOutputImageData->UpdateInformation();
#endif
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

vtkImageDataPtr Image::getBaseVtkImageData()
{
	return mBaseImageData;
}

vtkImageDataPtr Image::getRefVtkImageData()
{
	return mOutputImageData;
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
