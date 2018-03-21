/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestReconstructionAlgorithmFixture.h"

#include "cxReconstructionMethodService.h"
#include "cxSimpleSyntheticVolume.h"
#include "cxtestSphereSyntheticVolume.h"
#include "catch.hpp"
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

ReconstructionAlgorithmFixture::ReconstructionAlgorithmFixture()
{
	mInputGenerator.reset(new SyntheticReconstructInput);

	mVerbose = false;
	this->defineOutputVolume(mInputGenerator->getBounds()[0], 1);
}

void ReconstructionAlgorithmFixture::defineOutputVolume(double bounds, double spacing)
{
	mOutputVolumeDefinition.mBounds = cx::Vector3D::Ones() * bounds;
	mOutputVolumeDefinition.mSpacing = cx::Vector3D::Ones() * spacing;
}


void ReconstructionAlgorithmFixture::setOverallBoundsAndSpacing(double size, double spacing)
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

void ReconstructionAlgorithmFixture::printConfiguration()
{
	QString indent("");
	std::cout << "=== ReconstructAlgorithmFixture: Configuration: ===" << std::endl;
//	this-mPhantom->printInfo();
	mInputGenerator->printConfiguration();
	std::cout << indent << "Output Volume Bounds: " << mOutputVolumeDefinition.mBounds << std::endl;
	std::cout << indent << "Output Volume Spacing: " << mOutputVolumeDefinition.mSpacing << std::endl;
	std::cout << "======" << std::endl;
}

void ReconstructionAlgorithmFixture::setAlgorithm(cx::ReconstructionMethodService* algorithm)
{
	mAlgorithm = algorithm;
}

void ReconstructionAlgorithmFixture::generateInput()
{
	REQUIRE(this->getPhantom());
	REQUIRE(mOutputData);
	cx::Transform3D dMr = mOutputData->get_rMd().inv();
	mInputData = mInputGenerator->generateSynthetic_ProcessedUSInputData(dMr);
	REQUIRE(mInputData);
}

cx::ImagePtr ReconstructionAlgorithmFixture::createOutputVolume(QString name)
{
	cx::Vector3D bounds = this->getPhantom()->getBounds();
	Eigen::Array3i dim = Eigen::Array3i((bounds.array()/mOutputVolumeDefinition.mSpacing.array()).cast<int>())+1;
	vtkImageDataPtr data = cx::generateVtkImageData(dim, mOutputVolumeDefinition.mSpacing, 0);
	cx::Transform3D rMd = cx::createTransformTranslate((bounds-mOutputVolumeDefinition.mBounds)/2);

	cx::ImagePtr retval(new cx::Image(name, data));
	retval->get_rMd_History()->setRegistration(rMd);

	return retval;
}

void ReconstructionAlgorithmFixture::generateOutputVolume()
{
	mOutputData = this->createOutputVolume("output");
}

void ReconstructionAlgorithmFixture::reconstruct(QDomElement root)
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

SyntheticVolumeComparerPtr ReconstructionAlgorithmFixture::getComparer()
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

void ReconstructionAlgorithmFixture::checkRMSBelow(double threshold)
{
	this->getComparer()->checkRMSBelow(threshold);
}

void ReconstructionAlgorithmFixture::checkCentroidDifferenceBelow(double val)
{
	this->getComparer()->checkCentroidDifferenceBelow(val);
}

void ReconstructionAlgorithmFixture::checkMassDifferenceBelow(double val)
{
	this->getComparer()->checkMassDifferenceBelow(val);
}

void ReconstructionAlgorithmFixture::saveNominalOutputToFile(QString filename)
{
	this->getComparer()->saveNominalOutputToFile(filename);
}

void ReconstructionAlgorithmFixture::saveOutputToFile(QString filename)
{
	this->getComparer()->saveOutputToFile(filename);
}

} // namespace cxtest
