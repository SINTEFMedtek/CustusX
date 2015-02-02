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


#include <QStringList>
#include <QFile>
#include <QTextStream>
#include "cxLogicManager.h"
#include "cxDataLocations.h"
#include "tsf-config.h"

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

	mFolderPath = cx::DataLocations::findConfigFilePath("", "/tsf", QString(KERNELS_DIR)) + "/parameters/";
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


