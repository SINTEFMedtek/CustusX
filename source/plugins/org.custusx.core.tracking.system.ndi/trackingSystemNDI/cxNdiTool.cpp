/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxNdiTool.h"

#include <QtGlobal>

namespace cx
{

NdiTool::NdiTool(ToolFileParser::ToolInternalStructurePtr internalStructure) :
	mInternalStructure(internalStructure),
	mPortHandle(-1),
	mValid(false),
	mVisible(false),
	mInitialized(false),
	mMissedFrames(0)
{
	qRegisterMetaType<Transform3D>("Transform3D");
	qRegisterMetaType<ToolPositionMetadata>("ToolPositionMetadata");

	if (mInternalStructure)
		mValid = mInternalStructure->verify();
}

NdiTool::~NdiTool()
{
}

ToolFileParser::ToolInternalStructurePtr NdiTool::getInternalStructure() const
{
	return mInternalStructure;
}

QString NdiTool::getUid() const
{
	if (mInternalStructure)
		return mInternalStructure->mUid;
	return "";
}

bool NdiTool::isValid() const
{
	return mValid;
}

bool NdiTool::isVisible() const
{
	return mVisible;
}

bool NdiTool::isInitialized() const
{
	return mInitialized;
}

bool NdiTool::isReference() const
{
	return mInternalStructure && mInternalStructure->mIsReference;
}

int NdiTool::getPortHandle() const
{
	return mPortHandle;
}

void NdiTool::setPortHandle(int handle)
{
	mPortHandle = handle;
}

void NdiTool::updateTransform(const Transform3D& transform, double timestamp, bool visible, double errorRMS_mm)
{
	if (!visible)
		mMissedFrames++;
	else
		mMissedFrames = 0;

	// Tolerate brief outages: stay visible if within tolerance, hiding the toggle from the UI
	bool reportVisible = visible || (mVisible && mMissedFrames <= kMissTolerance);

	if (reportVisible != mVisible)
	{
		mVisible = reportVisible;
		emit toolVisible(mVisible);
	}

	if (mVisible && visible)
	{
		ToolPositionMetadata metadata;
		metadata.mData = QString("errorRMS_mm:%1").arg(errorRMS_mm);
		emit toolTransformAndTimestamp(transform, timestamp, metadata);
	}
}

void NdiTool::setInitialized(bool on)
{
	mInitialized = on;
}

void NdiTool::setInvisible()
{
	mMissedFrames = kMissTolerance + 1;
	if (mVisible)
	{
		mVisible = false;
		emit toolVisible(false);
	}
}

} // namespace cx
