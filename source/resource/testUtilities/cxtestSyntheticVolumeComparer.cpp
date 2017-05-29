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

#include "cxtestSyntheticVolumeComparer.h"
#include "cxVolumeHelpers.h"
#include "catch.hpp"
#include "cxImage.h"
#include "cxtestUtilities.h"
#include <vtkImageData.h>
#include "cxRegistrationTransform.h"
#include <QDir>
#include <QString>
#include "cxTypeConversions.h"
#include "cxFileManagerService.h"

namespace cxtest
{

SyntheticVolumeComparer::SyntheticVolumeComparer() : mVerbose(false)
{
}

void SyntheticVolumeComparer::setPhantom(cx::cxSyntheticVolumePtr phantom)
{
	mPhantom = phantom;
	mNominalImage.reset();
}

void SyntheticVolumeComparer::setTestImage(cx::ImagePtr image)
{
	mTestImage = image;
	mNominalImage.reset();
}

//vtkImageDataPtr Utilities::create3DVtkImageData(Eigen::Array3i dim, const unsigned int voxelValue)
//{
//	return cx::generateVtkImageData(dim, cx::Vector3D(1,1,1), voxelValue);
//}

//cx::ImagePtr Utilities::create3DImage(Eigen::Array3i dim, const unsigned int voxelValue)
//{
//	vtkImageDataPtr vtkImageData = create3DVtkImageData(dim, voxelValue);
//	QString unique_string = qstring_cast(reinterpret_cast<long>(vtkImageData.GetPointer()));
//	QString imagesUid = QString("TESTUID_%2_%1").arg(unique_string);
//	cx::ImagePtr image(new cx::Image(imagesUid, vtkImageData));

//	return image;
//}


cx::ImagePtr SyntheticVolumeComparer::getNominalOutputImage() const
{
	REQUIRE(mTestImage);

	if (!mNominalImage)
	{
		Eigen::Array3i dim(mTestImage->getBaseVtkImageData()->GetDimensions());
		cx::Vector3D spacing(mTestImage->getBaseVtkImageData()->GetSpacing());
		mNominalImage = cxtest::Utilities::create3DImage(dim, spacing, 0);
		mNominalImage->get_rMd_History()->setRegistration(mTestImage->get_rMd());

		mPhantom->fillVolume(mNominalImage);
	}

	return mNominalImage;
}

void SyntheticVolumeComparer::checkRMSBelow(double threshold)
{
	float sse = this->getRMS();
	if (this->getVerbose())
		std::cout << "RMS value: " << sse << std::endl;
	CHECK(sse < threshold);
}

double SyntheticVolumeComparer::getRMS() const
{
	double sse = cx::calculateRMSError(mTestImage->getBaseVtkImageData(), this->getNominalOutputImage()->getBaseVtkImageData());
//	float sse = mPhantom->computeRMSError(mOutputData);
//	std::cout << "RMS value: " << sse << std::endl;
	return sse;
}

void SyntheticVolumeComparer::checkCentroidDifferenceBelow(double val)
{
	cx::Vector3D c_n = calculateCentroid(this->getNominalOutputImage());
	cx::Vector3D c_r = calculateCentroid(mTestImage);

	double difference = (c_n-c_r).norm();

	if (this->getVerbose())
		std::cout << "c_n=[" << c_n << "] c_r=[" << c_r << "] diff=[" << difference << "]" << std::endl;

	CHECK(difference < val);
}

void SyntheticVolumeComparer::checkMassDifferenceBelow(double val)
{
	double v_n = calculateMass(this->getNominalOutputImage());
	double v_r = calculateMass(mTestImage);
	double normalized_difference = fabs(v_n-v_r)/(v_n+v_r);

	if (this->getVerbose())
		std::cout << "v_n=[" << v_n << "] v_r=[" << v_r << "] diff=[" << normalized_difference << "]" << std::endl;

	CHECK(normalized_difference<val);
}

void SyntheticVolumeComparer::checkValueWithin(cx::Vector3D p_r, int lowerLimit, int upperLimit)
{
	int val = this->getValue(mTestImage, p_r);
	if (this->getVerbose())
		std::cout << QString("p_r[%1]=%2, limit=[%3, %4]").arg(qstring_cast(p_r)).arg(val).arg(lowerLimit).arg(upperLimit) << std::endl;
	CHECK(val>=lowerLimit);
	CHECK(val<=upperLimit);
}

double SyntheticVolumeComparer::getValue(cx::ImagePtr image, cx::Vector3D p_r)
{
	vtkImageDataPtr raw = image->getBaseVtkImageData();
	cx::Vector3D p_d = image->get_rMd().inv().coord(p_r);
	Eigen::Array3d spacing(raw->GetSpacing());
	Eigen::Array3i voxel = cx::round(p_r.array() / spacing).cast<int>();
	double val = raw->GetScalarComponentAsDouble(voxel.data()[0],voxel.data()[1],voxel.data()[2], 0);
	return val;
}

void SyntheticVolumeComparer::saveNominalOutputToFile(QString filename, cx::FileManagerServicePtr port)
{
	port->saveImage(this->getNominalOutputImage(), this->addFullPath(filename));
}

void SyntheticVolumeComparer::saveOutputToFile(QString filename, cx::FileManagerServicePtr port)
{
	port->saveImage(mTestImage, this->addFullPath(filename));
}

QString SyntheticVolumeComparer::addFullPath(QString filename)
{
	QDir dir(QDir::homePath() + "/test/");
	dir.mkdir(".");
	QString fullFilename = dir.absoluteFilePath(filename);
	return fullFilename;
}

} // namespace cxtest


