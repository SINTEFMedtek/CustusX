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

#ifndef CXTESTRECONSTRUCTIONALGORITHMFIXTURE_H
#define CXTESTRECONSTRUCTIONALGORITHMFIXTURE_H

#include "cxtest_org_custusx_usreconstruction_export.h"

#include "cxReconstructionMethodService.h"
#include "cxtestSyntheticVolumeComparer.h"
#include "cxtestSyntheticReconstructInput.h"

namespace cxtest
{

/**
 * \ingroup cx
 * \date 21.11.2013
 * \author christiana
 *
 * Definitions:
 *   - Phantom/SyntheticVolume is defined by a [0,bounds] in space r
 *   - Output volume is defined in space d, parameters (dims,spacing,dMr).
 *   - Plane transforms are dMf...??? equals transforms in reconstructinputdata
 *          i.e from input frame plane to ref space.
 *          dMf = dMr*rMf, where rMf is the plane position on ref space.
 *
 */
class CXTEST_ORG_CUSTUSX_USRECONSTRUCTION_EXPORT ReconstructionAlgorithmFixture
{
public:
	ReconstructionAlgorithmFixture();

	void setAlgorithm(cx::ReconstructionMethodServicePtr algorithm);
	void defineOutputVolume(double bounds, double spacing);
	void setOverallBoundsAndSpacing(double size, double spacing);

	void reconstruct(QDomElement root);

	void checkRMSBelow(double threshold);
	void checkCentroidDifferenceBelow(double val);
	void checkMassDifferenceBelow(double val);

	void saveNominalOutputToFile(QString filename);
	void saveOutputToFile(QString filename);

	void setVerbose(bool val) { mVerbose = val; }
	bool getVerbose() const { return mVerbose; }
	cx::cxSyntheticVolumePtr getPhantom() { return mInputGenerator->getPhantom(); }

	SyntheticReconstructInputPtr getInputGenerator() { return mInputGenerator; }

private:
	void generateInput();
	void generateOutputVolume();
	void printConfiguration();
	cx::ImagePtr createOutputVolume(QString name);

	cx::ReconstructionMethodServicePtr mAlgorithm;
	cx::ProcessedUSInputDataPtr mInputData;
	cx::ImagePtr mOutputData;
	bool mVerbose;

	// setup parameters
	struct OutputVolumeType
	{
		cx::Vector3D mBounds;
		cx::Vector3D mSpacing;
	};
	OutputVolumeType mOutputVolumeDefinition;

	// cached values
	SyntheticVolumeComparerPtr getComparer();
	SyntheticVolumeComparerPtr mComparer;
	SyntheticReconstructInputPtr mInputGenerator;
};

} // namespace cxtest


#endif // CXTESTRECONSTRUCTIONALGORITHMFIXTURE_H
