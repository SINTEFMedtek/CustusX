
#ifndef CXTESTTSFPRESETSFIXTURE_H_
#define CXTESTTSFPRESETSFIXTURE_H_

#include <QDomElement>

#ifdef CX_USE_TSF
#include "cxTSFPresets.h"
#endif //CX_USE_TSF

namespace cxtest {

/**
 * cxtestTSFPresetsFixture.h
 *
 *  Created on: Sep 26, 2013
 *      Author: jbake
 */
class TSFPresetsFixture {

public:
	TSFPresetsFixture();
	~TSFPresetsFixture();

	void setUp();
	void tearDown();

protected:
	QDomElement createPresetElement(cx::TSFPresetsPtr presets);
	cx::TSFPresetsPtr newPreset(); ///< creates a new test preset
	void deletePreset(cx::TSFPresetsPtr preset); ///< deletes the test preset

	QString mFolderPath; ///< path to folder where all preset are stored
	QString mPresetFileName; ///< file name of the preset file generated
	QString mPresetFilePath; ///< path to new preset file
	QString mBackUpPostFix; ///< string to append to backed up folder
};

} /* namespace cxtest */

#endif /* CXTESTTSFPRESETSFIXTURE_H_ */
