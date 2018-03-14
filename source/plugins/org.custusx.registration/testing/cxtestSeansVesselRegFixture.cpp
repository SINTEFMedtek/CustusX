/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <cxtestSeansVesselRegFixture.h>

#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkCellArray.h>
#include "cxMesh.h"
#include "cxDataLocations.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxRegistrationTransform.h"
#include "cxTypeConversions.h"
#include <QDir>
#include "cxtestPatientModelServiceMock.h"
#include "cxReporter.h"

#include "catch.hpp"

namespace cxtest
{
SeansVesselRegFixture::SeansVesselRegFixture()
{
	this->setUp();
}

SeansVesselRegFixture::~SeansVesselRegFixture()
{
	this->tearDown();
}

void SeansVesselRegFixture::setUp()
{
	cx::DataLocations::setTestMode();
	cx::Reporter::initialize();
}

void SeansVesselRegFixture::tearDown()
{
	cx::Reporter::shutdown();
}

/**return endpoint
 *
 */
cx::Vector3D SeansVesselRegFixture::append_line(std::vector<cx::Vector3D>* pts,
		cx::Vector3D a, cx::Vector3D b, double spacing)
{
	cx::Vector3D u = (b - a).normal();
	unsigned N = (unsigned) floor((b - a).length() / spacing + 0.5);
	for (unsigned i = 0; i < N; ++i)
		pts->push_back(a + u * spacing * (i + 1));
	return b;
}

/**return endpoint
 *
 */
cx::Vector3D SeansVesselRegFixture::append_pt(std::vector<cx::Vector3D>* pts,
		cx::Vector3D a)
{
	pts->push_back(a);
	return a;
}

QStringList SeansVesselRegFixture::generateTestData()
{
	QString path = cx::DataLocations::getTestDataPath() + "/temp/elastix/";
	QDir().mkpath(path);
	QStringList retval;
	double spacing = 0.1;
	std::vector<cx::Vector3D> pts;
	cx::Vector3D a, b;

	// an Y-fork
	spacing = 0.1;
	a = this->append_pt(&pts, cx::Vector3D(0, 0, 0));
	a = this->append_line(&pts, a, cx::Vector3D(0, 0, 10), spacing);
	b = this->append_line(&pts, a, cx::Vector3D(-3, 0, 15), spacing);
	b = this->append_line(&pts, a, cx::Vector3D(3, 0, 15), spacing);
	retval << this->saveVTKFile(pts, path + "test1.vtk");
	this->saveVTKFile(pts, path + "test1_b.vtk");
	//-----------------------------------------------------

	// The Y-fork, with a bend on the beginning.
	pts.clear();
	spacing = 0.1;
	a = this->append_pt(&pts, cx::Vector3D(0, 5, -2));
	a = this->append_line(&pts, a, cx::Vector3D(0, 0, 0), spacing);
	a = this->append_line(&pts, a, cx::Vector3D(0, 0, 10), spacing);
	b = this->append_line(&pts, a, cx::Vector3D(-3, 0, 15), spacing);
	b = this->append_line(&pts, a, cx::Vector3D(3, 0, 15), spacing);
	// a bent line 5 above the Y-fork.
	a = this->append_pt(&pts, cx::Vector3D(-5, -5, 0));
	a = this->append_line(&pts, a, cx::Vector3D(-5, 5, 8), spacing);
	a = this->append_line(&pts, a, cx::Vector3D(-5, 5, 12), spacing);
	retval << this->saveVTKFile(pts, path + "test2.vtk");
	this->saveVTKFile(pts, path + "test2_b.vtk");
	//-----------------------------------------------------

	return retval;
}

QString SeansVesselRegFixture::saveVTKFile(std::vector<cx::Vector3D> pts,
		QString filename)
{
	vtkPolyDataPtr poly = this->generatePolyData(pts);
	this->saveVTKFile(poly, filename);
	return filename;
}

vtkPolyDataPtr SeansVesselRegFixture::generatePolyData(
		std::vector<cx::Vector3D> pts)
{
	vtkPolyDataPtr mPolyData = vtkPolyDataPtr::New();
	vtkPointsPtr mPoints = vtkPointsPtr::New();
	vtkCellArrayPtr mLines = vtkCellArrayPtr::New();
	mPolyData->SetPoints(mPoints);
//	mPolyData->SetLines(mLines);
	mPolyData->SetVerts(mLines);

	for (unsigned i = 0; i < pts.size(); ++i)
	{
		mPoints->InsertNextPoint(pts[i].begin());
	}

	// fill cell points for the entire polydata.
	mLines->Initialize();
	std::vector<vtkIdType> ids(mPoints->GetNumberOfPoints());
	for (unsigned i = 0; i < ids.size(); ++i)
		ids[i] = i;
	mLines->InsertNextCell(ids.size(), &(*ids.begin()));

	mPolyData->Modified();
	return mPolyData;
}

void SeansVesselRegFixture::saveVTKFile(vtkPolyDataPtr data, QString filename)
{
	vtkPolyDataWriterPtr writer = vtkPolyDataWriterPtr::New();
	writer->SetInputData(data);
	writer->SetFileName(cstring_cast(filename));
	writer->Update();
	writer->Write();
}

std::vector<cx::Transform3D> SeansVesselRegFixture::generateTransforms()
{
	// This is a set of perturbations that work on the input dataset.
	// Larger values cause failure.

	cx::Transform3D T_center = cx::createTransformTranslate(
			cx::Vector3D(-33, -47, -17)); // from inspection of the datasets
	cx::Transform3D T_111 = cx::createTransformTranslate(cx::Vector3D(1, 1, 1));
	cx::Transform3D T_110 = cx::createTransformTranslate(cx::Vector3D(1, 1, 0));
	cx::Transform3D T_100 = cx::createTransformTranslate(cx::Vector3D(1, 0, 0));
	cx::Transform3D T_222 = cx::createTransformTranslate(cx::Vector3D(2, 2, 2));
	cx::Transform3D R_x5 = cx::createTransformRotateX(5 / 180.0 * M_PI);
	cx::Transform3D R_xy3 = cx::createTransformRotateY(3 / 180.0 * M_PI)
			* cx::createTransformRotateX(3 / 180.0 * M_PI);
	cx::Transform3D R_xy5 = cx::createTransformRotateY(5 / 180.0 * M_PI)
			* cx::createTransformRotateX(5 / 180.0 * M_PI);

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

	// causes failure in findClosestPoint -> nan
//	pert.push_back(createTransformRotateY(120 / 180.0 * M_PI) *cx::createTransformRotateX(90 / 180.0 * M_PI) *cx::createTransformTranslate(cx::Vector3D(90, 50, 0)));

	return pert;
}

void SeansVesselRegFixture::doTestVessel2VesselRegistration(
		cx::Transform3D perturbation, QString filenameSource,
		QString filenameTarget, double tol_dist, double tol_angle)
{
	bool verbose = 0;

	if (verbose)
	{
		std::cout << "======== VesselVesselReg start: =============="
				<< std::endl;
		std::cout << "======== ====================== =============="
				<< std::endl;
	}

	PatientModelServiceMock pasm;

	QString dummy;
	cx::DataPtr source = pasm.importData(filenameSource, dummy);
	cx::DataPtr target = pasm.importData(filenameTarget, dummy);
	CHECK(source);
	CHECK(target);
	source->get_rMd_History()->setRegistration(perturbation);

	if (false)
	{
		QString info =
				QString(
						"======== VesselVesselReg pre info: ==============\n\tSource=%1\n\tTarget=%2\n\tPerturbation=\n%3").arg(
						filenameSource).arg(filenameTarget).arg(
						qstring_cast(perturbation));
		std::cout << std::endl << std::endl;
		std::cout << info << std::endl;
	}

	cx::SeansVesselReg vesselReg;
	vesselReg.setDebugOutput(verbose);
	vesselReg.mt_doOnlyLinear = true;

	bool success = vesselReg.initialize(source, target,
			cx::DataLocations::getTestDataPath() + "/Log");
	success = success && vesselReg.execute();
	CHECK(success);

	cx::Transform3D linearTransform = vesselReg.getLinearResult();

	// characterize the difference matrix in angle-axis form:
	cx::Transform3D diff = linearTransform * perturbation.inv();
	cx::Vector3D t_delta = diff.matrix().block<3, 1>(0, 3);
	Eigen::AngleAxisd aa = Eigen::AngleAxisd(diff.matrix().block<3, 3>(0, 0));
	double a = aa.angle();
	cx::Vector3D axis = aa.axis();

	// characterize the input perturbation in angle-axis form:
	cx::Transform3D diff_input = perturbation;
	cx::Vector3D t_delta_input = diff_input.matrix().block<3, 1>(0, 3);
	Eigen::AngleAxisd aa_input = Eigen::AngleAxisd(
			diff_input.matrix().block<3, 3>(0, 0));
	double a_input = aa_input.angle();

	if (verbose)
	{

		if ((t_delta.length() < tol_dist) && (fabs(a) < tol_angle))
		{
//			QString suc = QString("Success:\n\tSource=%1\n\tTarget=%2")
//				.arg(filenameSource)
//				.arg(filenameTarget);
//			std::cout << suc << std::endl;
//			std::cout << "\tSuccess" << std::endl;
		}
//		else
		{
			std::cout << std::endl;
			std::cout << "======== VesselVesselReg info: =============="
					<< std::endl;
			QString info1 = QString("\t\tSource=%1\n\t\tTarget=%2").arg(
					filenameSource).arg(filenameTarget);
			std::cout << info1 << std::endl;
			std::cout << "\tFAILED" << std::endl;
			QString info2 = QString("\tPerturbation=\n%1").arg(
					qstring_cast(perturbation));
			std::cout << info2 << std::endl;

			QString fail = QString("\tResult^-1=\n%1").arg(
					qstring_cast(linearTransform));
			std::cout << fail << std::endl;
		}
	}

	// output a one-line string with essential info suitable for pasting into Excel:
	std::cout
			<< QString("dist=\t%1\tangle=\t%2").arg(t_delta.length(), 6, 'f', 2).arg(
					a / M_PI * 180.0, 6, 'f', 2);
	std::cout << "\t"
			<< QString("pert\tdist=\t%1\tangle=\t%2").arg(
					t_delta_input.length(), 6, 'f', 2).arg(
					a_input / M_PI * 180.0, 6, 'f', 2);
	std::cout << "\t" << QFileInfo(filenameSource).baseName() << "\t"
			<< QFileInfo(filenameTarget).baseName();
	std::cout << std::endl;

	if (verbose)
	{
		std::cout << "======== ====================== =============="
				<< std::endl;
		std::cout << "======== VesselVesselReg complete: =============="
				<< std::endl;
	}

//	REQUIRE(t_delta.length() < tol_dist);
//	REQUIRE(fabs(a) < tol_angle);
}
} /* namespace cxtest */
