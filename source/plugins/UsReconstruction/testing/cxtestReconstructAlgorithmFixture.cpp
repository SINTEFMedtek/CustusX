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

#include "cxtestReconstructAlgorithmFixture.h"

#include "sscReconstructAlgorithm.h"
#include "TordReconstruct/cxSimpleSyntheticVolume.h"
#include "cxtestSphereSyntheticVolume.h"
#include "catch.hpp"
#include "sscPNNReconstructAlgorithm.h"
#include "sscDataReaderWriter.h"
#include "vtkImageData.h"
#include <QDomElement>
#include "sscImage.h"
#include "sscRegistrationTransform.h"
#include "sscVolumeHelpers.h"

namespace cxtest
{

void ReconstructAlgorithmFixture::setAlgorithm(cx::ReconstructAlgorithm* algorithm)
{
	mAlgorithm = algorithm;
}

void ReconstructAlgorithmFixture::setBoxAndLinesPhantom()
{
	Eigen::Array3i dims(100, 100, 100);
	mPhantom.reset(new cx::cxSimpleSyntheticVolume(dims));
}

void ReconstructAlgorithmFixture::setSpherePhantom()
{
	Eigen::Array3i dims(100, 100, 100);
	mPhantom.reset(new cxtest::SphereSyntheticVolume(dims, cx::Vector3D(50,50,50), 10));
}


void ReconstructAlgorithmFixture::setWireCrossPhantom()
{

}

std::vector<cx::Transform3D> ReconstructAlgorithmFixture::generateFrames_rMf_tilted()
{
	// generate transforms from frame to reference.
	std::vector<cx::Transform3D> planes;
	for(int i = 0; i < 100; i++)
	{
		cx::Transform3D transform = cx::Transform3D::Identity();
		cx::Vector3D translation(0,0,i);
		transform.translation() = translation;
		transform.rotate(Eigen::AngleAxisd((double)(i-50)/100 *M_PI/8, Eigen::Vector3d::UnitY()));
		planes.push_back(transform);
	}
	return planes;
}

void ReconstructAlgorithmFixture::generateInput()
{
	REQUIRE(mPhantom);
	REQUIRE(mOutputData);

	std::vector<cx::Transform3D> planes = this->generateFrames_rMf_tilted();
	Eigen::Array2f pixelSpacing(0.5f, 0.5f);
	Eigen::Array2i us_dims(200, 200);
	std::cout << "Starting sampling\n";
	mInputData = mPhantom->sampleUsData(planes,
										pixelSpacing,
										us_dims,
										mOutputData->get_rMd().inv(),
										0.0,
										0.0);
	std::cout << "Done sampling\n";
	REQUIRE(mInputData);
}

void ReconstructAlgorithmFixture::generateOutputVolume()
{
	Eigen::Array3i dim(100,100,100);
	cx::Vector3D spacing = cx::Vector3D(1, 1, 1) * 1;
	vtkImageDataPtr data = cx::generateVtkImageData(dim, spacing, 0);
	cx::Transform3D rMd = cx::Transform3D::Identity();

	mOutputData = cx::ImagePtr(new cx::Image("output", data));
	mOutputData->get_rMd_History()->setRegistration(rMd);
}

void ReconstructAlgorithmFixture::reconstruct()
{
	REQUIRE(mInputData);
	REQUIRE(mPhantom);
	REQUIRE(mOutputData);
	REQUIRE(mAlgorithm);

	std::cout << "Reconstructing\n";
	QDomDocument domDoc;
	QDomElement root = domDoc.createElement("TordTest");

	mAlgorithm->reconstruct(mInputData,
							mOutputData->getBaseVtkImageData(),
							root);
	std::cout << "Reconstruction done\n";
}

void ReconstructAlgorithmFixture::checkRMSBelow(double threshold)
{
	float sse = this->getRMS();
	std::cout << "RMS value: " << sse << std::endl;
	REQUIRE(sse < threshold);
}

double ReconstructAlgorithmFixture::getRMS()
{
	float sse = mPhantom->computeRMSError(mOutputData);
	std::cout << "RMS value: " << sse << std::endl;
	return sse;
}

//void ReconstructAlgorithmFixture::saveNominalOutputToFile(QString filename)
//{
//	vtkImageDataPtr data = vtkImageDataPtr::New();
//	data->DeepCopy(mOutputData->getBaseVtkImageData());



//	cx::ImagePtr image = cx::ImagePtr(new cx::Image("nominal", data));
//	image->get_rMd_History()->setRegistration(mOutputData->get_rMd());

//	cx::MetaImageReader().saveImage(image, filename);
//}

void ReconstructAlgorithmFixture::saveOutputToFile(QString filename)
{
	cx::MetaImageReader().saveImage(mOutputData, filename);
}

} // namespace cxtest
