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
	void testNewPresets(); ///< Tests if framework for saving new presets works
	void testDeletePresets(); ///< Tests if deleting presets work

private:
	QDomElement createPresetElement(TSFPresetsPtr presets);

public:
	CPPUNIT_TEST_SUITE( TestTSFPresets );
		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testLoadPresets );
		CPPUNIT_TEST( testNewPresets );
		CPPUNIT_TEST( testDeletePresets );
	CPPUNIT_TEST_SUITE_END();

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestTSFPresets );
} /* namespace cx */
#endif /* CXTESTTSFPRESETS_H_ */
