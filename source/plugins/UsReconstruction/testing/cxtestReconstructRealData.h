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
#ifndef CXTESTRECONSTRUCTREALDATA_H
#define CXTESTRECONSTRUCTREALDATA_H

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
class ReconstructRealTestData
{
public:
	QString getSourceFilename() const;
	void validateData(cx::ImagePtr output);
	void validateAngioData(cx::ImagePtr angioOut);
	void validateBModeData(cx::ImagePtr bmodeOut);
private:
	int getValue(cx::ImagePtr data, int x, int y, int z);
};


} // namespace cxtest


#endif // CXTESTRECONSTRUCTREALDATA_H
