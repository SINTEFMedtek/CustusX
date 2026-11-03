/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPOSITIONQUALITY_H_
#define CXPOSITIONQUALITY_H_

#include "cxResourceExport.h"

#include <QString>
#include "cxDefinitions.h"

namespace cx
{

/** NDI BX port status bits */
namespace NdiPortStatus
{
const int OutOfVolume = 0x40;
const int PartiallyInVolume = 0x80;
}

/** NDI BX tool information bits (Polaris) */
namespace NdiToolInfo
{
const int BadTransformFit = 0x01;
const int NotEnoughMarkers = 0x02;
}

/** The quality information of one tool position from an NDI tracker. */
struct cxResource_EXPORT PositionQualitySample
{
	bool visible = false;          ///< false if the transform is missing (out of view) or the port is disabled
	double errorRMS_mm = 0.0;
	int portStatus = 0;  ///< BX port status bits, see NdiPortStatus
	int toolInfo = 0;    ///< BX tool information bits, see NdiToolInfo. Polaris only.
	int markersUsed = -1; ///< Markers used to compute the transform, -1 if unknown. Polaris only.
};

/** \brief Decides whether a tool position from an NDI tracker may be inaccurate.
 *
 * Warns when the tool is (partially) out of the measurement volume, when the RMS error is
 * above a limit, or, for Polaris, when the fit is bad or fewer markers than the tool has were used
 * (a hidden or partly hidden sphere). A warning is set after a few bad frames in a row and
 * cleared after a few good frames, so that it doesn't flicker.
 *
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT PositionQuality
{
public:
	static constexpr double kDefaultMaxErrorRMSPolaris_mm = 0.5;
	static constexpr double kDefaultMaxErrorRMSAurora_mm = 1.0;
	static const int kFramesBeforeWarning = 3;
	static const int kFramesBeforeClearing = 10;

	explicit PositionQuality(double maxErrorRMS_mm = kDefaultMaxErrorRMSPolaris_mm);

	/** The problems with one sample, empty if it is OK. expectedMarkers <= 0: don't check markers. */
	static QString evaluate(const PositionQualitySample& sample, double maxErrorRMS_mm, int expectedMarkers);
	static double getDefaultMaxErrorRMS(TRACKING_SYSTEM trackerType);

	QString update(const PositionQualitySample& sample); ///< Add a sample, return the current warning. Invisible samples are ignored.
	QString getWarning() const;
	void reset(); ///< Clear the warning, e.g. when tracking stops
	void setMaxErrorRMS(double maxErrorRMS_mm);
	int getExpectedMarkers() const; ///< The tool's number of markers, or the most markers used in any sample so far
	/** The tool's own number of markers: fewer is a warning from the first sample. For a tool with markers
	 *  on several faces this is the total, so it always gets a warning ("4 of 8 markers used"). */
	void setMarkerCount(int markers);
	static int readMarkerCount(QString romFile); ///< The number of markers in an NDI tool definition (.rom) file, -1 if it can't be read

private:
	double mMaxErrorRMS_mm;
	int mExpectedMarkers;
	int mBadFrames;
	int mGoodFrames;
	QString mWarning;
};

} // namespace cx

#endif /* CXPOSITIONQUALITY_H_ */
