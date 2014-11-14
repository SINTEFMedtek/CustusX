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

#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkCellArray.h>
#include "cxDataManager.h"
#include "cxMesh.h"
#include "cxVector3D.h"
#include "cxDataLocations.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxRegistrationTransform.h"
#include "cxTypeConversions.h"
#include <QFileInfo>
#include <QDir>
#include "cxXmlOptionItem.h"

#include "catch.hpp"

#include "cxLogger.h"
#include "cxTime.h"
#include "cxDataLocations.h"
#include "cxElastixExecuter.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxSettings.h"
#include "cxElastixSingleThreadedRunner.h"
#include "cxTypeConversions.h"
#include "cxElastixParameters.h"
#include "cxLogicManager.h"
#include "cxRegServices.h"

namespace cxtest
{

/** 
 *
 *
 * \ingroup cx
 * \date 6/7/2013, 2013
 * \author christiana
 */

class ElastiXFixture
{
public:
	ElastiXFixture()
	{
//		cx::Reporter::initialize();
//		cx::cxDataManager::initialize();
		cx::LogicManager::initialize();
	}

	~ElastiXFixture()
	{
		cx::LogicManager::shutdown();
//		cx::DataManager::shutdown();
//		cx::Reporter::shutdown();
	}
	bool compareTransforms(cx::Transform3D result, cx::Transform3D solution)
	{
		std::cout << "result\n" << result << std::endl;
		std::cout << "solution\n" << solution << std::endl;

		cx::Transform3D diff = solution * result.inv();

		std::cout << "diff\n" << diff << std::endl;

		cx::Vector3D t_delta = diff.matrix().block<3, 1>(0, 3);
		Eigen::AngleAxisd angleAxis = Eigen::AngleAxisd(diff.matrix().block<3, 3>(0, 0));
		double angle = angleAxis.angle();

		cx::Vector3D shift = diff.coord(cx::Vector3D(0,0,0));


		QString res = QString(""
			"Shift vector (r):\t%1\n"
			"Accuracy |v|:\t%2mm\n"
			"Angle:       \t%3*\n"
			"")
			.arg(qstring_cast(shift))
			.arg(shift.length(), 6, 'f', 2)
			.arg(angle / M_PI * 180.0, 6, 'f', 2);

		std::cout << res << std::endl;

		return (fabs(angle/M_PI*180.0) < 0.1) && (shift.length() < 0.1);
	}
};



TEST_CASE("ElastiX should register kaisa to a translated+resampled version of same", "[pluginRegistration][integration][not_win32][not_win64]")
{
	ElastiXFixture fixture;

	QString kaisa_padded_fname = cx::DataLocations::getTestDataPath() + "/testing/elastiX/kaisa_padded.mhd";
	QString kaisa_resliced_fname = cx::DataLocations::getTestDataPath() + "/testing/elastiX/kaisa_resliced.mhd";
	QString kaisa_resliced_linear_fname = cx::DataLocations::getTestDataPath() + "/testing/elastiX/kaisa_resliced_linear.mhd";

	std::cout << "------" << kaisa_padded_fname << std::endl;
	cx::DataPtr kaisa_resliced_linear = cx::dataManager()->loadData("source_"+kaisa_resliced_linear_fname, kaisa_resliced_linear_fname);
	cx::DataPtr kaisa_padded = cx::dataManager()->loadData("source_"+kaisa_padded_fname, kaisa_padded_fname);
	cx::DataPtr kaisa_resliced = cx::dataManager()->loadData("source_"+kaisa_resliced_fname, kaisa_resliced_fname);

	REQUIRE(kaisa_resliced_linear.get());
	REQUIRE(kaisa_padded.get());
	QString elastixPreset = "elastix/p_Rigid"; // this is an autogenerated preset

	cx::Transform3D solution;
	solution.matrix() <<
				1, 0 ,0, 10,
				0, 1, 0,  5,
				0, 0, 1, -2,
				0, 0, 0,  1;

	cx::Transform3D result = cx::Transform3D::Identity();

	cx::ElastixParametersPtr parameters(new cx::ElastixParameters(cx::XmlOptionFile()));
	parameters->getCurrentPreset()->setValue(elastixPreset);


	cx::RegServices services = cx::RegServices::getNullObjects();

	cx::ElastixSingleThreadedRunner runner(services);
	REQUIRE(runner.registerLinear(kaisa_padded, kaisa_resliced_linear, parameters, &result));

	REQUIRE(fixture.compareTransforms(result, solution) == true);
}


} // namespace cxtest


