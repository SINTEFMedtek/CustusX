/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxToolImpl.h"

#include <vtkSTLReader.h>
#include <QDir>
#include <vtkConeSource.h>

#include "cxTypeConversions.h"
#include "cxLogger.h"

namespace cx
{

ToolImpl::ToolImpl(const QString& uid, const QString& name) :
	Tool(uid, name),
	mPositionHistory(new TimedTransformMap()),
	m_prMt(Transform3D::Identity()),
	mPolyData(NULL),
	mTooltipOffset(0)
{
}

ToolImpl::~ToolImpl()
{

}

std::set<Tool::Type> ToolImpl::getTypes() const
{
	ToolFileParser::ToolInternalStructurePtr toolStructure = this->getToolFileToolStructure();
	std::set<Type> retval;

	if (toolStructure->mIsReference)
		retval.insert(Tool::TOOL_REFERENCE);
	if (toolStructure->mIsPointer)
		retval.insert(Tool::TOOL_POINTER);
	if (toolStructure->mIsProbe)
		retval.insert(Tool::TOOL_US_PROBE);

	return retval;
}

ToolPositionMetadata ToolImpl::getMetadata() const
{
	if (mMetadata.empty())
		return ToolPositionMetadata();
	return mMetadata.rbegin()->second;
}

const std::map<double, ToolPositionMetadata>& ToolImpl::getMetadataHistory()
{
	return mMetadata;
}

double ToolImpl::getTooltipOffset() const
{
	return mTooltipOffset;
}

void ToolImpl::setTooltipOffset(double val)
{
	if (similar(val, mTooltipOffset))
		return;
	mTooltipOffset = val;
	emit tooltipOffset(mTooltipOffset);
}

TimedTransformMapPtr ToolImpl::getPositionHistory()
{
	return mPositionHistory;
}

TimedTransformMap ToolImpl::getSessionHistory(double startTime, double stopTime)
{
	TimedTransformMap::iterator startIt = mPositionHistory->lower_bound(startTime);
	TimedTransformMap::iterator stopIt = mPositionHistory->upper_bound(stopTime);

	TimedTransformMap retval(startIt, stopIt);
	return retval;
}

Transform3D ToolImpl::get_prMt() const
{
	return m_prMt;
}

void ToolImpl::set_prMt(const Transform3D& prMt, double timestamp)
{
	if (mPositionHistory->count(timestamp))
	{
		if (similar(mPositionHistory->find(timestamp)->second, prMt))
			return;
	}

	m_prMt = prMt;
	// Store positions in history, but only if visible - the history has no concept of visibility
	if (this->getVisible())
		(*mPositionHistory)[timestamp] = m_prMt;
	emit toolTransformAndTimestamp(m_prMt, timestamp);
}

void ToolImpl::resetTrackingPositionFilter(TrackingPositionFilterPtr filter)
{
    mTrackingPositionFilter = filter;
}

void ToolImpl::createToolGraphic()
{
	QString toolGraphicsFileName = this->getToolFileToolStructure()->mGraphicsFileName;
	QDir dir;
	if (!toolGraphicsFileName.isEmpty()
					&& dir.exists(toolGraphicsFileName))
	{
		vtkSTLReaderPtr reader = vtkSTLReaderPtr::New();
		reader->SetFileName(cstring_cast(toolGraphicsFileName));
		reader->Update();
		mPolyData = reader->GetOutput();
	}
	else
	{
		mPolyData = Tool::createDefaultPolyDataCone();
	}
}

vtkPolyDataPtr ToolImpl::getGraphicsPolyData() const
{
	return mPolyData;
}

bool ToolImpl::hasReferencePointWithId(int id)
{
	return this->getReferencePoints().count(id);
}

std::map<int, Vector3D> ToolImpl::getReferencePoints() const
{
	return getToolFileToolStructure()->mReferencePoints;
}

} // namespace cx

