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

	void saveNominalOutputToFile(QString filename, cx::FileManagerServicePtr port);
	void saveOutputToFile(QString filename, cx::FileManagerServicePtr port);

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
