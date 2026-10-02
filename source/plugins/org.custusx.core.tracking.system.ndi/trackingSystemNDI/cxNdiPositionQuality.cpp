/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxNdiPositionQuality.h"

#include <QStringList>

namespace cx
{

NdiPositionQuality::NdiPositionQuality(double maxErrorRMS_mm) :
	mMaxErrorRMS_mm(maxErrorRMS_mm),
	mExpectedMarkers(0),
	mBadFrames(0),
	mGoodFrames(0)
{
}

QString NdiPositionQuality::evaluate(const NdiToolSample& sample, double maxErrorRMS_mm, int expectedMarkers)
{
	QStringList problems;
	if (sample.portStatus & NdiPortStatus::OutOfVolume)
	{
		problems << "out of the measurement volume";
	}
	else if (sample.portStatus & NdiPortStatus::PartiallyInVolume)
	{
		problems << "partially out of the measurement volume";
	}
	if (sample.toolInfo & NdiToolInfo::BadTransformFit)
	{
		problems << "bad marker fit";
	}
	if (sample.toolInfo & NdiToolInfo::NotEnoughMarkers)
	{
		problems << "not enough markers";
	}
	if (sample.markersUsed >= 0 && expectedMarkers > 0 && sample.markersUsed < expectedMarkers)
	{
		problems << QString("%1 of %2 markers used").arg(sample.markersUsed).arg(expectedMarkers);
	}
	if (maxErrorRMS_mm > 0.0 && sample.errorRMS_mm > maxErrorRMS_mm)
	{
		problems << QString("RMS error %1 (limit %2)").arg(sample.errorRMS_mm, 0, 'f', 2).arg(maxErrorRMS_mm, 0, 'f', 2);
	}
	return problems.join(", ");
}

QString NdiPositionQuality::update(const NdiToolSample& sample)
{
	if (sample.visible)
	{
		mExpectedMarkers = std::max(mExpectedMarkers, sample.markersUsed);
		QString problems = evaluate(sample, mMaxErrorRMS_mm, mExpectedMarkers);
		if (!problems.isEmpty())
		{
			mGoodFrames = 0;
			mBadFrames++;
			if (mBadFrames >= kFramesBeforeWarning || !mWarning.isEmpty())
			{
				mWarning = problems;
			}
		}
		else
		{
			mBadFrames = 0;
			mGoodFrames++;
			if (mGoodFrames >= kFramesBeforeClearing)
			{
				mWarning.clear();
			}
		}
	}
	return mWarning;
}

QString NdiPositionQuality::getWarning() const
{
	return mWarning;
}

void NdiPositionQuality::reset()
{
	mBadFrames = 0;
	mGoodFrames = 0;
	mWarning.clear();
}

void NdiPositionQuality::setMaxErrorRMS(double maxErrorRMS_mm)
{
	mMaxErrorRMS_mm = maxErrorRMS_mm;
}

int NdiPositionQuality::getExpectedMarkers() const
{
	return mExpectedMarkers;
}

} // namespace cx
