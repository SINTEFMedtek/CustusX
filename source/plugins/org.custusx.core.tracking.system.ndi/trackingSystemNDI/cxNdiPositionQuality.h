/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNDIPOSITIONQUALITY_H_
#define CXNDIPOSITIONQUALITY_H_

#include "org_custusx_core_tracking_system_ndi_Export.h"

#include <QString>
#include "cxNdiDevice.h"

namespace cx
{

/** \brief Decides whether a tool position may be inaccurate, from the BX status of each sample.
 *
 * Warns when the tool is (partially) out of the measurement volume, when the RMS error is
 * above a limit, or, for Polaris, when the fit is bad or fewer markers than usual were used
 * (a hidden or partly hidden sphere). A warning is set after a few bad frames in a row and
 * cleared after a few good frames, so that it doesn't flicker.
 *
 * \ingroup org_custusx_core_tracking_system_ndi
 */
class org_custusx_core_tracking_system_ndi_EXPORT NdiPositionQuality
{
public:
	static constexpr double kDefaultMaxErrorRMSPolaris_mm = 0.5;
	static constexpr double kDefaultMaxErrorRMSAurora_mm = 1.0;
	static const int kFramesBeforeWarning = 3;
	static const int kFramesBeforeClearing = 10;

	explicit NdiPositionQuality(double maxErrorRMS_mm = kDefaultMaxErrorRMSPolaris_mm);

	/** The problems with one sample, empty if it is OK. expectedMarkers <= 0: don't check markers. */
	static QString evaluate(const NdiToolSample& sample, double maxErrorRMS_mm, int expectedMarkers);

	QString update(const NdiToolSample& sample); ///< Add a sample, return the current warning. Invisible samples are ignored.
	QString getWarning() const;
	void reset(); ///< Clear the warning, e.g. when tracking stops
	void setMaxErrorRMS(double maxErrorRMS_mm);
	int getExpectedMarkers() const; ///< The most markers used in any sample so far

private:
	double mMaxErrorRMS_mm;
	int mExpectedMarkers;
	int mBadFrames;
	int mGoodFrames;
	QString mWarning;
};

} // namespace cx

#endif /* CXNDIPOSITIONQUALITY_H_ */
