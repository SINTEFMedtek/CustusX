/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	void createToolGraphic();

	TimedTransformMapPtr mPositionHistory;
	Transform3D m_prMt; ///< the transform from the tool to the patient reference
	TrackingPositionFilterPtr mTrackingPositionFilter;
	std::map<double, ToolPositionMetadata> mMetadata;
	vtkPolyDataPtr mPolyData; ///< the polydata used to represent the tool graphically

	virtual std::set<Type> getTypes() const;
	virtual std::map<int, Vector3D> getReferencePoints() const;
	virtual ToolFileParser::ToolInternalStructurePtr getToolFileToolStructure() const { return ToolFileParser::ToolInternalStructurePtr(); }
private:
	double mTooltipOffset;
};
typedef boost::shared_ptr<ToolImpl> cxToolPtr;

} // namespace cx

#endif // CXTOOLIMPL_H
