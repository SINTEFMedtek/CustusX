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
#include <ctkPluginContext.h>
#include "cxDilationFilter.h"
#include "cxDataLocations.h"
#include "cxSelectDataStringProperty.h"
#include "cxData.h"
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxSessionStorageService.h"
#include "cxVisServices.h"
#include "cxtestVisServices.h"

TEST_CASE("DilationFilter: execute", "[unit][modules][Algorithm][DilationFilter]")
{
	cx::DataLocations::setTestMode();

	{
		cxtest::TestVisServicesPtr dummyservices = cxtest::TestVisServices::create();

		// Setup filter
		cx::DilationFilterPtr filter(new cx::DilationFilter(dummyservices));
		REQUIRE(filter);
		filter->getInputTypes();
		filter->getOutputTypes();
		filter->getOptions();

		QString filename = cx::DataLocations::getTestDataPath()+ "/testing/DilationFilter/helix_seg.mhd";
		QString info;
		cx::DataPtr data = dummyservices->patient()->importData(filename, info);
		REQUIRE(data);

		//set input
		std::vector < cx::SelectDataStringPropertyBasePtr > input =filter->getInputTypes();
		{
			INFO("Number of inputs has changed.");
			REQUIRE(input.size() == 1);
		}
		{
			INFO("Could not set input to the filter.");
			REQUIRE(input[0]->setValue(data->getUid()));
		}
		{
			INFO("The name of the input data is not as we requested.");
			REQUIRE(input[0]->getData()->getName() == "helix_seg");
		}
		// Execute
		{
            INFO("Preprocessing DilationFilter failed.");
			REQUIRE(filter->preProcess());
		}

		{
			REQUIRE(filter->execute());
		}
		{
            INFO("Post processing data from Dilation Filter failed.");
			REQUIRE(filter->postProcess());
		}

		// Check output
		std::vector < cx::SelectDataStringPropertyBasePtr > output = filter->getOutputTypes();
		{
			INFO("Number of outputs has changed.");
			REQUIRE(output.size() == 2);
		}
		{
			INFO("Segmentation volume not generated.");
			REQUIRE(output[0]->getData());
		}
		{
			INFO("Surface/contour not generated.");
			REQUIRE(output[1]->getData());
		}
	}
}
