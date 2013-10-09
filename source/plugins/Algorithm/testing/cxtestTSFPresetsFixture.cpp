#include "cxtestTSFPresetsFixture.h"

#ifdef CX_USE_TSF
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include "cxLogicManager.h"
#include "cxDataLocations.h"

namespace cxtest {

TSFPresetsFixture::TSFPresetsFixture()
{
	this->setUp();
}

TSFPresetsFixture::~TSFPresetsFixture()
{
	this->tearDown();
}

void TSFPresetsFixture::setUp()
{
	cx::LogicManager::initialize();

	mFolderPath = cx::DataLocations::getTSFPath()+"/parameters/";
	mPresetFileName = "CPPUNIT_TEST";
	mPresetFilePath = mFolderPath+"centerline-gpu/"+mPresetFileName;
}

void TSFPresetsFixture::tearDown()
{
	cx::LogicManager::shutdown();
}

QDomElement TSFPresetsFixture::createPresetElement(cx::TSFPresetsPtr presets)
{
	std::map<QString, QString> newPresetMap;
	newPresetMap["centerline-method"] = "gpu"; //str
	newPresetMap["centerpoints-only"] = "true"; //bool
	newPresetMap["radius-min"] = "50.0"; //float/double

	QDomElement retval = cx::TSFPresets::createPresetElement(mPresetFileName, newPresetMap);
	return retval;
}

cx::TSFPresetsPtr TSFPresetsFixture::newPreset()
{
	cx::TSFPresetsPtr presets(new cx::TSFPresets());
	QDomElement element = this->createPresetElement(presets);
	presets->addCustomPreset(element);
	presets->save();

	return presets;
}

void TSFPresetsFixture::deletePreset(cx::TSFPresetsPtr preset)
{
	preset->deleteCustomPreset(mPresetFileName);
	preset->remove();
}

} /* namespace cxtest */

#endif //CX_USE_TSF
