#include "sscImage.h"

#include <vtkImageReslice.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkDoubleArray.h>

#include <sstream>

#define USE_TRANSFORM_RESCLICER

namespace ssc
{

Image::~Image()
{
}

Image::Image(const std::string& uid, const vtkImageDataPtr& data) :
	mImageTransferFunctions3D(data),
	mImageLookupTable2D(data),
	mUid(uid), mName(uid), mBaseImageData(data),
	mLandmarks(vtkDoubleArray::New())
{
	mOutputImageData = mBaseImageData;
#ifdef USE_TRANSFORM_RESCLICER
	//mBaseImageData->ReleaseDataFlagOn();
	mOrientator = vtkImageReslicePtr::New();
	mOrientator->SetInput(mBaseImageData);
	mOutputImageData = mOrientator->GetOutput();
	mOutputImageData->Update();
	//mOutputImageData->UpdateInformation();
#endif
	mLandmarks->SetNumberOfComponents(3);

	mOutputImageData->GetScalarRange();	// this line updates some internal vtk value, and (on fedora) removes 4.5s in the second render().
}

void Image::setVtkImageData(const vtkImageDataPtr& data)
{
	mBaseImageData = data;
#ifdef USE_TRANSFORM_RESCLICER
	mOrientator->SetInput(mBaseImageData);
	mOrientator->SetResliceAxes(mTransform.matrix());
	mOutputImageData->Update();
	mOutputImageData->UpdateInformation();
	mOutputImageData->GetScalarRange();	// this line updates some internal vtk value, and (on fedora) removes 4.5s in the second render().
#endif
	mImageTransferFunctions3D = ImageTF3D(data);
	mImageLookupTable2D = ImageLUT2D(data);


	emit vtkImageDataChanged();
}

ImageTF3D& Image::transferFunctions3D()
{
	return mImageTransferFunctions3D;
}

ImageLUT2D& Image::lookupTable2D()
{
	return mImageLookupTable2D;
}

void Image::setName(const std::string& name)
{
	mName = name;
}

void Image::setTransform(const Transform3D& trans)
{
	if (similar(trans, mTransform))
	{
		return;
	}

	mTransform = trans;

#ifdef USE_TRANSFORM_RESCLICER
	mOrientator->SetResliceAxes(mTransform.matrix());
	mOutputImageData->Update();
	mOutputImageData->UpdateInformation();
	mOutputImageData->GetScalarRange();	// this line updates some internal vtk value, and (on fedora) removes 4.5s in the second render().
#endif

	emit vtkImageDataChanged();
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

vtkDoubleArrayPtr Image::getLandmarks()
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
void Image::addLandmarkSlot(double x, double y, double z)
{
	double point[3] = {x, y, z};
	mLandmarks->InsertNextTupleValue(point);
	emit landmarkAdded(x, y, z);
}
void Image::removeLandmarkSlot(double x, double y, double z)
{

	int numberOfLandmarks = mLandmarks->GetNumberOfTuples();
	for(int i=0; i<= numberOfLandmarks-1; i++)
	{
		double* point = mLandmarks->GetTuple(i);
		if(point[0] == x && point[1] == y && point[2] == z)
			mLandmarks->RemoveTuple(i);
	}
	emit landmarkRemoved(x, y, z);
}
void Image::printLandmarks()
{
	std::cout << "Landmarks: " << std::endl;
	for(int i=0; i<= mLandmarks->GetNumberOfTuples()-1; i++)
	{
		double* point = mLandmarks->GetTuple(i);
		std::stringstream stream;
		stream << i << ": (";
		stream << point[0];
		stream << ",";
		stream << point[1];
		stream << ",";
		stream << point[2];
		stream << ")";
		std::cout << stream.str() << std::endl;
	}
}
//void Image::setLut(const vtkLookupTablePtr& lut)
//{
//	mLut = lut;
//}
//
//vtkLookupTablePtr Image::getLut() const
//{
//	return mLut;
//}


} // namespace ssc
