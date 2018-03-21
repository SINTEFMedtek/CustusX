/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestVLCRecorderFixture.h"

#include "cxReporter.h"
#include "cxTime.h"
#include "cxDataLocations.h"
#include "cxVLCRecorder.h"
#include "cxtestUtilities.h"

#include "catch.hpp"
#include <QDir>
#include <QFileInfo>

namespace cxtest
{
VLCRecorderFixture::VLCRecorderFixture() :
		mMoviePath("")
{
	cx::reporter()->initialize();
	cx::vlc()->getInstance();
}

VLCRecorderFixture::~VLCRecorderFixture()
{
	cx::reporter()->shutdown();
}

void VLCRecorderFixture::createMovieFilePath()
{
	mMoviePath = cx::DataLocations::getTestDataPath() + "/testing/VLC/";
	QDir().mkpath(mMoviePath);
	QString format = cx::timestampSecondsFormat();
	QString filename = QDateTime::currentDateTime().toString(format);
	mMoviePath += filename+".mp4";
}

void VLCRecorderFixture::checkThatVLCCanRecordTheScreen(int secondsToRecord)
{
	this->createMovieFilePath();

	CHECK_FALSE(cx::vlc()->isRecording());

	cx::vlc()->startRecording(mMoviePath);
	REQUIRE(cx::vlc()->waitForStarted());
	CHECK(cx::vlc()->isRecording());

	Utilities::sleep_sec(secondsToRecord);

	cx::vlc()->stopRecording();
	REQUIRE(cx::vlc()->waitForFinished());
	CHECK_FALSE(cx::vlc()->isRecording());
}

void VLCRecorderFixture::checkIsMovieFileOfValidSize()
{
	CHECK(QFile::exists(mMoviePath));

	QFileInfo info(mMoviePath);

	int byteSizeForUnfinishedMovies = 717; //number fetched on a mac
	CHECK(info.size() > byteSizeForUnfinishedMovies);
}
} /* namespace cxtest */
