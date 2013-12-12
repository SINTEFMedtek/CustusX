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
#ifndef CXTESTRECONSTRUCTMANAGERFIXTURE_H
#define CXTESTRECONSTRUCTMANAGERFIXTURE_H

#include <QApplication>
#include <vtkImageData.h>
#include "sscReconstructManager.h"
#include "sscImage.h"
#include "sscPNNReconstructAlgorithm.h"
#include "cxDataLocations.h"
#include "cxDataManager.h"
#include "sscReconstructPreprocessor.h"
#include "sscReconstructParams.h"
#include "cxTimedAlgorithm.h"
#include "cxUSReconstructInputDataAlgoritms.h"
#include "sscReconstructManager.h"
#include "sscDataAdapter.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include <sscBoolDataAdapterXml.h>


#include "recConfig.h"
#ifdef SSC_USE_OpenCL
	#include "TordReconstruct/TordTest.h"
  #include "TordReconstruct/cxSimpleSyntheticVolume.h"
#endif // SSC_USE_OpenCL


#include "catch.hpp"

#include "cxtestUtilities.h"
#include "sscUSFrameData.h"
#include "sscDummyTool.h"
#include "cxImageDataContainer.h"
#include "cxUsReconstructionFileMaker.h"

#include "cxtestSphereSyntheticVolume.h"
#include "cxtestReconstructAlgorithmFixture.h"

namespace cxtest
{

/** Unit tests that test the US reconstruction plugin
 *
 *
 * \ingroup cxtest
 * \date june 25, 2013
 * \author christiana
 */
class ReconstructManagerTestFixture
{
public:
	ReconstructManagerTestFixture();
	~ReconstructManagerTestFixture();

	cx::ReconstructManagerPtr getManager();
	void reconstruct();	///< run the reconstruction in the main thread
	void threadedReconstruct();
	std::vector<cx::ImagePtr> getOutput();
	SyntheticVolumeComparerPtr getComparerForOutput(ReconstructAlgorithmFixture& algoFixture, int index);
	void setPNN_InterpolationSteps(int value);

private:
	cx::ReconstructManagerPtr mManager;
	std::vector<cx::ImagePtr> mOutput; // valid after (threaded)reconstruct() has been run
};



} // namespace cxtest


#endif // CXTESTRECONSTRUCTMANAGERFIXTURE_H
