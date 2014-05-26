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
#ifndef CXTESTRECONSTRUCTIONMANAGERFIXTURE_H
#define CXTESTRECONSTRUCTIONMANAGERFIXTURE_H

#include "cxReconstructionManager.h"
#include "cxtestSyntheticVolumeComparer.h"
#include "cxtestSyntheticReconstructInput.h"

namespace cxtest
{

/** Unit tests that test the US reconstruction plugin
 *
 *
 * \ingroup cxtest
 * \date june 25, 2013
 * \author christiana
 */
class ReconstructionManagerTestFixture
{
public:
	ReconstructionManagerTestFixture();
	~ReconstructionManagerTestFixture();

	cx::ReconstructionManagerPtr getManager();
	void reconstruct();	///< run the reconstruction in the main thread
	void threadedReconstruct();
	std::vector<cx::ImagePtr> getOutput();
	SyntheticVolumeComparerPtr getComparerForOutput(SyntheticReconstructInputPtr input, int index);
	void setPNN_InterpolationSteps(int value);

	void setVerbose(bool val) { mVerbose = val; }
	bool getVerbose() const { return mVerbose; }

private:
	cx::ReconstructionManagerPtr mManager;
	std::vector<cx::ImagePtr> mOutput; // valid after (threaded)reconstruct() has been run
	bool mVerbose;
};



} // namespace cxtest


#endif // CXTESTRECONSTRUCTIONMANAGERFIXTURE_H
