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

#ifndef CXTESTRECONSTRUCTALGORITHMFIXTURE_H
#define CXTESTRECONSTRUCTALGORITHMFIXTURE_H

#include "sscReconstructAlgorithm.h"
#include "TordReconstruct/cxSimpleSyntheticVolume.h"

namespace cx
{
typedef boost::shared_ptr<class cxSyntheticVolume> cxSyntheticVolumePtr;
}

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
class ReconstructAlgorithmFixture
{
public:
	void setAlgorithm(cx::ReconstructAlgorithm* algorithm);
	void setBoxAndLinesPhantom();
	void setWireCrossPhantom();
	std::vector<cx::Transform3D> generateFrames_rMf_tilted();
	void generateInput();
	void generateOutputVolume();
	void reconstruct();
	void checkRMSBelow(double threshold);
	double getRMS();

private:
	cx::ReconstructAlgorithm* mAlgorithm;
	cx::cxSyntheticVolumePtr mPhantom;
	cx::ProcessedUSInputDataPtr mInputData;
	vtkImageDataPtr mOutputData;
	cx::Transform3D m_dMr;
};

} // namespace cxtest


#endif // CXTESTRECONSTRUCTALGORITHMFIXTURE_H
