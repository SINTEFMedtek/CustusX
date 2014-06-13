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
#ifndef CXTESTSYNTHETICVOLUMECOMPARER_H
#define CXTESTSYNTHETICVOLUMECOMPARER_H

#include "cxSimpleSyntheticVolume.h"

namespace cxtest
{
typedef boost::shared_ptr<class SyntheticVolumeComparer> SyntheticVolumeComparerPtr;

/** Compares nominal and real versions of a test volume.
 *
 *  The input Test Image contains real data based on the phantom.
 *  The check methods compares this image to the corresponding
 *  nominal values in the phantom.
 *
 *  This class calls catch test functions.
 *
 * \ingroup cx
 * \date 28.11.2013
 * \author christiana
 */
class SyntheticVolumeComparer
{
public:
	SyntheticVolumeComparer();
	void setPhantom(cx::cxSyntheticVolumePtr phantom);
	void setTestImage(cx::ImagePtr image);

	void checkRMSBelow(double threshold);
	void checkCentroidDifferenceBelow(double val);
	void checkMassDifferenceBelow(double val);
	void checkValueWithin(cx::Vector3D p_r, int lowerLimit, int upperLimit);

	void saveNominalOutputToFile(QString filename);
	void saveOutputToFile(QString filename);

	void setVerbose(bool val) { mVerbose = val; }
	bool getVerbose() const { return mVerbose; }

private:
	double getValue(cx::ImagePtr image, cx::Vector3D p_r);
	cx::ImagePtr getNominalOutputImage() const;
	QString addFullPath(QString filename);
	double getRMS() const;

	cx::cxSyntheticVolumePtr mPhantom;
	cx::ImagePtr mTestImage;
	mutable cx::ImagePtr mNominalImage;
	bool mVerbose;
};


} // namespace cxtest



#endif // CXTESTSYNTHETICVOLUMECOMPARER_H
