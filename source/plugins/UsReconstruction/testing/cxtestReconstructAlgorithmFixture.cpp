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
#include "catch.hpp"
#include "sscPNNReconstructAlgorithm.h"
#include "vtkImageData.h"
#include <QDomElement>


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
										m_dMr,
										0.0,
										0.0);
	std::cout << "Done sampling\n";
	REQUIRE(mInputData);
}

void ReconstructAlgorithmFixture::generateOutputVolume()
{
	mOutputData = vtkImageDataPtr::New();
	mOutputData->SetExtent(0, 99, 0, 99, 0, 99);
	mOutputData->SetSpacing(1, 1, 1);
	m_dMr = cx::Transform3D::Identity();
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
							mOutputData,
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

} // namespace cxtest
