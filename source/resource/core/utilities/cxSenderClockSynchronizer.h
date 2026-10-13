/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSENDERCLOCKSYNCHRONIZER_H_
#define CXSENDERCLOCKSYNCHRONIZER_H_

#include "cxResourceExport.h"
#include <deque>

namespace cx
{

/** \brief Converts timestamps from a sender's clock to this computer's clock.
 *
 * The offset is the smallest receive minus sender time in a sliding window. Uses the receive
 * time while the sender time is not increasing or is late, and synchronizes again if that lasts.
 * A sender time of 0 means no timestamp: the receive time is used.
 *
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT SenderClockSynchronizer
{
public:
	static constexpr double kWindow_ms = 10000.0;
	static constexpr double kResyncLimit_ms = 1000.0;
	static constexpr double kResyncAfter_ms = 2000.0;

	SenderClockSynchronizer();
	void reset();
	double getTimestamp(double sent_ms, double received_ms); ///< \return the time on this computer, never later than received_ms
	bool hasOffset() const;
	double getOffset_ms() const; ///< received - sent, for the message with the shortest delay in the window
	bool usedReceiveTime() const; ///< The last timestamp is the receive time, because the sender time could not be used
	int getResyncCount() const;

private:
	struct Difference
	{
		double received_ms;
		double difference_ms;
	};
	double getSynchronizedTimestamp(double sent_ms, double received_ms);
	bool isLate(double sent_ms, double received_ms) const;
	void addDifference(double received_ms, double difference_ms);

	std::deque<Difference> mDifferences; ///< increasing differences: the front is the smallest in the window
	double mLastSent_ms;
	double mLateSince_ms;
	bool mUsedReceiveTime;
	int mResyncCount;
};

} // namespace cx

#endif /* CXSENDERCLOCKSYNCHRONIZER_H_ */
