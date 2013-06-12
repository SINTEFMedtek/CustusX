#ifndef CXTESTTSFPRESETS_H_
#define CXTESTTSFPRESETS_H_

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <QDomElement>

#ifdef CX_USE_TSF
#include "cxTSFPresets.h"
#endif //CX_USE_TSF

namespace cx {

class TestTSFPresets  : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testConstructor();///< Test constructor

	void testLoadPresets(); ///< Test that loading presets from file works
	void testSaveNewPresets(); ///< Tests if framework for saving new presets works
	void testDeletePresets(); ///< Tests if deleting presets work

private:
	QDomElement createPresetElement(TSFPresetsPtr presets);
	TSFPresetsPtr newPreset(); ///< creates a new test preset
	void deletePreset(TSFPresetsPtr preset); ///< deletes the test preset

	QString mFolderPath; ///< path to folder where all preset are stored
	QString mPresetFileName; ///< file name of the preset file generated
	QString mPresetFilePath; ///< path to new preset file
	QString mBackUpPostFix; ///< string to append to backed up folder

public:
	CPPUNIT_TEST_SUITE( TestTSFPresets );
		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testLoadPresets );
		CPPUNIT_TEST( testSaveNewPresets );
		CPPUNIT_TEST( testDeletePresets );
	CPPUNIT_TEST_SUITE_END();

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestTSFPresets );
} /* namespace cx */
#endif /* CXTESTTSFPRESETS_H_ */
