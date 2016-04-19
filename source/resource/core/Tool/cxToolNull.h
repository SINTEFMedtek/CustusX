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
#ifndef CXTOOLNULL_H
#define CXTOOLNULL_H

#include "cxTool.h"

namespace cx
{

/** Null implementation of Tool
 *
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT ToolNull: public Tool
{
public:
	virtual ~ToolNull() {}

	virtual std::set<Type> getTypes() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual TimedTransformMapPtr getPositionHistory();
	virtual ToolPositionMetadata getMetadata() const;
	virtual const std::map<double, ToolPositionMetadata>& getMetadataHistory();

	virtual bool getVisible() const;
	virtual bool isInitialized() const;

	virtual QString getUid() const;
	virtual QString getName() const;

	virtual bool isCalibrated() const;
	virtual Transform3D getCalibration_sMt() const;
	virtual void setCalibration_sMt(Transform3D calibration);

	virtual ProbePtr getProbe() const;
	virtual double getTimestamp() const;
	virtual void printSelf(std::ostream &os, Indent indent);

	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);
	virtual std::map<int, Vector3D> getReferencePoints() const;
	virtual bool hasReferencePointWithId(int id);

	virtual TimedTransformMap getSessionHistory(double startTime, double stopTime);
	virtual Transform3D get_prMt() const;

	virtual void resetTrackingPositionFilter(TrackingPositionFilterPtr filter);

	virtual void set_prMt(const Transform3D& prMt, double timestamp);
	virtual void setVisible(bool vis);

	virtual bool isNull();
	static ToolPtr getNullObject();

private:
	std::map<double, ToolPositionMetadata> mMetadata;
};

} // namespace cx

#endif // CXTOOLNULL_H
