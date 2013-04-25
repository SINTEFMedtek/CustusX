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
	mParameterFilePath = cx::DataLocations::getTSFPath()+"/parameters/parameters";
	mBackUpPostFix = "_BackUp";

	mDirty = false;
	this->backup();
}

void TestTSFPresets::tearDown()
{
	this->restore();
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
	{
		CPPUNIT_ASSERT_MESSAGE("Default preset "+preset.toStdString()+" is not defined as default.", presets->isDefaultPreset(preset));
//		QStringList split = preset.split(": ");
//		foreach(QString string, split)
//		{
//			if(!string.contains("centerline"))
//				CPPUNIT_ASSERT_MESSAGE("Preset "+string.toStdString()+" is not defined as default.", presets->isDefaultPreset(string));
//		}
	}
}

void TestTSFPresets::testNewPresets()
{
	TSFPresetsPtr presets = this->newPreset();
	CPPUNIT_ASSERT_MESSAGE("Custom preset "+mPresetFileName.toStdString()+" is defined as default.", !presets->isDefaultPreset(mPresetFileName));

	CPPUNIT_ASSERT_MESSAGE("Could not locate the newly created preset file: "+mPresetFilePath.toStdString(), QFile::exists(mPresetFilePath));

	CPPUNIT_ASSERT_MESSAGE("New parameter file is empty.", QFile(mPresetFilePath).size() != 0);

	QFile parameters(mParameterFilePath);
	CPPUNIT_ASSERT_MESSAGE("Could not open parameter file for reading.", parameters.open(QFile::ReadOnly));

	QTextStream in;
	in.setDevice(&parameters);
	QString text = in.readAll();
	parameters.close();
	CPPUNIT_ASSERT_MESSAGE("New preset name not added to parameter file.", text.contains(mPresetFileName, Qt::CaseInsensitive));
}

void TestTSFPresets::testDeletePresets()
{
	TSFPresetsPtr preset = this->newPreset();
	this->deletePreset(preset);

	CPPUNIT_ASSERT_MESSAGE("Preset file was not removed when deleted: "+mPresetFilePath.toStdString(), !QFile::exists(mPresetFilePath));

	QFile parameters(mParameterFilePath);
	CPPUNIT_ASSERT_MESSAGE("Could not open parameter file for reading.", parameters.open(QFile::ReadOnly));

	QTextStream in;
	in.setDevice(&parameters);
	QString text = in.readAll();
	parameters.close();
	CPPUNIT_ASSERT_MESSAGE("New preset name not removed from parameter file.", !text.contains(mPresetFileName, Qt::CaseInsensitive));
}

QDomElement TestTSFPresets::createPresetElement(TSFPresetsPtr presets)
{

	std::map<QString, QString> newPresetMap;
	newPresetMap["centerline-method"] = "gpu"; //str
	newPresetMap["centerpoints-only"] = "true"; //bool
	newPresetMap["radius-min"] = "50.0"; //float/double

	//create xml
	QDomElement retval = TSFPresets::createPresetElement(mPresetFileName, newPresetMap);
	return retval;
}

void TestTSFPresets::backup()
{
	if(mDirty)
		return;

	QFile parameters(mParameterFilePath);
	parameters.copy(mParameterFilePath+mBackUpPostFix);
}

void TestTSFPresets::restore()
{
	if(!mDirty)
		return;

	QFile oldParameters(mParameterFilePath);
	oldParameters.remove();

	QFile parameters(mParameterFilePath+mBackUpPostFix);
	parameters.rename(mParameterFilePath);

	if(QFile::exists(mPresetFilePath))
	{
		QFile newPreset(mPresetFilePath);
		newPreset.remove();
	}
}

TSFPresetsPtr TestTSFPresets::newPreset()
{
	TSFPresetsPtr presets(new TSFPresets());
	QDomElement element = this->createPresetElement(presets);
	presets->addCustomPreset(element);
	presets->save();

	mDirty = true;

	return presets;
}

void TestTSFPresets::deletePreset(TSFPresetsPtr preset)
{
	preset->deleteCustomPreset(mPresetFileName);
	preset->remove();
	//mDirty = false;
}

} /* namespace cx */
