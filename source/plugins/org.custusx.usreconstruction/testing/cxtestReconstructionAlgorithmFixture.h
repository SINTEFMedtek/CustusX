/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

	void setAlgorithm(cx::ReconstructionMethodService* algorithm);
	void defineOutputVolume(double bounds, double spacing);
	void setOverallBoundsAndSpacing(double size, double spacing);

	void reconstruct(QDomElement root);

	void checkRMSBelow(double threshold);
	void checkCentroidDifferenceBelow(double val);
	void checkMassDifferenceBelow(double val);

	void saveNominalOutputToFile(QString filename, cx::FileManagerServicePtr filemanager);
	void saveOutputToFile(QString filename, cx::FileManagerServicePtr filemanager);

	void setVerbose(bool val)
	{
		mVerbose = val;
	}
	bool getVerbose() const
	{
		return mVerbose;
	}
	cx::cxSyntheticVolumePtr getPhantom()
	{
		return mInputGenerator->getPhantom();
	}

	SyntheticReconstructInputPtr getInputGenerator()
	{
		return mInputGenerator;
	}

private:
	void generateInput();
	void generateOutputVolume();
	void printConfiguration();
	cx::ImagePtr createOutputVolume(QString name);

	cx::ReconstructionMethodService* mAlgorithm;
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
