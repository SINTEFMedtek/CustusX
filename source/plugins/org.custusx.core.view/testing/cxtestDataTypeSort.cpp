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

#include "cxMesh.h"
#include "cxImage.h"
#include "cxDataMetric.h"
#include "cxPointMetric.h"
#include "cxViewWrapper.h"
#include "cxTypeConversions.h"
#include "cxtestSpaceProviderMock.h"
#include "cxCoreServices.h"
#include "cxtestPatientModelServiceMock.h"
#include "cxLogger.h"
#include "cxtestDummyDataManager.h"

TEST_CASE("Sort cx::Data user-friendly using getPriority()", "[unit][service][visualization]")
{
	cx::MeshPtr mesh(new cx::Mesh("mesh1     "));
	cx::ImagePtr image_mr(new cx::Image("image1_mr ", vtkImageDataPtr()));
	image_mr->setModality("MR");
	cx::ImagePtr image_ct(new cx::Image("image1_ct ", vtkImageDataPtr()));
	image_ct->setModality("CT");
	cx::ImagePtr image_us(new cx::Image("image1_us ", vtkImageDataPtr()));
	image_us->setModality("US");
	image_us->setImageType("B-Mode");
	cx::ImagePtr image_usa(new cx::Image("image1_usa", vtkImageDataPtr()));
	image_usa->setModality("US");
	image_usa->setImageType("Angio");
	cx::PointMetricPtr point = cx::PointMetric::create("point1    ", "", cx::PatientModelServicePtr(), cxtest::SpaceProviderMock::create());

	std::vector<cx::DataPtr> unsorted1;
	unsorted1.push_back(image_us);
	unsorted1.push_back(point);
	unsorted1.push_back(image_mr);
	unsorted1.push_back(mesh);
	unsorted1.push_back(image_usa);
	unsorted1.push_back(image_ct);
	std::vector<cx::DataPtr> unsorted2 = unsorted1;

	std::vector<cx::DataPtr> sorted;
	sorted.push_back(image_ct);
	sorted.push_back(image_mr);
	sorted.push_back(image_us);
	sorted.push_back(image_usa);
	sorted.push_back(mesh);
	sorted.push_back(point);

	// sanity check: should be entirely unsorted at start
	CHECK(unsorted1.size()==sorted.size());
	for (unsigned i=0; i<sorted.size(); ++i)
		CHECK(unsorted1[i]!=sorted[i]);

	std::sort(unsorted1.begin(), unsorted1.end(), &cx::dataTypeSort);

	// check sorting success
	CHECK(unsorted1.size()==sorted.size());
	for (unsigned i=0; i<sorted.size(); ++i)
		CHECK(unsorted1[i]==sorted[i]);

	cxtest::TestVisServicesPtr nullBackend = cxtest::TestVisServices::create();

	for (unsigned i=0; i<unsorted2.size(); ++i)
		nullBackend->patient()->insertData(unsorted2[i]);

	cx::ViewGroupData vgData(nullBackend);
	for (unsigned i=0; i<unsorted2.size(); ++i)
		vgData.addDataSorted(unsorted2[i]->getUid());
	std::vector<cx::DataPtr> sorted2 = vgData.getData();

	// check sorting success
	CHECK(sorted2.size()==sorted.size());
	for (unsigned i=0; i<sorted.size(); ++i)
		CHECK(sorted2[i]==sorted[i]);
}

