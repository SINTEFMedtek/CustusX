/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxDataLocations.h"
#include "cxPatientModelService.h"
#include "cxCenterlineRegistration.h"
#include "cxtestSessionStorageTestFixture.h"
#include "cxVisServices.h"
#include "cxTypeConversions.h"
#include <cmath>
#include <random>

typedef boost::shared_ptr<cx::CenterlineRegistration> CenterlineRegistrationPtr;


namespace cxtest {


// This test just use two random centerlines and register them to each other.
// The test only verifies that the code is running without crashing, and that all objects are created.
TEST_CASE("CenterlineRegistration: execute", "[integration][org.custusx.registration.method.centerline]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	storageFixture.loadSession1();//Create test patient folder

    CenterlineRegistrationPtr centerlineRegistration;
    centerlineRegistration = CenterlineRegistrationPtr(new cx::CenterlineRegistration());
    REQUIRE(centerlineRegistration);

    centerlineRegistration->UpdateScales(true, true, true, true, true, true);

    QString filenameCenterline1 = cx::DataLocations::getTestDataPath()+"/testing/Centerline/US_aneurism_cl_size0.vtk";
    QString filenameCenterline2 = cx::DataLocations::getTestDataPath()+"/testing/Centerline/US_aneurism_cl_size1.vtk";

	QString info;
	cx::DataPtr dataCenterline1 = storageFixture.mServices->patient()->importData(filenameCenterline1, info);
	cx::DataPtr dataCenterline2 = storageFixture.mServices->patient()->importData(filenameCenterline2, info);

    REQUIRE(dataCenterline1);
    REQUIRE(dataCenterline2);

    cx::MeshPtr mesh1 = boost::dynamic_pointer_cast<cx::Mesh>(dataCenterline1);
    cx::MeshPtr mesh2 = boost::dynamic_pointer_cast<cx::Mesh>(dataCenterline2);

    vtkPointsPtr centerlinePoints1 = centerlineRegistration->processCenterline(mesh1->getVtkPolyData(), cx::Transform3D::Identity());
    centerlineRegistration->SetFixedPoints(centerlinePoints1);

    vtkPointsPtr centerlinePoints2 = centerlineRegistration->processCenterline(mesh2->getVtkPolyData(), cx::Transform3D::Identity());
    centerlineRegistration->SetMovingPoints(centerlinePoints2);

    cx::Transform3D rMpr = centerlineRegistration->FullRegisterMoving(cx::Transform3D::Identity());
    REQUIRE(rMpr.data());
}


namespace
{

vtkPointsPtr createCurve()
{
	vtkPointsPtr retval = vtkPointsPtr::New();
	for (int i = 0; i < 400; ++i)
	{
		const double t = i * 0.05;
		retval->InsertNextPoint(30*std::cos(t), 20*std::sin(1.3*t), 4*t);
	}
	return retval;
}

vtkPointsPtr transformedSubset(vtkPointsPtr points, cx::Transform3D transform)
{
	vtkPointsPtr retval = vtkPointsPtr::New();
	for (vtkIdType i = 20; i < points->GetNumberOfPoints() - 20; i += 3)
	{
		double p[3];
		points->GetPoint(i, p);
		const cx::Vector3D moved = transform.coord(cx::Vector3D(p[0], p[1], p[2]));
		retval->InsertNextPoint(moved[0], moved[1], moved[2]);
	}
	return retval;
}

cx::Transform3D registerCurve(vtkPointsPtr fixed, vtkPointsPtr moving, const bool* freeParameters, cx::Transform3D init)
{
	cx::CenterlineRegistration registration;
	registration.UpdateScales(freeParameters[0], freeParameters[1], freeParameters[2], freeParameters[3], freeParameters[4], freeParameters[5]);
	registration.SetFixedPoints(fixed);
	registration.SetMovingPoints(moving);
	return registration.FullRegisterMoving(init);
}

} // namespace

TEST_CASE("CenterlineRegistration: finds a rigid transform of tracked positions onto the centerline", "[unit][org.custusx.registration.method.centerline]")
{
	vtkPointsPtr fixed = createCurve();
	const cx::Transform3D truth = cx::createTransformTranslate(cx::Vector3D(3, -4, 2)) * cx::createTransformRotateZ(0.08) * cx::createTransformRotateX(-0.05);
	vtkPointsPtr moving = transformedSubset(fixed, truth.inv());
	const bool all[6] = {true, true, true, true, true, true};

	cx::Transform3D result = registerCurve(fixed, moving, all, cx::Transform3D::Identity());

	INFO("Result: " << result << ", expected: " << truth);
	CHECK(cx::similar(result, truth, 0.01));
}

TEST_CASE("CenterlineRegistration: locked parameters keep their initial value", "[unit][org.custusx.registration.method.centerline]")
{
	vtkPointsPtr fixed = createCurve();
	const cx::Transform3D truth = cx::createTransformTranslate(cx::Vector3D(2, 1, -3)) * cx::createTransformRotateZ(0.05);
	vtkPointsPtr moving = transformedSubset(fixed, truth.inv());
	const bool translationOnly[6] = {false, false, false, true, true, true};
	const bool noZ[6] = {true, true, false, true, true, false};

	cx::Transform3D translated = registerCurve(fixed, moving, translationOnly, cx::Transform3D::Identity());
	cx::Transform3D rotationPart = translated;
	rotationPart.translation() = cx::Vector3D::Zero();
	CHECK(cx::similar(rotationPart, cx::Transform3D::Identity()));

	cx::Transform3D init = cx::createTransformTranslate(cx::Vector3D(0, 0, 1.5));
	cx::Transform3D withoutZ = registerCurve(fixed, moving, noZ, init);
	INFO("Result: " << withoutZ);
	CHECK(withoutZ.translation()[2] == Approx(1.5));
	CHECK(std::atan2(-withoutZ(0,1), withoutZ(1,1)) == Approx(0).epsilon(1e-9));
}

TEST_CASE("CenterlineRegistration: without points the initial transform is returned", "[unit][org.custusx.registration.method.centerline]")
{
	cx::CenterlineRegistration registration;
	registration.SetFixedPoints(createCurve());
	registration.SetMovingPoints(vtkPointsPtr::New());
	const cx::Transform3D init = cx::createTransformTranslate(cx::Vector3D(1, 2, 3)) * cx::createTransformRotateY(0.3);
	CHECK(cx::similar(registration.FullRegisterMoving(init), init));
}


namespace
{

vtkPolyDataPtr toPolyData(vtkPointsPtr points)
{
	vtkPolyDataPtr retval = vtkPolyDataPtr::New();
	retval->SetPoints(points);
	return retval;
}

/** Tracked tool positions prMt along the middle part of the centerline, with noise. */
cx::TimedTransformMap trackPositions(vtkPointsPtr centerline_d, cx::Transform3D prMd, double noise)
{
	std::mt19937 rng(17);
	std::normal_distribution<double> gaussian(0, noise);
	cx::TimedTransformMap retval;
	for (vtkIdType i = 40; i < centerline_d->GetNumberOfPoints() - 40; i += 2)
	{
		double p[3];
		centerline_d->GetPoint(i, p);
		cx::Vector3D position = prMd.coord(cx::Vector3D(p[0], p[1], p[2]));
		position += cx::Vector3D(gaussian(rng), gaussian(rng), gaussian(rng));
		retval[i*10.0] = cx::createTransformTranslate(position);
	}
	return retval;
}

double meanError(const cx::TimedTransformMap& prMt, cx::Transform3D rMpr, cx::Transform3D true_rMpr)
{
	double sum = 0;
	for (cx::TimedTransformMap::const_iterator iter = prMt.begin(); iter != prMt.end(); ++iter)
	{
		const cx::Vector3D position = iter->second.translation();
		sum += (rMpr.coord(position) - true_rMpr.coord(position)).norm();
	}
	return sum / prMt.size();
}

double registrationError(cx::Transform3D initialError, double noise)
{
	vtkPointsPtr centerline_d = createCurve();
	const cx::Transform3D rMd = cx::createTransformTranslate(cx::Vector3D(-20, 35, 110)) * cx::createTransformRotateY(0.4);
	const cx::Transform3D true_rMpr = cx::createTransformTranslate(cx::Vector3D(5, -8, 12)) * cx::createTransformRotateX(0.2);
	const cx::TimedTransformMap prMt = trackPositions(centerline_d, true_rMpr.inv() * rMd, noise);
	const cx::Transform3D old_rMpr = initialError * true_rMpr;

	cx::CenterlineRegistration registration;
	const cx::Transform3D delta = registration.runCenterlineRegistration(toPolyData(centerline_d), rMd, prMt, old_rMpr);
	const cx::Transform3D new_rMpr = delta * old_rMpr;
	return meanError(prMt, new_rMpr, true_rMpr);
}

} // namespace

TEST_CASE("CenterlineRegistration: corrects the patient registration from noisy tracked positions", "[unit][org.custusx.registration.method.centerline]")
{
	const cx::Transform3D initialError = cx::createTransformTranslate(cx::Vector3D(4, -3, 2)) * cx::createTransformRotateZ(0.05);
	const double error = registrationError(initialError, 0.5);
	INFO("Mean error after registration: " << error << " mm");
	CHECK(error < 0.3);
}

TEST_CASE("CenterlineRegistration: converges from 10 mm and 10 degrees off", "[unit][org.custusx.registration.method.centerline]")
{
	const cx::Transform3D initialError = cx::createTransformTranslate(cx::Vector3D(7, -5, 5)) * cx::createTransformRotateX(0.12) * cx::createTransformRotateZ(-0.12);
	const double error = registrationError(initialError, 0.5);
	INFO("Mean error after registration: " << error << " mm");
	CHECK(error < 0.3);
}

}; // end cxtest namespace
