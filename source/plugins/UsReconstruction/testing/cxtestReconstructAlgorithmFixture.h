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
	void defineProbeMovementNormalizedTranslationRange(double range);
	void defineProbeMovementAngleRange(double range);
	void defineProbeMovementSteps(int steps);
	void defineProbe(cx::ProbeDefinition probe);
	void setOverallBoundsAndSpacing(double size, double spacing);

	void setBoxAndLinesPhantom();
	void setSpherePhantom();
	void setWireCrossPhantom();

	void reconstruct();

	void checkRMSBelow(double threshold);
	void checkCentroidDifferenceBelow(double val);
	void checkMassDifferenceBelow(double val);

	void saveNominalOutputToFile(QString filename);
	void saveOutputToFile(QString filename);

	void setVerbose(bool val) { mVerbose = val; }
	bool getVerbose() const { return mVerbose; }

	void generateSynthetic_USReconstructInputData();

private:
	std::vector<cx::Transform3D> generateFrames_rMt_tilted();
	void generateInput();
	void generateOutputVolume();
	void printConfiguration();
	double getRMS();
	cx::ImagePtr createOutputVolume(QString name);
	cx::ImagePtr getNominalOutputImage();

	/** Generate a sequence of planes using the input definition.
	  * The planes work around p0, applying translation and rotation
	  * simultaneously.
	  */
	std::vector<cx::Transform3D> generateFrames(cx::Vector3D p0,
												cx::Vector3D range_translation,
												double range_angle,
												cx::Vector3D rotation_axis,
												int steps);

	cx::ReconstructAlgorithmPtr mAlgorithm;
	cx::cxSyntheticVolumePtr mPhantom;
	cx::ProcessedUSInputDataPtr mInputData;
	cx::ImagePtr mOutputData;
	bool mVerbose;
//	cx::Transform3D m_dMr;

	// setup parameters
	cx::Vector3D mBounds;
	struct OutputVolumeType
	{
		cx::Vector3D mBounds;
		cx::Vector3D mSpacing;
	};
	OutputVolumeType mOutputVolumeDefinition;

	cx::ProbeDefinition mProbe;

	struct ProbeMovement
	{
		cx::Vector3D mRangeNormalizedTranslation;
		double mRangeAngle;
		double mSteps;
	};
	ProbeMovement mProbeMovementDefinition;

	// cached values
	cx::ImagePtr mNominalOutputImage;
};

} // namespace cxtest


#endif // CXTESTRECONSTRUCTALGORITHMFIXTURE_H
