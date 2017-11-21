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

std::set<Tool::Type> ToolImpl::getTypesFromToolStructure(ToolFileParser::ToolInternalStructurePtr toolStructure) const
{
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

void ToolImpl::createToolGraphic(QString toolGraphicsFileName)
{
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
} // namespace cx

