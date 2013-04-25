#include "catch.hpp"

#ifdef CX_USE_TSF

//=================================================================
// TSF library tests
//=================================================================

#include "parameters.hpp"
#include "tsf-config.h"

SCENARIO("Loading the Neuro-Vessels-USA (gpu) preset", "[TSF]"){
	GIVEN("we init the parameters with default values"){
		std::string path = std::string(PARAMETERS_DIR);
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

SCENARIO("Loading the Phantom-Acc-US (gpu) preset", "[TSF]"){
	GIVEN("we init the parameters with default values"){
		std::string path = std::string(PARAMETERS_DIR);
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
#include <QStringList>

TEST_CASE("should load tsf presets from file location and get more than 0 presets", "[TSF][TSFPresets]"){
	cx::TSFPresetsPtr presets(new cx::TSFPresets());

	QStringList presetList = presets->getPresetList();
	CHECK(presetList.size() > 0);

	foreach ( QString item, presetList)
		std::cout << item.toStdString() << std::endl;
}

#endif //CX_USE_TSF
