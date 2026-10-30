/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <QDateTime>
#include <cmath>
#include "cxIGTLinkClientStreamer.h"
#include "cxImage.h"
#include "cxtestUtilities.h"

namespace cxtest
{

namespace
{
class TestIGTLinkClientStreamer : public cx::IGTLinkClientStreamer
{
public:
	using cx::IGTLinkClientStreamer::synchronizeTimestamp;
};

qint64 getSynchronizedTimestamp(TestIGTLinkClientStreamer& streamer, QDateTime sent)
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage();
	image->setAcquisitionTime(sent);
	streamer.synchronizeTimestamp(image);
	return image->getAcquisitionTime().toMSecsSinceEpoch();
}

qint64 getSynchronizedTimestamp(QString address, QDateTime sent)
{
	TestIGTLinkClientStreamer streamer;
	streamer.setAddress(address, 18944);
	return getSynchronizedTimestamp(streamer, sent);
}
} // namespace

TEST_CASE("IGTLinkClientStreamer: a sender on this computer uses the same clock", "[unit][plugins][org.custusx.core.video]")
{
	CHECK(cx::IGTLinkClientStreamer::isOnThisComputer("localhost"));
	CHECK(cx::IGTLinkClientStreamer::isOnThisComputer("Localhost"));
	CHECK(cx::IGTLinkClientStreamer::isOnThisComputer("127.0.0.1"));
	CHECK(cx::IGTLinkClientStreamer::isOnThisComputer("::1"));
	CHECK_FALSE(cx::IGTLinkClientStreamer::isOnThisComputer("192.0.2.1"));
	CHECK_FALSE(cx::IGTLinkClientStreamer::isOnThisComputer("scanner"));
}

TEST_CASE("IGTLinkClientStreamer: keeps the sent time from this computer, and synchronizes other senders", "[unit][plugins][org.custusx.core.video]")
{
	QDateTime sent = QDateTime::currentDateTime().addMSecs(-50);
	CHECK(getSynchronizedTimestamp("localhost", sent) == sent.toMSecsSinceEpoch());

	// the first image from another computer gets the receive time
	qint64 remoteDelay = getSynchronizedTimestamp("192.0.2.1", sent) - sent.toMSecsSinceEpoch();
	CHECK(remoteDelay >= 40);
}

TEST_CASE("IGTLinkClientStreamer: a local sent time that doesn't fit this computer's clock is synchronized", "[unit][plugins][org.custusx.core.video]")
{
	QDateTime now = QDateTime::currentDateTime();
	// e.g. a tunnel to a scanner with its clock behind or ahead
	qint64 lateDelay = now.toMSecsSinceEpoch() - getSynchronizedTimestamp("localhost", now.addMSecs(-300));
	CHECK(std::abs(lateDelay) < 100);
	qint64 aheadDelay = now.toMSecsSinceEpoch() - getSynchronizedTimestamp("localhost", now.addSecs(5));
	CHECK(std::abs(aheadDelay) < 100);
}

TEST_CASE("IGTLinkClientStreamer: a repeated local frame gets the receive time", "[unit][plugins][org.custusx.core.video]")
{
	TestIGTLinkClientStreamer streamer;
	streamer.setAddress("localhost", 18944);
	QDateTime sent = QDateTime::currentDateTime().addMSecs(-50);
	CHECK(getSynchronizedTimestamp(streamer, sent) == sent.toMSecsSinceEpoch());
	qint64 repeatedDelay = getSynchronizedTimestamp(streamer, sent) - sent.toMSecsSinceEpoch(); // e.g. a resent frame
	CHECK(repeatedDelay >= 40);
}

} // namespace cxtest
