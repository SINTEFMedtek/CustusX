#include "testing/cxTestTSFPresets.h"

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include "cxLogicManager.h"
#include "cxDataLocations.h"

namespace cx {

void TestTSFPresets::setUp()
{
	cx::LogicManager::initialize();

	mFolderPath = cx::DataLocations::getTSFPath()+"/parameters/";
	mPresetFileName = "CPPUNIT_TEST";
	mPresetFilePath = mFolderPath+"centerline-gpu/"+mPresetFileName;
}

void TestTSFPresets::tearDown()
{
	cx::LogicManager::shutdown();
}

void TestTSFPresets::testConstructor()
{}

void TestTSFPresets::testLoadPresets()
{
	TSFPresetsPtr presets(new TSFPresets());

	QStringList presetList = presets->getPresetList("");
	CPPUNIT_ASSERT_MESSAGE("List of presets is shorter than expected.", presetList.size() >= 0);

	foreach(QString preset, presetList)
		CPPUNIT_ASSERT_MESSAGE("Default preset "+preset.toStdString()+" is defined as default.", !presets->isDefaultPreset(preset));
}

void TestTSFPresets::testSaveNewPresets()
{
	TSFPresetsPtr presets = this->newPreset();
	CPPUNIT_ASSERT_MESSAGE("Custom preset "+mPresetFileName.toStdString()+" is defined as default.", !presets->isDefaultPreset(mPresetFileName));

	CPPUNIT_ASSERT_MESSAGE("Could not locate the newly created preset file: "+mPresetFilePath.toStdString(), QFile::exists(mPresetFilePath));

	CPPUNIT_ASSERT_MESSAGE("New parameter file is empty.", QFile(mPresetFilePath).size() != 0);
}

void TestTSFPresets::testDeletePresets()
{
	TSFPresetsPtr preset = this->newPreset();
	this->deletePreset(preset);

	CPPUNIT_ASSERT_MESSAGE("Preset file was not removed when deleted: "+mPresetFilePath.toStdString(), !QFile::exists(mPresetFilePath));
}

QDomElement TestTSFPresets::createPresetElement(TSFPresetsPtr presets)
{
	std::map<QString, QString> newPresetMap;
	newPresetMap["centerline-method"] = "gpu"; //str
	newPresetMap["centerpoints-only"] = "true"; //bool
	newPresetMap["radius-min"] = "50.0"; //float/double

	QDomElement retval = TSFPresets::createPresetElement(mPresetFileName, newPresetMap);
	return retval;
}

TSFPresetsPtr TestTSFPresets::newPreset()
{
	TSFPresetsPtr presets(new TSFPresets());
	QDomElement element = this->createPresetElement(presets);
	presets->addCustomPreset(element);
	presets->save();

	return presets;
}

void TestTSFPresets::deletePreset(TSFPresetsPtr preset)
{
	preset->deleteCustomPreset(mPresetFileName);
	preset->remove();
}

} /* namespace cx */
