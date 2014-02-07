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

#ifndef CXTESTSYNTHETICRECONSTRUCTINPUT_H
#define CXTESTSYNTHETICRECONSTRUCTINPUT_H

#include "sscReconstructAlgorithm.h"
#include "TordReconstruct/cxSimpleSyntheticVolume.h"
#include "cxtestSyntheticVolumeComparer.h"

namespace cxtest
{

typedef boost::shared_ptr<class SyntheticReconstructInput> SyntheticReconstructInputPtr;

/** Generate synthetic US reconstruct input data based on a SyntheticVolume.
 *
 *
 * \ingroup cx
 * \date 12.12.2013, 2013
 * \author christiana
 */
class SyntheticReconstructInput
{
public:
	SyntheticReconstructInput();
	void defineProbeMovementNormalizedTranslationRange(double range);
	void defineProbeMovementAngleRange(double range);
	void defineProbeMovementSteps(int steps);
	void defineProbe(cx::ProbeDefinition probe);
	void setOverallBoundsAndSpacing(double size, double spacing);

	void setBoxAndLinesPhantom();
	void setSpherePhantom();
	void setWireCrossPhantom();

	cx::cxSyntheticVolumePtr getPhantom() { return mPhantom; }
	void printConfiguration();
	cx::Vector3D getBounds() const { return mBounds; }

	//	SyntheticReconstructInput();
	cx::USReconstructInputData generateSynthetic_USReconstructInputData();
	cx::ProcessedUSInputDataPtr generateSynthetic_ProcessedUSInputData(cx::Transform3D dMr);

private:
	std::vector<cx::Transform3D> generateFrames_rMt_tilted();
	/** Generate a sequence of planes using the input definition.
	  * The planes work around p0, applying translation and rotation
	  * simultaneously.
	  */
	std::vector<cx::Transform3D> generateFrames(cx::Vector3D p0,
												cx::Vector3D range_translation,
												double range_angle,
												cx::Vector3D rotation_axis,
												int steps);

	cx::cxSyntheticVolumePtr mPhantom;

	// setup parameters
	cx::Vector3D mBounds;
	cx::ProbeDefinition mProbe;

	struct ProbeMovement
	{
		cx::Vector3D mRangeNormalizedTranslation;
		double mRangeAngle;
		double mSteps;
	};
	ProbeMovement mProbeMovementDefinition;

};


} // namespace cxtest


#endif // CXTESTSYNTHETICRECONSTRUCTINPUT_H
