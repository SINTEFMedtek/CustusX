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
#ifndef CXTESTRECONSTRUCTIONMANAGERFIXTURE_H
#define CXTESTRECONSTRUCTIONMANAGERFIXTURE_H

#include "cxtest_org_custusx_usreconstruction_export.h"

#include "cxtestSyntheticVolumeComparer.h"
#include "cxtestSyntheticReconstructInput.h"
#include "cxUsReconstructionService.h"

namespace cxtest
{

/** Unit tests that test the US reconstruction plugin
 *
 *
 * \ingroup cxtest
 * \date june 25, 2013
 * \author christiana
 */
class CXTEST_ORG_CUSTUSX_USRECONSTRUCTION_EXPORT ReconstructionManagerTestFixture
{
public:
	ReconstructionManagerTestFixture();
	~ReconstructionManagerTestFixture();

	cx::UsReconstructionServicePtr getManager();
	void reconstruct();	///< run the reconstruction in the main thread
	void threadedReconstruct();
	std::vector<cx::ImagePtr> getOutput();
	SyntheticVolumeComparerPtr getComparerForOutput(SyntheticReconstructInputPtr input, int index);
	void setPNN_InterpolationSteps(int value);

	void setVerbose(bool val) { mVerbose = val; }
	bool getVerbose() const { return mVerbose; }

private:
	cx::UsReconstructionServicePtr mManager;
	std::vector<cx::ImagePtr> mOutput; // valid after (threaded)reconstruct() has been run
	bool mVerbose;
	cx::PatientModelServicePtr mPatientModelService;
	cx::ViewServicePtr mViewService;
};



} // namespace cxtest


#endif // CXTESTRECONSTRUCTIONMANAGERFIXTURE_H
