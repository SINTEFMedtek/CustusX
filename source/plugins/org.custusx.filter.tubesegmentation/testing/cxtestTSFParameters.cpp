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

#include "catch.hpp"


//=================================================================
// TSF library tests
//=================================================================

#include "cxtestTSFFixture.h"
#include <QString>
#include "parameters.hpp"
#include "tsf-config.h"

SCENARIO("Loading the Neuro-Vessels-USA (gpu) preset", "[TSF][unit]"){
	GIVEN("we init the parameters with default values"){
		TestTubeSegmentationFramework helper;
		std::string path = helper.getParametersDir().toStdString();
		paramList neuroVesselsUSAParameters;
		REQUIRE_NOTHROW(neuroVesselsUSAParameters = initParameters(path));

		WHEN("we set the string parameters parameter to Neuro-Vessels-USA"){
			REQUIRE_NOTHROW(setParameter(neuroVesselsUSAParameters, "centerline-method", "gpu"));
			REQUIRE_NOTHROW(setParameter(neuroVesselsUSAParameters, "parameters", "Neuro-Vessels-USA"));

			AND_WHEN("we load the parameter presets"){
				REQUIRE_NOTHROW(loadParameterPreset(neuroVesselsUSAParameters, path));

				THEN("we can check that the expected values are set"){
					CHECK(getParamStr(neuroVesselsUSAParameters, "minimum") == "50");
					CHECK(getParamStr(neuroVesselsUSAParameters, "maximum") == "200");
					CHECK(getParam(neuroVesselsUSAParameters, "fmax") == Approx(0.1));
					CHECK(getParam(neuroVesselsUSAParameters, "min-mean-tdf") == 0.5);
					CHECK(getParam(neuroVesselsUSAParameters, "radius-min") == 1.5);
					CHECK(getParam(neuroVesselsUSAParameters, "radius-max") == 7.0);
					CHECK(getParam(neuroVesselsUSAParameters, "small-blur") == 2.0);
					CHECK(getParam(neuroVesselsUSAParameters, "large-blur") == 3.0);
					CHECK(getParamStr(neuroVesselsUSAParameters, "cropping") == "threshold");
					CHECK(getParam(neuroVesselsUSAParameters, "cropping-threshold") == 50);
					CHECK(getParam(neuroVesselsUSAParameters, "min-tree-length") == 10);
					CHECK(getParamBool(neuroVesselsUSAParameters, "sphere-segmentation") == true);
					CHECK(getParam(neuroVesselsUSAParameters, "cube-size") == 4);
				}
				AND_WHEN("we try to set a bool parameter to false"){
					REQUIRE_NOTHROW(setParameter(neuroVesselsUSAParameters, "sphere-segmentation", "false"));

					THEN("we can check that it get the correct value"){
						CHECK(getParamBool(neuroVesselsUSAParameters, "sphere-segmentation") == false);
					}
				}
			}
		}
	}
}

SCENARIO("Loading the Phantom-Acc-US (gpu) preset", "[TSF][unit]"){
	GIVEN("we init the parameters with default values"){
		TestTubeSegmentationFramework helper;
		std::string path = helper.getParametersDir().toStdString();
		paramList phantomAccUSParameters;
		REQUIRE_NOTHROW(phantomAccUSParameters = initParameters(path));

		WHEN("we set the string parameters parameter to Neuro-Vessels-US"){
			REQUIRE_NOTHROW(setParameter(phantomAccUSParameters, "centerline-method", "gpu"));
			REQUIRE_NOTHROW(setParameter(phantomAccUSParameters, "parameters", "Phantom-Acc-US"));

			AND_WHEN("we load the parameter presets"){
				REQUIRE_NOTHROW(loadParameterPreset(phantomAccUSParameters, path));

				THEN("we can check that the expected values are set"){
					CHECK(getParamStr(phantomAccUSParameters, "mode") == "white");
					CHECK(getParam(phantomAccUSParameters, "radius-min") == 5);
					CHECK(getParam(phantomAccUSParameters, "large-blur") == 5);
					CHECK(getParam(phantomAccUSParameters, "fmax") == Approx(0.05));
					CHECK(getParamStr(phantomAccUSParameters, "cropping") == "threshold");
					CHECK(getParam(phantomAccUSParameters, "cropping-threshold") == 100);
					CHECK(getParam(phantomAccUSParameters, "min-scan-lines-threshold") == 5);
					CHECK(getParamStr(phantomAccUSParameters, "minimum") == "0");
					CHECK(getParamStr(phantomAccUSParameters, "maximum") == "150");
					CHECK(getParamBool(phantomAccUSParameters, "no-segmentation") == true);
				}
				AND_WHEN("we try to set a bool parameter to false"){
					REQUIRE_NOTHROW(setParameter(phantomAccUSParameters, "no-segmentation", "false"));

					THEN("we can check that it get the correct value"){
						CHECK(getParamBool(phantomAccUSParameters, "no-segmentation") == false);
					}
				}
			}
		}
	}
}

//=================================================================
// TSFPresets tests
//=================================================================
#include "cxTSFPresets.h"
#include "cxPresetWidget.h"
#include <QStringList>

TEST_CASE("should load tsf presets from file location and get more than 0 presets", "[TSF][TSFPresets][unit]"){
	cx::TSFPresetsPtr presets(new cx::TSFPresets());

	QStringList presetList = presets->getPresetList();
    REQUIRE(presetList.size() > 0);
}

TEST_CASE("should be able to get id of tsfpreset", "[TSF][TSFPresets][unit]"){
    cx::TSFPresetsPtr presets(new cx::TSFPresets());

    QString presetId = presets->getId();
    REQUIRE(!(presetId.isNull() || presetId.isEmpty()));
}

TEST_CASE("should be able to read from settings file last used preset", "[TSF][TSFPresets][unit]"){
    cx::TSFPresetsPtr presets(new cx::TSFPresets());

    cx::PresetWidget* widget = new cx::PresetWidget(NULL);
    widget->setPresets(presets);
    QString selectedPreset = "Synthetic-Vascusynth";
    REQUIRE(widget->requestSetCurrentPreset(selectedPreset));
    QString lastUsedPresetName = widget->getLastUsedPresetNameFromSettingsFile();
    REQUIRE(!(lastUsedPresetName.isNull() || lastUsedPresetName.isEmpty()));
    REQUIRE(lastUsedPresetName.compare(selectedPreset) == 0);
    delete widget;
}
