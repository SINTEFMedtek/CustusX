/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTSYNTHETICRECONSTRUCTINPUT_H
#define CXTESTSYNTHETICRECONSTRUCTINPUT_H

#include "cxtestutilities_export.h"
#include "cxSimpleSyntheticVolume.h"
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
class CXTESTUTILITIES_EXPORT SyntheticReconstructInput
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
