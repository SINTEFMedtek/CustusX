/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestSeansVesselRegFixture.h"
#include "catch.hpp"

#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkCellArray.h>
#include "cxMesh.h"
#include "cxVector3D.h"
#include "cxDataLocations.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxRegistrationTransform.h"
#include "cxTypeConversions.h"
#include <QFileInfo>
#include <QDir>
#include "cxFileManagerServiceProxy.h"
#include "cxLogicManager.h"


TEST_CASE_METHOD(cxtest::SeansVesselRegFixture, "SeansVesselReg: V2V syntectic data", "[integration][modules][registration][not_win32]")
//void TestRegistrationV2V::testV2V_synthetic_data()
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	QStringList files = this->generateTestData();

	// accepted tolerances, lo for self-test, hi for differing source and target
//	double tol_dist_low = 1.0E-4;
//	double tol_ang_low = 1.0E-4;
	double tol_dist_low = 1;
	double tol_ang_low = 2/180.0*M_PI;
//	double tol_dist_hi = 5;
//	double tol_ang_hi = 5.0/180.0*M_PI;

	std::vector<cx::Transform3D> pert = this->generateTransforms();

	std::cout << std::endl;

	for (unsigned i = 0; i < pert.size(); ++i)
	{
		this->doTestVessel2VesselRegistration(pert[i], files[0], files[0], tol_dist_low, tol_ang_low, filemanager);
	}
	std::cout << std::endl;

	for (unsigned i = 0; i < pert.size(); ++i)
	{
		this->doTestVessel2VesselRegistration(pert[i], files[1], files[1], tol_dist_low, tol_ang_low, filemanager);
	}
	std::cout << std::endl;
//	for (unsigned i = 0; i < pert.size(); ++i)
//	{
//		this->doTestVessel2VesselRegistration(pert[i], fname1, fname2, tol_dist_hi, tol_ang_hi);
//		this->doTestVessel2VesselRegistration(pert[i], fname2, fname1, tol_dist_hi, tol_ang_hi);
//	}
//	std::cout << std::endl;
//	for (unsigned i = 0; i < pert.size(); ++i)
//	{
//		this->doTestVessel2VesselRegistration(pert[i], fname1, fname3, tol_dist_hi, tol_ang_hi);
//		this->doTestVessel2VesselRegistration(pert[i], fname3, fname1, tol_dist_hi, tol_ang_hi);
//	}
//	std::cout << std::endl;
//	for (unsigned i = 0; i < pert.size(); ++i)
//	{
//		this->doTestVessel2VesselRegistration(pert[i], fname0, fname1, tol_dist_hi, tol_ang_hi);
//		this->doTestVessel2VesselRegistration(pert[i], fname1, fname0, tol_dist_hi, tol_ang_hi);
//	}
	cx::LogicManager::shutdown();
}

TEST_CASE_METHOD(cxtest::SeansVesselRegFixture, "SeansVesselReg: V2V registration", "[integration][modules][registration][not_win32]")
//void TestRegistrationV2V::testVessel2VesselRegistration()
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	QString fname0 = cx::DataLocations::getTestDataPath() + "/testing/Centerline/US_aneurism_cl_size0.vtk";
	QString fname1 = cx::DataLocations::getTestDataPath() + "/testing/Centerline/US_aneurism_cl_size1.vtk";
	QString fname2 = cx::DataLocations::getTestDataPath() + "/testing/Centerline/US_aneurism_cl_size2.vtk";
	QString fname3 = cx::DataLocations::getTestDataPath() + "/testing/Centerline/US_aneurism_cl_size3.vtk";

	// accepted tolerances, lo for self-test, hi for differing source and target
//	double tol_dist_low = 1.0E-4;
//	double tol_ang_low = 1.0E-4;
//	double tol_dist_hi = 5;
//	double tol_ang_hi = 5.0/180.0*M_PI;

	double tol_dist_low = 0.5;
	double tol_ang_low = 1/180.0*M_PI;
	double tol_dist_hi = 1;
	double tol_ang_hi = 2.0/180.0*M_PI;

	// This is a set of perturbations that work on the input dataset.
	// Larger values cause failure.

	cx::Transform3D T_center =cx::createTransformTranslate(cx::Vector3D(-33,-47,-17)); // from inspection of the datasets
	cx::Transform3D T_111 =cx::createTransformTranslate(cx::Vector3D(1,1,1));
	cx::Transform3D T_110 =cx::createTransformTranslate(cx::Vector3D(1,1,0));
	cx::Transform3D T_100 =cx::createTransformTranslate(cx::Vector3D(1,0,0));
	cx::Transform3D T_222 =cx::createTransformTranslate(cx::Vector3D(2,2,2));
	cx::Transform3D R_x5 =cx::createTransformRotateX(5 / 180.0 * M_PI);
	cx::Transform3D R_xy3 =cx::createTransformRotateY(3 / 180.0 * M_PI) *cx::createTransformRotateX(3 / 180.0 * M_PI);
	cx::Transform3D R_xy5 =cx::createTransformRotateY(5 / 180.0 * M_PI) *cx::createTransformRotateX(5 / 180.0 * M_PI);

	std::vector<cx::Transform3D> pert;
	pert.push_back(cx::Transform3D::Identity());
	pert.push_back(T_100);
	pert.push_back(T_110);
	pert.push_back(T_111);
	pert.push_back(T_222);
	pert.push_back(T_center * R_x5 * T_center.inv());
	pert.push_back(T_center * R_xy3 * T_center.inv());
	pert.push_back(T_center * R_xy3 * T_center.inv() * T_111);
	pert.push_back(T_center * R_xy5 * T_center.inv());
	pert.push_back(T_center * R_xy5 * T_center.inv() * T_222);

	// this set causes two failures
	cx::Transform3D T_large1 =cx::createTransformTranslate(cx::Vector3D(3, 3, 3));
	cx::Transform3D T_large2 =cx::createTransformTranslate(cx::Vector3D(0, 0, 5));
	pert.push_back(T_large1);
	pert.push_back(T_large2);
//
//	std::cout << std::endl;
//	this->doTestVessel2VesselRegistration(T_large1, fname1, fname2, tol_dist_hi, tol_ang_hi);
//	this->doTestVessel2VesselRegistration(T_large1, fname2, fname1, tol_dist_hi, tol_ang_hi);

	// causes failure in findClosestPoint -> nan
//	pert.push_back(createTransformRotateY(120 / 180.0 * M_PI) *cx::createTransformRotateX(90 / 180.0 * M_PI) *cx::createTransformTranslate(cx::Vector3D(90, 50, 0)));

	std::cout << std::endl;

	for (unsigned i = 0; i < pert.size(); ++i)
	{
		this->doTestVessel2VesselRegistration(pert[i], fname1, fname1, tol_dist_low, tol_ang_low, filemanager);
	}
	std::cout << std::endl;
	for (unsigned i = 0; i < pert.size(); ++i)
	{
		this->doTestVessel2VesselRegistration(pert[i], fname1, fname2, tol_dist_hi, tol_ang_hi, filemanager);
		this->doTestVessel2VesselRegistration(pert[i], fname2, fname1, tol_dist_hi, tol_ang_hi, filemanager);
	}
	std::cout << std::endl;
	for (unsigned i = 0; i < pert.size(); ++i)
	{
		this->doTestVessel2VesselRegistration(pert[i], fname1, fname3, tol_dist_hi, tol_ang_hi, filemanager);
		this->doTestVessel2VesselRegistration(pert[i], fname3, fname1, tol_dist_hi, tol_ang_hi, filemanager);
	}
	std::cout << std::endl;
	for (unsigned i = 0; i < pert.size(); ++i)
	{
		this->doTestVessel2VesselRegistration(pert[i], fname0, fname1, tol_dist_hi, tol_ang_hi, filemanager);
		this->doTestVessel2VesselRegistration(pert[i], fname1, fname0, tol_dist_hi, tol_ang_hi, filemanager);
	}

	cx::LogicManager::shutdown();
}
