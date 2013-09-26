/*
 * cxtestCatchTSFPresets.cpp
 *
 *  Created on: Sep 26, 2013
 *      Author: jbake
 */

#include "cxtestTSFPresetsFixture.h"
#include "catch.hpp"

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include "cxLogicManager.h"
#include "cxDataLocations.h"

TEST_CASE_METHOD(cxtest::TSFPresetsFixture, "TSFPresets: Create fixture", "[unit][plugins][Algorithm][TSF]")
{
	CHECK(true);
}

TEST_CASE_METHOD(cxtest::TSFPresetsFixture, "TSFPresets: Load presets", "[unit][plugins][Algorithm][TSF]")
{
	cx::TSFPresetsPtr presets(new cx::TSFPresets());

	QStringList presetList = presets->getPresetList("");
	INFO("List of presets is shorter than expected.");
	CHECK(presetList.size() >= 0);
//	CPPUNIT_ASSERT_MESSAGE("List of presets is shorter than expected.", presetList.size() >= 0);

	foreach(QString preset, presetList)
	{
		INFO("Default preset "+preset.toStdString()+" is defined as default.");
		CHECK(!presets->isDefaultPreset(preset));
//		CPPUNIT_ASSERT_MESSAGE("Default preset "+preset.toStdString()+" is defined as default.", !presets->isDefaultPreset(preset));
	}
}

TEST_CASE_METHOD(cxtest::TSFPresetsFixture, "TSFPresets: Save new preset", "[unit][plugins][Algorithm][TSF]")
{
	cx::TSFPresetsPtr presets = this->newPreset();
	INFO("Custom preset "+mPresetFileName.toStdString()+" is defined as default.");
	CHECK(!presets->isDefaultPreset(mPresetFileName));
//	CPPUNIT_ASSERT_MESSAGE("Custom preset "+mPresetFileName.toStdString()+" is defined as default.", !presets->isDefaultPreset(mPresetFileName));

	INFO("Could not locate the newly created preset file: "+mPresetFilePath.toStdString());
	CHECK(QFile::exists(mPresetFilePath));
//	CPPUNIT_ASSERT_MESSAGE("Could not locate the newly created preset file: "+mPresetFilePath.toStdString(), QFile::exists(mPresetFilePath));

	INFO("New parameter file is empty.");
	CHECK(QFile(mPresetFilePath).size() != 0);
//	CPPUNIT_ASSERT_MESSAGE("New parameter file is empty.", QFile(mPresetFilePath).size() != 0);
}

TEST_CASE_METHOD(cxtest::TSFPresetsFixture, "TSFPresets: Delete presets", "[unit][plugins][Algorithm][TSF]")
{
	cx::TSFPresetsPtr preset = this->newPreset();
	this->deletePreset(preset);

	INFO("Preset file was not removed when deleted: "+mPresetFilePath.toStdString());
	CHECK(!QFile::exists(mPresetFilePath));
//	CPPUNIT_ASSERT_MESSAGE("Preset file was not removed when deleted: "+mPresetFilePath.toStdString(), !QFile::exists(mPresetFilePath));
}
