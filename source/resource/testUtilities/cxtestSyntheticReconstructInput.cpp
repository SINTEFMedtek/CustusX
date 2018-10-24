/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestSyntheticReconstructInput.h"
#include "cxDummyTool.h"
#include "cxSimpleSyntheticVolume.h"
#include "cxtestSphereSyntheticVolume.h"
#include "cxTypeConversions.h"


namespace cxtest
{

SyntheticReconstructInput::SyntheticReconstructInput()
{
	mBounds = cx::Vector3D(99,99,99);

	mProbeMovementDefinition.mRangeNormalizedTranslation = cx::Vector3D::UnitX();
//	mProbeMovementDefinition.mRangeAngle = M_PI/8;
//	mProbeMovementDefinition.mSteps = 100;
	mProbeMovementDefinition.mRangeAngle = 0;
	mProbeMovementDefinition.mSteps = 200;

	mProbe = cx::DummyToolTestUtilities::createProbeDefinitionLinear(100, 100, Eigen::Array2i(200,200));
}

void SyntheticReconstructInput::defineProbeMovementNormalizedTranslationRange(double range)
{
	mProbeMovementDefinition.mRangeNormalizedTranslation = cx::Vector3D::UnitX() * range;
}
void SyntheticReconstructInput::defineProbeMovementAngleRange(double range)
{
	mProbeMovementDefinition.mRangeAngle = range;
}
void SyntheticReconstructInput::defineProbeMovementSteps(int steps)
{
	mProbeMovementDefinition.mSteps = steps;
}
void SyntheticReconstructInput::defineProbe(cx::ProbeDefinition probe)
{
	mProbe = probe;
}

void SyntheticReconstructInput::setOverallBoundsAndSpacing(double size, double spacing)
{
	// factors controlling sample rate:
	//  - output volume spacing
	//  - probe plane in-plane spacing
	//  - probe planes spacing (between the planes)
	//
	// set all these rates to the input spacing:

	mBounds = cx::Vector3D::Ones() * size;
	mProbe = cx::DummyToolTestUtilities::createProbeDefinitionLinear(size, size, Eigen::Array2i(1,1)*((std::lround(size/spacing))+1));
	mProbeMovementDefinition.mRangeNormalizedTranslation = cx::Vector3D::UnitX();
	mProbeMovementDefinition.mRangeAngle = 0;
	mProbeMovementDefinition.mSteps = size/spacing+1;
}

void SyntheticReconstructInput::printConfiguration()
{
	QString indent("");
	mPhantom->printInfo();
	std::cout << indent << "Probe:\n" << streamXml2String(mProbe) << std::endl;
	std::cout << indent << "ProbeMovement RangeNormalizedTranslation: " << mProbeMovementDefinition.mRangeNormalizedTranslation << std::endl;
	std::cout << indent << "ProbeMovement RangeAngle: " << mProbeMovementDefinition.mRangeAngle << std::endl;
	std::cout << indent << "ProbeMovement Steps: " << mProbeMovementDefinition.mSteps<< std::endl;
}

void SyntheticReconstructInput::setBoxAndLinesPhantom()
{
	mPhantom.reset(new cx::cxSimpleSyntheticVolume(mBounds));
}

void SyntheticReconstructInput::setSpherePhantom()
{
	mPhantom.reset(new cxtest::SphereSyntheticVolume(mBounds));
}

void SyntheticReconstructInput::setWireCrossPhantom()
{

}

std::vector<cx::Transform3D> SyntheticReconstructInput::generateFrames_rMt_tilted()
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
std::vector<cx::Transform3D> SyntheticReconstructInput::generateFrames(cx::Vector3D p0,
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

cx::USReconstructInputData SyntheticReconstructInput::generateSynthetic_USReconstructInputData()
{
	cx::Vector3D p0(mBounds[0]/2, mBounds[1]/2, 0); //probe starting point. pointing along z
	cx::Vector3D range_translation = mBounds[0] * mProbeMovementDefinition.mRangeNormalizedTranslation;
	double range_angle = mProbeMovementDefinition.mRangeAngle;
	int steps_full = 3*mProbeMovementDefinition.mSteps;

	// generate oversampled list of positions, for use both in tracking and imaging.
	std::vector<cx::Transform3D> rMt_full;
	rMt_full = this->generateFrames(p0,
									range_translation,
									range_angle,
									Eigen::Vector3d::UnitY(),
									steps_full);

	cx::USReconstructInputData result;

	// sample out tracking positions from the full list
	for (unsigned i=0; i<steps_full; i+=2)
	{
		cx::TimedPosition pos;
		pos.mTime = i;
		pos.mPos = rMt_full[i]; // TODO: skrell av rMpr
		result.mPositions.push_back(pos);
	}

	// sample out image frames from the full list
	std::vector<vtkImageDataPtr> frames;
	for (unsigned i=0; i<steps_full; i+=3)
	{
		cx::TimedPosition pos;
		pos.mTime = i;
		result.mFrames.push_back(pos);

		frames.push_back(mPhantom->sampleUsData(rMt_full[i], mProbe));
	}
	result.mUsRaw = cx::USFrameData::create("virtual", frames);

	// fill rest of info
	result.rMpr = cx::Transform3D::Identity(); // if <>Identity, remember to also change mPositions
	result.mProbeUid = "testProbe";
	result.mProbeDefinition.setData(mProbe);

	return result;
}

cx::ProcessedUSInputDataPtr SyntheticReconstructInput::generateSynthetic_ProcessedUSInputData(cx::Transform3D dMr)
{
	std::vector<cx::Transform3D> planes = this->generateFrames_rMt_tilted();
//	std::cout << "Starting sampling\n";
	cx::ProcessedUSInputDataPtr retval;
	retval = mPhantom->sampleUsData(planes, mProbe, dMr);
//	std::cout << "Done sampling\n";
	return retval;
}


} // namespace cxtest

