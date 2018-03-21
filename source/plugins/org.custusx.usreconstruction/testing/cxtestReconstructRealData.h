/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTESTRECONSTRUCTREALDATA_H
#define CXTESTRECONSTRUCTREALDATA_H

#include "cxtest_org_custusx_usreconstruction_export.h"

#include "cxImage.h"

namespace cxtest
{

/** One data set from a lab US acquisition,
 *  along with methods to test if a reconstructed volume
 *  based on these data are correct.
 *
 * \ingroup cx
 * \date 12.12.2013, 2013
 * \author christiana
 */
class CXTEST_ORG_CUSTUSX_USRECONSTRUCTION_EXPORT ReconstructRealTestData
{
public:
	QString getSourceFilename() const;
	void validateData(cx::ImagePtr output);
	void validateAngioData(cx::ImagePtr angioOut);
	void validateBModeData(cx::ImagePtr bmodeOut);
private:
	std::vector<int> getCrossVolumeSamples();
	std::vector<int> getLightVolumeSamples();
	std::vector<int> getDarkVolumeSamples();
	void checkSamples(cx::ImagePtr image, std::vector<int> samples, int lowerThreshold, bool matchThresholdExactly = false);
	int getValue(cx::ImagePtr data, int x, int y, int z);
};


} // namespace cxtest


#endif // CXTESTRECONSTRUCTREALDATA_H
