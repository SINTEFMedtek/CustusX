/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPositionQuality.h"

#include <QFile>

#include <QStringList>

namespace cx
{

PositionQuality::PositionQuality(double maxErrorRMS_mm) :
	mMaxErrorRMS_mm(maxErrorRMS_mm),
	mExpectedMarkers(0),
	mBadFrames(0),
	mGoodFrames(0)
{
}

QString PositionQuality::evaluate(const PositionQualitySample& sample, double maxErrorRMS_mm, int expectedMarkers)
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

QString PositionQuality::update(const PositionQualitySample& sample)
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

QString PositionQuality::getWarning() const
{
	return mWarning;
}

void PositionQuality::reset()
{
	mBadFrames = 0;
	mGoodFrames = 0;
	mWarning.clear();
}

void PositionQuality::setMaxErrorRMS(double maxErrorRMS_mm)
{
	mMaxErrorRMS_mm = maxErrorRMS_mm;
}

void PositionQuality::setMarkerCount(int markers)
{
	mExpectedMarkers = std::max(mExpectedMarkers, markers);
}

int PositionQuality::readMarkerCount(QString romFile)
{
	// An NDI tool definition starts with "NDI", and has the number of markers as a 32 bit integer at byte 28
	int retval = -1;
	QFile file(romFile);
	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray data = file.read(32);
		if (data.size() == 32 && data.startsWith("NDI"))
		{
			const unsigned char* bytes = reinterpret_cast<const unsigned char*>(data.constData()) + 28;
			retval = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
		}
	}
	return retval;
}

double PositionQuality::getDefaultMaxErrorRMS(TRACKING_SYSTEM trackerType)
{
	bool aurora = (trackerType == tsAURORA);
	return aurora ? kDefaultMaxErrorRMSAurora_mm : kDefaultMaxErrorRMSPolaris_mm;
}

QString PositionQuality::combineWarnings(QString toolWarning, QString referenceWarning)
{
	QString retval = toolWarning;
	if (!referenceWarning.isEmpty())
	{
		retval = toolWarning.isEmpty() ? "Reference: " + referenceWarning : toolWarning + "; reference: " + referenceWarning;
	}
	return retval;
}

int PositionQuality::getExpectedMarkers() const
{
	return mExpectedMarkers;
}

} // namespace cx
