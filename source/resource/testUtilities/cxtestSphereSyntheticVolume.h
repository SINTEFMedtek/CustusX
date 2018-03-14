/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTESTSPHERESYNTETICVOLUME_H
#define CXTESTSPHERESYNTETICVOLUME_H

#include "cxtestutilities_export.h"
#include "cxSyntheticVolume.h"

namespace cxtest {

/**
 *
 *
 * \ingroup cx
 * \date 21.11.2013, 2013
 * \author christiana
 */
class CXTESTUTILITIES_EXPORT SphereSyntheticVolume : public cx::cxSyntheticVolume
{
public:
	SphereSyntheticVolume(cx::Vector3D bounds);
	void setSphere(const cx::Vector3D& center, double radius);

	virtual unsigned char evaluate(const cx::Vector3D& p) const;
	virtual void printInfo() const;

private:
	double mRadius;
	cx::Vector3D mCenter;

};

}

#endif // CXTESTSPHERESYNTETICVOLUME_H
