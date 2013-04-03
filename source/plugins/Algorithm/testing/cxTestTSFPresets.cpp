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
}

void TestTSFPresets::tearDown()
{
	cx::LogicManager::shutdown();
}

void TestTSFPresets::testConstructor()
{
}

void TestTSFPresets::testLoadPresets()
{
	TSFPresetsPtr presets(new TSFPresets());

	QStringList presetList = presets->getPresetList("");
	CPPUNIT_ASSERT_MESSAGE("List of presets shorter than expected.", presetList.size() >= 6);

	foreach(QString preset, presetList)
	{
		QStringList split = preset.split(": ");
		foreach(QString string, split)
		{
			if(!string.contains("centerline"))
				//there is no way to find out if a preset is custom or not in tsf today
				CPPUNIT_ASSERT_MESSAGE("Preset "+string.toStdString()+" is defined as default.", !presets->isDefaultPreset(string));
		}
	}
}

void TestTSFPresets::testNewPresets()
{
	QString testName = "CPPUNIT_TEST";
	QString filepath = cx::DataLocations::getTSFPath()+"/parameters/"+testName;
	QString parametersPath = cx::DataLocations::getTSFPath()+"/parameters/parameters";

	TSFPresetsPtr presets(new TSFPresets());
	QDomElement element = this->createPresetElement(presets);
	presets->addCustomPreset(testName, element);
	presets->save();

	CPPUNIT_ASSERT_MESSAGE("Could not locate the newly created preset file: "+filepath.toStdString(), !QFile::exists(filepath));

	CPPUNIT_ASSERT_MESSAGE("New parameter file is empty.", QFile(filepath).size() == 0);

	QFile parameters(parametersPath);
	CPPUNIT_ASSERT_MESSAGE("Could not open parameter file for reading.", parameters.open(QFile::ReadOnly));

	QTextStream in;
	in.setDevice(&parameters);
	QString text = in.readAll();
	CPPUNIT_ASSERT_MESSAGE("New preset name not added to parameter file.", text.contains(testName, Qt::CaseInsensitive));
}

void TestTSFPresets::testDeletePresets()
{
	CPPUNIT_ASSERT_MESSAGE("Not implemented yet.", false);
}

QDomElement TestTSFPresets::createPresetElement(TSFPresetsPtr presets)
{

	std::map<QString, QString> newPresetMap;
	newPresetMap["centerline-method"] = "gpu"; //str
	newPresetMap["centerpoints-only"] = "true"; //bool
	newPresetMap["radius-min"] = "50.0"; //float/double

	//create xml
	QDomDocument doc = presets->getCustomFile().getDocument();
	QDomElement retval = doc.createElement("Preset");
	std::map<QString, QString>::iterator it2;
	for(it2 = newPresetMap.begin(); it2 != newPresetMap.end(); ++it2){
		QDomElement newNode = doc.createElement(it2->first);
		newNode.appendChild(doc.createTextNode(it2->second));
		retval.appendChild(newNode);
	}
	return retval;

}

} /* namespace cx */
