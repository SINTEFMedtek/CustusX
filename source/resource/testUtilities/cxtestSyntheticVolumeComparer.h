/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTESTSYNTHETICVOLUMECOMPARER_H
#define CXTESTSYNTHETICVOLUMECOMPARER_H

#include "cxtestutilities_export.h"
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
class CXTESTUTILITIES_EXPORT SyntheticVolumeComparer
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
