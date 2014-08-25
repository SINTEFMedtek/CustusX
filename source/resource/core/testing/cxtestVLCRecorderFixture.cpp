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
