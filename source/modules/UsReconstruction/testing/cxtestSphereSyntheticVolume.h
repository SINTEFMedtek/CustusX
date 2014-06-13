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
#ifndef CXTESTSPHERESYNTETICVOLUME_H
#define CXTESTSPHERESYNTETICVOLUME_H

#include "cxSyntheticVolume.h"

namespace cxtest {

/**
 *
 *
 * \ingroup cx
 * \date 21.11.2013, 2013
 * \author christiana
 */
class SphereSyntheticVolume : public cx::cxSyntheticVolume
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
