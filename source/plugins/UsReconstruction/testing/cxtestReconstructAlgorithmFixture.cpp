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
#include "sscDummyTool.h"
#include "sscTypeConversions.h"

namespace cxtest
{

ReconstructAlgorithmFixture::ReconstructAlgorithmFixture()
{
	mVerbose = false;
	mBounds = cx::Vector3D(99,99,99);
	this->defineOutputVolume(mBounds[0], 1);
//	this->defineOutputVolume(mBounds[0], 2);
//	this->defineOutputVolume(49, 2);

	mProbeMovementDefinition.mRangeNormalizedTranslation = cx::Vector3D::UnitX();
//	mProbeMovementDefinition.mRangeAngle = M_PI/8;
//	mProbeMovementDefinition.mSteps = 100;
	mProbeMovementDefinition.mRangeAngle = 0;
	mProbeMovementDefinition.mSteps = 200;

	mProbe = cx::DummyToolTestUtilities::createProbeDataLinear(100, 100, Eigen::Array2i(200,200));
}

void ReconstructAlgorithmFixture::defineOutputVolume(double bounds, double spacing)
{
	mOutputVolumeDefinition.mBounds = cx::Vector3D::Ones() * bounds;
	mOutputVolumeDefinition.mSpacing = cx::Vector3D::Ones() * spacing;
}


void ReconstructAlgorithmFixture::defineProbeMovementNormalizedTranslationRange(double range)
{
	mProbeMovementDefinition.mRangeNormalizedTranslation = cx::Vector3D::UnitX() * range;
}
void ReconstructAlgorithmFixture::defineProbeMovementAngleRange(double range)
{
	mProbeMovementDefinition.mRangeAngle = range;
}
void ReconstructAlgorithmFixture::defineProbeMovementSteps(int steps)
{
	mProbeMovementDefinition.mSteps = steps;
}
void ReconstructAlgorithmFixture::defineProbe(cx::ProbeDefinition probe)
{
	mProbe = probe;
}

void ReconstructAlgorithmFixture::setOverallBoundsAndSpacing(double size, double spacing)
{
	// factors controlling sample rate:
	//  - output volume spacing
	//  - probe plane in-plane spacing
	//  - probe planes spacing (between the planes)
	//
	// set all these rates to the input spacing:

	mBounds = cx::Vector3D::Ones() * size;
	this->defineOutputVolume(size, spacing);
	mProbe = cx::DummyToolTestUtilities::createProbeDataLinear(size, size, Eigen::Array2i(1,1)*(size/spacing+1));
	mProbeMovementDefinition.mRangeNormalizedTranslation = cx::Vector3D::UnitX();
	mProbeMovementDefinition.mRangeAngle = 0;
	mProbeMovementDefinition.mSteps = size/spacing+1;

}

void ReconstructAlgorithmFixture::printConfiguration()
{
	QString indent("");
	std::cout << "=== ReconstructAlgorithmFixture: Configuration: ===" << std::endl;
	mPhantom->printInfo();
	std::cout << indent << "Output Volume Bounds: " << mOutputVolumeDefinition.mBounds << std::endl;
	std::cout << indent << "Output Volume Spacing: " << mOutputVolumeDefinition.mSpacing << std::endl;
	std::cout << indent << "Probe:\n" << streamXml2String(mProbe) << std::endl;
	std::cout << indent << "ProbeMovement RangeNormalizedTranslation: " << mProbeMovementDefinition.mRangeNormalizedTranslation << std::endl;
	std::cout << indent << "ProbeMovement RangeAngle: " << mProbeMovementDefinition.mRangeAngle << std::endl;
	std::cout << indent << "ProbeMovement Steps: " << mProbeMovementDefinition.mSteps<< std::endl;
	std::cout << "======" << std::endl;
}

/*
// Work in progress - CA/2013-11-27 - test entire rec pipeline.
void ReconstructAlgorithmFixture::generateSynthetic_USReconstructInputData()
{
//	cx::Vector3D mBounds(100,100,100);
//	cx::cxSyntheticVolumePtr mPhantom;
	mPhantom.reset(new cxtest::SphereSyntheticVolume(mBounds));
	cx::ProbeDefinition mProbe; // TODO Define

	cx::Vector3D p0(mBounds[0]/2, mBounds[1]/2, 0); //probe starting point. pointing along z
	cx::Vector3D range_translation = mBounds[0] * mProbeMovementDefinition.mRangeNormalizedTranslation;
	double range_angle = mProbeMovementDefinition.mRangeAngle;
//	int steps = mProbeMovementDefinition.mSteps;
	int steps_full = 3*mProbeMovementDefinition.mSteps;

	std::vector<cx::Transform3D> rMt_full;
	rMt_full = this->generateFrames(p0,
									range_translation,
									range_angle,
									Eigen::Vector3d::UnitY(),
									steps_full);

	cx::USReconstructInputData result;

	for (unsigned i=0; i<steps_full/2; ++i)
	{
		cx::TimedPosition pos;
		pos.mTime = i;
		pos.mPos = rMt_full[i]; // TODO: skrell av rMpr
		result.mPositions.push_back(pos);
	}

	std::vector<vtkImageDataPtr> frames;
	for (unsigned i=0; i<steps_full/3; ++i)
	{
		cx::TimedPosition pos;
		pos.mTime = i;
		result.mFrames.push_back(pos);

		frames.push_back(mPhantom->sampleUsData(rMt_full[i], mProbe));
	}
	result.mUsRaw = cx::USFrameData::create(frames);

	result.rMpr = cx::Transform3D::Identity();
	result.mProbeUid = "testProbe";
	result.mProbeData.setData(mProbe);

}*/



void ReconstructAlgorithmFixture::setAlgorithm(cx::ReconstructAlgorithmPtr algorithm)
{
	mAlgorithm = algorithm;
}

void ReconstructAlgorithmFixture::setBoxAndLinesPhantom()
{
	mPhantom.reset(new cx::cxSimpleSyntheticVolume(mBounds));
}

void ReconstructAlgorithmFixture::setSpherePhantom()
{
	mPhantom.reset(new cxtest::SphereSyntheticVolume(mBounds));
}


void ReconstructAlgorithmFixture::setWireCrossPhantom()
{

}

std::vector<cx::Transform3D> ReconstructAlgorithmFixture::generateFrames_rMt_tilted()
{
	cx::Vector3D p0(mBounds[0]/2, mBounds[1]/2, 0); //probe starting point. pointing along z
	cx::Vector3D range_translation = mBounds[0] * mProbeMovementDefinition.mRangeNormalizedTranslation;
	double range_angle = mProbeMovementDefinition.mRangeAngle;
	int steps = mProbeMovementDefinition.mSteps;

	// generate transforms from tool to reference.
	return this->generateFrames(p0,
								range_translation,
								range_angle,
								Eigen::Vector3d::UnitY(),
								steps);
}

/** Generate a sequence of planes using the input definition.
  * The planes work around p0, applying translation and rotation
  * simultaneously.
  */
std::vector<cx::Transform3D> ReconstructAlgorithmFixture::generateFrames(cx::Vector3D p0,
																		 cx::Vector3D range_translation,
																		 double range_angle,
																		 cx::Vector3D rotation_axis,
																		 int steps)
{
	// generate transforms from tool to reference.
	std::vector<cx::Transform3D> planes;
	for(int i = 0; i < steps; ++i)
	{
		double R = steps-1;
		double t = (i-R/2)/R; // range [-0.5 .. 0.5]
		cx::Transform3D transform = cx::Transform3D::Identity();
		transform.translation() = p0 + range_translation*t;
		transform.rotate(Eigen::AngleAxisd(t*range_angle, rotation_axis));
		planes.push_back(transform);
	}
	return planes;
}

void ReconstructAlgorithmFixture::generateInput()
{
	REQUIRE(mPhantom);
	REQUIRE(mOutputData);

	std::vector<cx::Transform3D> planes = this->generateFrames_rMt_tilted();
	std::cout << "Starting sampling\n";
	mInputData = mPhantom->sampleUsData(planes,
										mProbe,
										mOutputData->get_rMd().inv());
	std::cout << "Done sampling\n";
	REQUIRE(mInputData);
}

cx::ImagePtr ReconstructAlgorithmFixture::createOutputVolume(QString name)
{
	Eigen::Array3i dim = Eigen::Array3i((mBounds.array()/mOutputVolumeDefinition.mSpacing.array()).cast<int>())+1;
	vtkImageDataPtr data = cx::generateVtkImageData(dim, mOutputVolumeDefinition.mSpacing, 0);
	cx::Transform3D rMd = cx::createTransformTranslate((mBounds-mOutputVolumeDefinition.mBounds)/2);

	cx::ImagePtr retval(new cx::Image(name, data));
	retval->get_rMd_History()->setRegistration(rMd);

	return retval;
}

void ReconstructAlgorithmFixture::generateOutputVolume()
{
	mOutputData = this->createOutputVolume("output");
}

void ReconstructAlgorithmFixture::reconstruct()
{
	if (this->getVerbose())
		this->printConfiguration();

	REQUIRE(mPhantom);
	REQUIRE(mAlgorithm);

	if (!mOutputData)
		this->generateOutputVolume();
	if (!mInputData)
		this->generateInput();

	if (this->getVerbose())
		std::cout << "Reconstructing\n";
	QDomDocument domDoc;
	QDomElement root = domDoc.createElement("TordTest");

	mAlgorithm->reconstruct(mInputData,
							mOutputData->getBaseVtkImageData(),
							root);
	if (this->getVerbose())
		std::cout << "Reconstruction done\n";
}

void ReconstructAlgorithmFixture::checkRMSBelow(double threshold)
{
	float sse = this->getRMS();
	if (this->getVerbose())
		std::cout << "RMS value: " << sse << std::endl;
	CHECK(sse < threshold);
}

double ReconstructAlgorithmFixture::getRMS()
{
	double sse = cx::calculateRMSError(mOutputData->getBaseVtkImageData(), this->getNominalOutputImage()->getBaseVtkImageData());
//	float sse = mPhantom->computeRMSError(mOutputData);
//	std::cout << "RMS value: " << sse << std::endl;
	return sse;
}

cx::ImagePtr ReconstructAlgorithmFixture::getNominalOutputImage()
{
	if (!mNominalOutputImage)
	{
		mNominalOutputImage = this->createOutputVolume("nominal");
		mPhantom->fillVolume(mNominalOutputImage);
	}
	return mNominalOutputImage;
}

void ReconstructAlgorithmFixture::checkCentroidDifferenceBelow(double val)
{
	cx::Vector3D c_n = calculateCentroid(this->getNominalOutputImage());
	cx::Vector3D c_r = calculateCentroid(mOutputData);

	double difference = (c_n-c_r).norm();

	if (this->getVerbose())
		std::cout << "c_n=[" << c_n << "] c_r=[" << c_r << "] diff=[" << difference << "]" << std::endl;

	CHECK(difference < val);
}

void ReconstructAlgorithmFixture::checkMassDifferenceBelow(double val)
{
	double v_n = calculateMass(this->getNominalOutputImage());
	double v_r = calculateMass(mOutputData);
	double normalized_difference = fabs(v_n-v_r)/(v_n+v_r);

	if (this->getVerbose())
		std::cout << "v_n=[" << v_n << "] v_r=[" << v_r << "] diff=[" << normalized_difference << "]" << std::endl;

	CHECK(normalized_difference<val);
}

void ReconstructAlgorithmFixture::saveNominalOutputToFile(QString filename)
{
	cx::MetaImageReader().saveImage(this->getNominalOutputImage(), filename);
}

void ReconstructAlgorithmFixture::saveOutputToFile(QString filename)
{
	cx::MetaImageReader().saveImage(mOutputData, filename);
}

} // namespace cxtest
