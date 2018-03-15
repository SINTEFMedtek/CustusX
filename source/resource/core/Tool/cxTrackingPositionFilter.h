/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTRACKINGPOSITIONFILTER_H
#define CXTRACKINGPOSITIONFILTER_H

#include "cxResourceExport.h"

#include "cxTransform3D.h"
#include <map>
#include <boost/shared_ptr.hpp>
#include "iir/Butterworth.h"

namespace cx
{

/** Applies a smoothing filter to tracking positions.
 *
 *
 * \ingroup cx_resource_core_tool
 * \date 2014-03-06
 * \author christiana
 */
class cxResource_EXPORT TrackingPositionFilter
{
public:
	TrackingPositionFilter();
	void setCutOffFrequency(double freq);
	void addPosition(Transform3D pos, double timestamp);
	Transform3D getFilteredPosition();	

private:
	std::map<double, Transform3D> mHistory;
	std::map<double, Transform3D> mResampled;
	std::map<double, Transform3D> mFiltered;
	void clearIfTimestampIsOlderThanHead(Transform3D pos, double timestamp);
	void clearIfJumpInTimestamps(Transform3D pos, double timestamp);
	void interpolateAndFilterPositions(Transform3D pos, double timestamp);
	void reset();
	float mCutOffFrequency;
	float mResampleFrequency;
	static const int mFilterOrder = 2;
	Iir::Butterworth::LowPass<mFilterOrder> fx;
	Iir::Butterworth::LowPass<mFilterOrder> fy;
	Iir::Butterworth::LowPass<mFilterOrder> fz;
};
typedef boost::shared_ptr<TrackingPositionFilter> TrackingPositionFilterPtr;

} // namespace cx


#endif // CXTRACKINGPOSITIONFILTER_H
