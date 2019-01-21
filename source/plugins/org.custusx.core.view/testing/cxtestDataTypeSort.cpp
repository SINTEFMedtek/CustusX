/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxtestVisServices.h"

TEST_CASE("Sort cx::Data user-friendly using getPriority()", "[unit][service][visualization]")
{
	cx::MeshPtr mesh(new cx::Mesh("mesh1     "));
	cx::ImagePtr image_mr(new cx::Image("image1_mr ", vtkImageDataPtr()));
	image_mr->setModality(cx::imMR);
	cx::ImagePtr image_ct(new cx::Image("image1_ct ", vtkImageDataPtr()));
	image_ct->setModality(cx::imCT);
	cx::ImagePtr image_us(new cx::Image("image1_us ", vtkImageDataPtr()));
	image_us->setModality(cx::imUS);
	image_us->setImageType(cx::istUSBMODE);
	cx::ImagePtr image_usa(new cx::Image("image1_usa", vtkImageDataPtr()));
	image_usa->setModality(cx::imUS);
	image_usa->setImageType(cx::istANGIO);
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
	{
		INFO(unsorted1[i]->getUid() + " == " + sorted[i]->getUid());
		CHECK(unsorted1[i]==sorted[i]);
	}

	cxtest::TestVisServicesPtr nullBackend = cxtest::TestVisServices::create();

	for (unsigned i=0; i<unsorted2.size(); ++i)
		nullBackend->patient()->insertData(unsorted2[i]);

	cx::ViewGroupData vgData(nullBackend, "0");
	for (unsigned i=0; i<unsorted2.size(); ++i)
		vgData.addDataSorted(unsorted2[i]->getUid());
	std::vector<cx::DataPtr> sorted2 = vgData.getData();

	// check sorting success
	CHECK(sorted2.size()==sorted.size());
	for (unsigned i=0; i<sorted.size(); ++i)
	{
		INFO(sorted2[i]->getUid() + " == " + sorted[i]->getUid());
		CHECK(sorted2[i]==sorted[i]);
	}
}

