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

#include "cxtestReconstructionAlgorithmFixture.h"

#include "cxReconstructionMethodService.h"
#include "cxSimpleSyntheticVolume.h"
#include "cxtestSphereSyntheticVolume.h"
#include "catch.hpp"
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

void ReconstructionAlgorithmFixture::saveNominalOutputToFile(QString filename, cx::FileManagerServicePtr filemanager)
{
	this->getComparer()->saveNominalOutputToFile(filename, filemanager);
}

void ReconstructionAlgorithmFixture::saveOutputToFile(QString filename, cx::FileManagerServicePtr filemanager)
{
	this->getComparer()->saveOutputToFile(filename, filemanager);
}

} // namespace cxtest
