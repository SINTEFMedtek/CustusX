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

#include "cxReconstructAlgorithm.h"
#include "TordReconstruct/cxSimpleSyntheticVolume.h"
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
class ReconstructAlgorithmFixture
{
public:
	ReconstructAlgorithmFixture();

	void setAlgorithm(cx::ReconstructAlgorithmPtr algorithm);
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

	cx::ReconstructAlgorithmPtr mAlgorithm;
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


#endif // CXTESTRECONSTRUCTALGORITHMFIXTURE_H
