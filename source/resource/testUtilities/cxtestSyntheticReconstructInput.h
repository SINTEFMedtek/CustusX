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
