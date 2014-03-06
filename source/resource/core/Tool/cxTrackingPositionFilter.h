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
#ifndef CXTRACKINGPOSITIONFILTER_H
#define CXTRACKINGPOSITIONFILTER_H

#include "sscTransform3D.h"
#include <map>
#include <boost/shared_ptr.hpp>

namespace cx
{

/** Applies a smoothing filter to tracking positions.
 *
 *
 * \ingroup cxTool
 * \date 2014-03-06
 * \author christiana
 */
class TrackingPositionFilter
{
public:
	TrackingPositionFilter();
	void addPosition(Transform3D pos, double timestamp);
	Transform3D getFilteredPosition();

private:
	std::map<double, Transform3D> mHistory;
	void clearIfTimestampIsOlderThanHead(double timestamp);
};
typedef boost::shared_ptr<TrackingPositionFilter> TrackingPositionFilterPtr;

} // namespace cx


#endif // CXTRACKINGPOSITIONFILTER_H
