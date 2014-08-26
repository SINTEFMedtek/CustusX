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
#include "cxtestTSFPresetsFixture.h"
#include "catch.hpp"

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include "cxLogicManager.h"
#include "cxDataLocations.h"

TEST_CASE_METHOD(cxtest::TSFPresetsFixture, "TSFPresets: Create fixture", "[unit][modules][Algorithm][TSF]")
{
	CHECK(true);
}

TEST_CASE_METHOD(cxtest::TSFPresetsFixture, "TSFPresets: Load presets", "[unit][modules][Algorithm][TSF]")
{
	cx::TSFPresetsPtr presets(new cx::TSFPresets());

	QStringList presetList = presets->getPresetList("");
	INFO("List of presets is shorter than expected.");
	CHECK(presetList.size() >= 0);

	foreach(QString preset, presetList)
	{
		INFO("Default preset "+preset.toStdString()+" is defined as default.");
		CHECK(!presets->isDefaultPreset(preset));
	}
}

TEST_CASE_METHOD(cxtest::TSFPresetsFixture, "TSFPresets: Save new preset", "[unit][modules][Algorithm][TSF]")
{
	cx::TSFPresetsPtr presets = this->newPreset();
	INFO("Custom preset "+mPresetFileName.toStdString()+" is defined as default.");
	CHECK(!presets->isDefaultPreset(mPresetFileName));

	INFO("Could not locate the newly created preset file: "+mPresetFilePath.toStdString());
	CHECK(QFile::exists(mPresetFilePath));

	INFO("New parameter file is empty.");
	CHECK(QFile(mPresetFilePath).size() != 0);
}

TEST_CASE_METHOD(cxtest::TSFPresetsFixture, "TSFPresets: Delete presets", "[unit][modules][Algorithm][TSF]")
{
	cx::TSFPresetsPtr preset = this->newPreset();
	this->deletePreset(preset);

	INFO("Preset file was not removed when deleted: "+mPresetFilePath.toStdString());
	CHECK(!QFile::exists(mPresetFilePath));
}
