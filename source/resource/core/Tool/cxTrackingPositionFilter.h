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
