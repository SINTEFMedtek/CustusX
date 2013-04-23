#include "catch.hpp"

//=================================================================
// TSF test
//=================================================================

#ifdef CX_USE_TSF
#include "parameters.hpp"
#include "tsf-config.h"
#endif //CX_USE_TSF

SCENARIO("Loading the Neuro-Vessels-USA preset", "[TSF]"){
	GIVEN("we init the parameters with default values"){
		std::string path = std::string(PARAMETERS_DIR);
		paramList neuroVesselsUSAParameters = initParameters(path);

		WHEN("we set the parameters option to Neuro-Vessels-USA"){
			setParameter(neuroVesselsUSAParameters, "parameters", "Neuro-Vessels-USA");

			AND_WHEN("we load the parameter presets"){
				loadParameterPreset(neuroVesselsUSAParameters, path);

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
			}
		}
	}
}

//=================================================================
