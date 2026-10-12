/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <memory>
#include "cxData.h"
#include "cxImage.h"
#include "cxDataLocations.h"
#include "cxPatientModelService.h"
#include "cxAirwaysFromCenterline.h"
#include "cxtestSessionStorageTestFixture.h"
#include "cxVisServices.h"
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include "cxBranchList.h"
#include "cxBranch.h"
#include "cxMesh.h"

typedef std::shared_ptr<class cx::AirwaysFromCenterline> AirwaysFromCenterlinePtr;

namespace cxtest {

namespace
{
const double cylinderRadius = 4.0; //mm

// Segmentation of a cylinder along z, centered at x=y=10 mm
cx::ImagePtr createCylinderSegmentation()
{
	vtkImageDataPtr image = vtkImageDataPtr::New();
	image->SetDimensions(41, 41, 81);
	image->SetSpacing(0.5, 0.5, 0.5);
	image->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
	int* dim = image->GetDimensions();
	for (int z = 0; z < dim[2]; ++z)
		for (int y = 0; y < dim[1]; ++y)
			for (int x = 0; x < dim[0]; ++x)
			{
				double distance = std::sqrt(std::pow(x*0.5 - 10, 2) + std::pow(y*0.5 - 10, 2));
				unsigned char* voxel = static_cast<unsigned char*>(image->GetScalarPointer(x, y, z));
				*voxel = (distance <= cylinderRadius + 1e-6) ? 1 : 0;
			}
	return cx::ImagePtr(new cx::Image("cylinder", image));
}

// Trachea and a child branch along the cylinder axis, with positions closer than the 0.5 mm point reduction in getVTKPoints()
cx::BranchListPtr createBranchesAlongCylinder()
{
	Eigen::MatrixXd tracheaPositions(3, 101);
	for (int i = 0; i < tracheaPositions.cols(); ++i)
		tracheaPositions.col(i) = Eigen::Vector3d(10, 10, 5 + 0.2*i);
	Eigen::MatrixXd childPositions(3, 51);
	for (int i = 0; i < childPositions.cols(); ++i)
		childPositions.col(i) = Eigen::Vector3d(10, 10, 25.2 + 0.2*i);

	cx::BranchPtr trachea(new cx::Branch());
	trachea->setPositions(tracheaPositions);
	cx::BranchPtr child(new cx::Branch());
	child->setPositions(childPositions);
	child->setParentBranch(trachea);
	trachea->addChildBranch(child);

	cx::BranchListPtr branchList(new cx::BranchList());
	branchList->addBranch(trachea);
	branchList->addBranch(child);
	return branchList;
}

void checkRadiusArray(vtkPolyDataPtr centerline)
{
	vtkDataArray* radius = centerline->GetPointData()->GetArray(cx::AirwaysFromCenterline::getRadiusArrayName().toStdString().c_str());
	REQUIRE(radius);
	REQUIRE(radius->GetNumberOfTuples() == centerline->GetNumberOfPoints());
	for (vtkIdType i = 0; i < radius->GetNumberOfTuples(); ++i)
		CHECK(std::abs(radius->GetTuple1(i) - cylinderRadius) <= 0.5);
	CHECK_FALSE(centerline->GetPointData()->GetScalars()); // radius should not color the centerline
}
} // namespace

TEST_CASE("AirwaysFromCenterline: smoothed centerline contains airway radius for each point", "[unit][org.custusx.filter.airwaysfromcenterline]")
{
	cx::BranchListPtr branchList = createBranchesAlongCylinder();
	branchList->setRadius(createCylinderSegmentation());
	cx::AirwaysFromCenterline airwaysFromCenterline;
	airwaysFromCenterline.setBranches(branchList);

	vtkPolyDataPtr centerline = airwaysFromCenterline.getVTKPoints();

	CHECK(centerline->GetNumberOfPoints() < 152); // positions are reduced
	checkRadiusArray(centerline);
}

TEST_CASE("AirwaysFromCenterline: smoothed centerline has no radius array when radius is not calculated", "[unit][org.custusx.filter.airwaysfromcenterline]")
{
	cx::AirwaysFromCenterline airwaysFromCenterline;
	airwaysFromCenterline.setBranches(createBranchesAlongCylinder());

	vtkPolyDataPtr centerline = airwaysFromCenterline.getVTKPoints();

	CHECK(centerline->GetNumberOfPoints() > 0);
	CHECK_FALSE(centerline->GetPointData()->HasArray(cx::AirwaysFromCenterline::getRadiusArrayName().toStdString().c_str()));
}

TEST_CASE("AirwaysFromCenterline: radius can be added to existing centerline mesh", "[unit][org.custusx.filter.airwaysfromcenterline]")
{
	cx::AirwaysFromCenterline airwaysFromCenterline;
	airwaysFromCenterline.setBranches(createBranchesAlongCylinder());
	cx::MeshPtr centerline = cx::MeshPtr(new cx::Mesh("centerline"));
	centerline->setVtkPolyData(airwaysFromCenterline.getVTKPoints());

	REQUIRE(cx::AirwaysFromCenterline::addRadiusToCenterline(centerline, createCylinderSegmentation()));

	checkRadiusArray(centerline->getVtkPolyData());
	CHECK_FALSE(cx::AirwaysFromCenterline::addRadiusToCenterline(centerline, cx::ImagePtr()));
}


TEST_CASE("AirwaysFromCenterline: execute", "[integration][org.custusx.filter.airwaysfromcenterline]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	storageFixture.loadSession1();//Create test patient folder

    //setup filter
    AirwaysFromCenterlinePtr airwaysFromCenterline = AirwaysFromCenterlinePtr(new cx::AirwaysFromCenterline());
    REQUIRE(airwaysFromCenterline);

    QString filenameCenterline = cx::DataLocations::getTestDataPath()+"/testing/Lung/Thorax__1_0_I30f_tsf_cl1.vtk";

    //create a new patient
	QString info;
	cx::DataPtr dataCenterline = storageFixture.mServices->patient()->importData(filenameCenterline, info);

    REQUIRE(dataCenterline);

    cx::MeshPtr mesh = std::dynamic_pointer_cast<cx::Mesh>(dataCenterline);

		airwaysFromCenterline->processCenterline(mesh);

    vtkPolyDataPtr outputAirwayMesh = airwaysFromCenterline->generateTubes();

    cx::MeshPtr outputCenterline = cx::MeshPtr(new cx::Mesh("test"));
    outputCenterline->setVtkPolyData(airwaysFromCenterline->getVTKPoints());

    REQUIRE(outputAirwayMesh);
    REQUIRE(outputCenterline->getVtkPolyData());
}

}; // end cxtest namespace
