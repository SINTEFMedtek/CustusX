/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

	cx::FileManagerServicePtr getFileManagerService();

private:
	cx::UsReconstructionServicePtr mManager;
	std::vector<cx::ImagePtr> mOutput; // valid after (threaded)reconstruct() has been run
	bool mVerbose;
	cx::PatientModelServicePtr mPatientModelService;
	cx::ViewServicePtr mViewService;
	cx::FileManagerServicePtr mFileManagerService;
};



} // namespace cxtest


#endif // CXTESTRECONSTRUCTIONMANAGERFIXTURE_H
