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
#ifndef CXTOOLIMPL_H
#define CXTOOLIMPL_H

#include "cxResourceExport.h"

#include "cxTool.h"
#include "cxToolFileParser.h"

namespace cx
{
typedef boost::shared_ptr<class TrackingPositionFilter> TrackingPositionFilterPtr;

/** \brief Common functionality for Tool subclasses
 *
 *
 * \ingroup cx_resource_core_tool
 * \date 2014-02-21
 * \author christiana
 */
class cxResource_EXPORT ToolImpl : public Tool
{
	Q_OBJECT
public:
	explicit ToolImpl(const QString& uid="", const QString& name ="");
	virtual ~ToolImpl();

	virtual TimedTransformMapPtr getPositionHistory();
	virtual TimedTransformMap getSessionHistory(double startTime, double stopTime);
	virtual Transform3D get_prMt() const;

	virtual ToolPositionMetadata getMetadata() const;
	virtual const std::map<double, ToolPositionMetadata>& getMetadataHistory();

	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);

	virtual void resetTrackingPositionFilter(TrackingPositionFilterPtr filter);
	virtual bool isNull() { return false; }

	virtual void addXml(QDomNode& dataNode) {Q_UNUSED(dataNode)}
	virtual void parseXml(QDomNode& dataNode) {Q_UNUSED(dataNode)}

	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual bool hasReferencePointWithId(int id);
protected:
	virtual void set_prMt(const Transform3D& prMt, double timestamp);
	void createToolGraphic(QString toolGraphicsFileName);

	TimedTransformMapPtr mPositionHistory;
	Transform3D m_prMt; ///< the transform from the tool to the patient reference
	TrackingPositionFilterPtr mTrackingPositionFilter;
	std::map<double, ToolPositionMetadata> mMetadata;
	vtkPolyDataPtr mPolyData; ///< the polydata used to represent the tool graphically

	std::set<Tool::Type> getTypesFromToolStructure(ToolFileParser::ToolInternalStructurePtr toolStructure) const;
private:
	double mTooltipOffset;
};
typedef boost::shared_ptr<ToolImpl> cxToolPtr;

} // namespace cx

#endif // CXTOOLIMPL_H
