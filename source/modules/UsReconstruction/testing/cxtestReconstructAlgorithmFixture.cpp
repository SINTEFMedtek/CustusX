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

#include "cxReconstructAlgorithm.h"
#include "TordReconstruct/cxSimpleSyntheticVolume.h"
#include "cxtestSphereSyntheticVolume.h"
#include "catch.hpp"
#include "cxPNNReconstructAlgorithm.h"
#include "cxDataReaderWriter.h"
#include "vtkImageData.h"
#include <QDomElement>
#include "cxImage.h"
#include "cxRegistrationTransform.h"
#include "cxVolumeHelpers.h"
#include "cxDummyTool.h"
#include "cxTypeConversions.h"
#include "cxImageDataContainer.h"

namespace cxtest
{

ReconstructAlgorithmFixture::ReconstructAlgorithmFixture()
{
	mInputGenerator.reset(new SyntheticReconstructInput);

	mVerbose = false;
	this->defineOutputVolume(mInputGenerator->getBounds()[0], 1);
}

void ReconstructAlgorithmFixture::defineOutputVolume(double bounds, double spacing)
{
	mOutputVolumeDefinition.mBounds = cx::Vector3D::Ones() * bounds;
	mOutputVolumeDefinition.mSpacing = cx::Vector3D::Ones() * spacing;
}


void ReconstructAlgorithmFixture::setOverallBoundsAndSpacing(double size, double spacing)
{
	// factors controlling sample rate:
	//  - output volume spacing
	//  - probe plane in-plane spacing
	//  - probe planes spacing (between the planes)
	//
	// set all these rates to the input spacing:

	mInputGenerator->setOverallBoundsAndSpacing(size, spacing);
	this->defineOutputVolume(size, spacing);
}

void ReconstructAlgorithmFixture::printConfiguration()
{
	QString indent("");
	std::cout << "=== ReconstructAlgorithmFixture: Configuration: ===" << std::endl;
//	this-mPhantom->printInfo();
	mInputGenerator->printConfiguration();
	std::cout << indent << "Output Volume Bounds: " << mOutputVolumeDefinition.mBounds << std::endl;
	std::cout << indent << "Output Volume Spacing: " << mOutputVolumeDefinition.mSpacing << std::endl;
	std::cout << "======" << std::endl;
}

void ReconstructAlgorithmFixture::setAlgorithm(cx::ReconstructionServicePtr algorithm)
{
	mAlgorithm = algorithm;
}

void ReconstructAlgorithmFixture::generateInput()
{
	REQUIRE(this->getPhantom());
	REQUIRE(mOutputData);
	cx::Transform3D dMr = mOutputData->get_rMd().inv();
	mInputData = mInputGenerator->generateSynthetic_ProcessedUSInputData(dMr);
	REQUIRE(mInputData);
}

cx::ImagePtr ReconstructAlgorithmFixture::createOutputVolume(QString name)
{
	cx::Vector3D bounds = this->getPhantom()->getBounds();
	Eigen::Array3i dim = Eigen::Array3i((bounds.array()/mOutputVolumeDefinition.mSpacing.array()).cast<int>())+1;
	vtkImageDataPtr data = cx::generateVtkImageData(dim, mOutputVolumeDefinition.mSpacing, 0);
	cx::Transform3D rMd = cx::createTransformTranslate((bounds-mOutputVolumeDefinition.mBounds)/2);

	cx::ImagePtr retval(new cx::Image(name, data));
	retval->get_rMd_History()->setRegistration(rMd);

	return retval;
}

void ReconstructAlgorithmFixture::generateOutputVolume()
{
	mOutputData = this->createOutputVolume("output");
}

void ReconstructAlgorithmFixture::reconstruct(QDomElement root)
{
	if (this->getVerbose())
		this->printConfiguration();

	REQUIRE(this->getPhantom());
	REQUIRE(mAlgorithm);

	if (!mOutputData)
		this->generateOutputVolume();
	if (!mInputData)
		this->generateInput();

	if (this->getVerbose())
		std::cout << "Reconstructing\n";

	REQUIRE(mAlgorithm->reconstruct(mInputData,
							mOutputData->getBaseVtkImageData(),
							root));
	if (this->getVerbose())
		std::cout << "Reconstruction done\n";
}

SyntheticVolumeComparerPtr ReconstructAlgorithmFixture::getComparer()
{
	if (!mComparer)
	{
		mComparer.reset(new SyntheticVolumeComparer());
		mComparer->setVerbose(this->getVerbose());
		mComparer->setPhantom(this->getPhantom());
		mComparer->setTestImage(mOutputData);
	}
	return mComparer;
}

void ReconstructAlgorithmFixture::checkRMSBelow(double threshold)
{
	this->getComparer()->checkRMSBelow(threshold);
}

void ReconstructAlgorithmFixture::checkCentroidDifferenceBelow(double val)
{
	this->getComparer()->checkCentroidDifferenceBelow(val);
}

void ReconstructAlgorithmFixture::checkMassDifferenceBelow(double val)
{
	this->getComparer()->checkMassDifferenceBelow(val);
}

void ReconstructAlgorithmFixture::saveNominalOutputToFile(QString filename)
{
	this->getComparer()->saveNominalOutputToFile(filename);
}

void ReconstructAlgorithmFixture::saveOutputToFile(QString filename)
{
	this->getComparer()->saveOutputToFile(filename);
}

} // namespace cxtest
