/*
 * cxTestRegistrationV2V.cpp
 *
 *  \date Oct 24, 2011
 *      \author christiana
 */

#include "cxTest_RegistrationV2V.h"

#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkCellArray.h>
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscVector3D.h"
#include "cxDataLocations.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "sscRegistrationTransform.h"
#include "sscTypeConversions.h"
#include <QFileInfo>
#include <QDir>

void TestRegistrationV2V::setUp()
{
	cx::DataLocations::setTestMode();

}

void TestRegistrationV2V::tearDown()
{
	cx::DataManager::shutdown();
}

/**return endpoint
 *
 */
cx::Vector3D TestRegistrationV2V::append_line(std::vector<cx::Vector3D>* pts, cx::Vector3D a, cx::Vector3D b, double spacing)
{
	cx::Vector3D u = (b-a).normal();
	unsigned N = (unsigned)floor((b-a).length()/spacing + 0.5);
	for (unsigned i=0; i<N; ++i)
        pts->push_back(a+u*spacing*(i+1));
	return b;
}

/**return endpoint
 *
 */
cx::Vector3D TestRegistrationV2V::append_pt(std::vector<cx::Vector3D>* pts, cx::Vector3D a)
{
	pts->push_back(a);
	return a;
}

QStringList TestRegistrationV2V::generateTestData()
{
	QString path = cx::DataLocations::getTestDataPath() + "/temp/elastix/";
	QDir().mkpath(path);
	QStringList retval;
	double spacing = 0.1;
	std::vector<cx::Vector3D> pts;
	cx::Vector3D a, b;

	// an Y-fork
	spacing = 0.1;
	a = this->append_pt(&pts, cx::Vector3D(0,0,0));
	a = this->append_line(&pts, a, cx::Vector3D(0,0,10), spacing);
	b = this->append_line(&pts, a, cx::Vector3D(-3,0,15), spacing);
	b = this->append_line(&pts, a, cx::Vector3D( 3,0,15), spacing);
	retval << this->saveVTKFile(pts, path + "test1.vtk");
	this->saveVTKFile(pts, path + "test1_b.vtk");
	//-----------------------------------------------------

	// The Y-fork, with a bend on the beginning.
	pts.clear();
	spacing = 0.1;
	a = this->append_pt(&pts, cx::Vector3D(0,5,-2));
	a = this->append_line(&pts, a, cx::Vector3D(0,0,0), spacing);
	a = this->append_line(&pts, a, cx::Vector3D(0,0,10), spacing);
	b = this->append_line(&pts, a, cx::Vector3D(-3,0,15), spacing);
	b = this->append_line(&pts, a, cx::Vector3D( 3,0,15), spacing);
	// a bent line 5 above the Y-fork.
	a = this->append_pt(&pts, cx::Vector3D(-5,-5,0));
	a = this->append_line(&pts, a, cx::Vector3D(-5,5,8), spacing);
	a = this->append_line(&pts, a, cx::Vector3D(-5,5,12), spacing);
	retval << this->saveVTKFile(pts, path + "test2.vtk");
	this->saveVTKFile(pts, path + "test2_b.vtk");
	//-----------------------------------------------------

	return retval;
}

QString TestRegistrationV2V::saveVTKFile(std::vector<cx::Vector3D> pts, QString filename)
{
	vtkPolyDataPtr poly = this->generatePolyData(pts);
	this->saveVTKFile(poly, filename);
	return filename;
}

vtkPolyDataPtr TestRegistrationV2V::generatePolyData(std::vector<cx::Vector3D> pts)
{
	vtkPolyDataPtr mPolyData = vtkPolyDataPtr::New();
	vtkPointsPtr mPoints = vtkPointsPtr::New();
	vtkCellArrayPtr mLines = vtkCellArrayPtr::New();
	mPolyData->SetPoints(mPoints);
//	mPolyData->SetLines(mLines);
	mPolyData->SetVerts(mLines);

	for (unsigned i=0; i<pts.size(); ++i)
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

void TestRegistrationV2V::saveVTKFile(vtkPolyDataPtr data, QString filename)
{
	vtkPolyDataWriterPtr writer = vtkPolyDataWriterPtr::New();
	writer->SetInput(data);
	writer->SetFileName(cstring_cast(filename));
	writer->Update();
	writer->Write();
}


std::vector<cx::Transform3D> TestRegistrationV2V::generateTransforms()
{
	// This is a set of perturbations that work on the input dataset.
	// Larger values cause failure.

	cx::Transform3D T_center = cx::createTransformTranslate(cx::Vector3D(-33,-47,-17)); // from inspection of the datasets
	cx::Transform3D T_111 = cx::createTransformTranslate(cx::Vector3D(1,1,1));
	cx::Transform3D T_110 = cx::createTransformTranslate(cx::Vector3D(1,1,0));
	cx::Transform3D T_100 = cx::createTransformTranslate(cx::Vector3D(1,0,0));
	cx::Transform3D T_222 = cx::createTransformTranslate(cx::Vector3D(2,2,2));
	cx::Transform3D R_x5 = cx::createTransformRotateX(5 / 180.0 * M_PI);
	cx::Transform3D R_xy3 = cx::createTransformRotateY(3 / 180.0 * M_PI) * cx::createTransformRotateX(3 / 180.0 * M_PI);
	cx::Transform3D R_xy5 = cx::createTransformRotateY(5 / 180.0 * M_PI) * cx::createTransformRotateX(5 / 180.0 * M_PI);

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

void TestRegistrationV2V::testV2V_synthetic_data()
{
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
		this->doTestVessel2VesselRegistration(pert[i], files[0], files[0], tol_dist_low, tol_ang_low);
	}
	std::cout << std::endl;

	for (unsigned i = 0; i < pert.size(); ++i)
	{
		this->doTestVessel2VesselRegistration(pert[i], files[1], files[1], tol_dist_low, tol_ang_low);
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
}

void TestRegistrationV2V::testVessel2VesselRegistration()
{
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
		this->doTestVessel2VesselRegistration(pert[i], fname1, fname1, tol_dist_low, tol_ang_low);
	}
	std::cout << std::endl;
	for (unsigned i = 0; i < pert.size(); ++i)
	{
		this->doTestVessel2VesselRegistration(pert[i], fname1, fname2, tol_dist_hi, tol_ang_hi);
		this->doTestVessel2VesselRegistration(pert[i], fname2, fname1, tol_dist_hi, tol_ang_hi);
	}
	std::cout << std::endl;
	for (unsigned i = 0; i < pert.size(); ++i)
	{
		this->doTestVessel2VesselRegistration(pert[i], fname1, fname3, tol_dist_hi, tol_ang_hi);
		this->doTestVessel2VesselRegistration(pert[i], fname3, fname1, tol_dist_hi, tol_ang_hi);
	}
	std::cout << std::endl;
	for (unsigned i = 0; i < pert.size(); ++i)
	{
		this->doTestVessel2VesselRegistration(pert[i], fname0, fname1, tol_dist_hi, tol_ang_hi);
		this->doTestVessel2VesselRegistration(pert[i], fname1, fname0, tol_dist_hi, tol_ang_hi);
	}

}


void TestRegistrationV2V::doTestVessel2VesselRegistration(cx::Transform3D perturbation, QString filenameSource, QString filenameTarget, double tol_dist, double tol_angle)
{
	bool verbose = 0;

	if (verbose)
	{
		std::cout << "======== VesselVesselReg start: ==============" << std::endl;
		std::cout << "======== ====================== ==============" << std::endl;
	}

	cx::DataPtr source = cx::dataManager()->loadData("source_"+filenameSource, filenameSource, cx::rtAUTO);
	cx::DataPtr target = cx::dataManager()->loadData("target_"+filenameTarget, filenameTarget, cx::rtAUTO);
	CPPUNIT_ASSERT(source!=0);
	CPPUNIT_ASSERT(target!=0);
	source->get_rMd_History()->setRegistration(perturbation);


	if (false)
	{
		QString info = QString("======== VesselVesselReg pre info: ==============\n\tSource=%1\n\tTarget=%2\n\tPerturbation=\n%3")
			.arg(filenameSource)
			.arg(filenameTarget)
			.arg(qstring_cast(perturbation));
		std::cout << std::endl << std::endl;
		std::cout << info << std::endl;
	}

	cx::SeansVesselReg vesselReg;
	vesselReg.setDebugOutput(verbose);
	vesselReg.mt_doOnlyLinear = true;

	bool success = vesselReg.execute(source, target, cx::DataLocations::getTestDataPath() + "/Log");
	CPPUNIT_ASSERT(success);

	cx::Transform3D linearTransform = vesselReg.getLinearResult();

	// characterize the difference matrix in angle-axis form:
	cx::Transform3D diff = linearTransform * perturbation.inv();
	cx::Vector3D t_delta = diff.matrix().block<3,1>(0, 3);
	Eigen::AngleAxisd aa = Eigen::AngleAxisd(diff.matrix().block<3,3>(0, 0));
	double a = aa.angle();
	cx::Vector3D axis = aa.axis();

	// characterize the input perturbation in angle-axis form:
	cx::Transform3D diff_input = perturbation;
	cx::Vector3D t_delta_input = diff_input.matrix().block<3,1>(0, 3);
	Eigen::AngleAxisd aa_input = Eigen::AngleAxisd(diff_input.matrix().block<3,3>(0, 0));
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
			std::cout << "======== VesselVesselReg info: ==============" << std::endl;
			QString info1 = QString("\t\tSource=%1\n\t\tTarget=%2")
				.arg(filenameSource)
				.arg(filenameTarget);
			std::cout << info1 << std::endl;
			std::cout << "\tFAILED" << std::endl;
			QString info2 = QString("\tPerturbation=\n%1")
				.arg(qstring_cast(perturbation));
			std::cout << info2 << std::endl;

			QString fail = QString("\tResult^-1=\n%1")
				.arg(qstring_cast(linearTransform));
			std::cout << fail << std::endl;
		}
	}

	// output a one-line string with essential info suitable for pasting into Excel:
	std::cout << QString("dist=\t%1\tangle=\t%2").arg(t_delta.length(), 6, 'f', 2).arg(a/M_PI*180.0, 6, 'f', 2);
	std::cout << "\t" << QString("pert\tdist=\t%1\tangle=\t%2").arg(t_delta_input.length(), 6, 'f', 2).arg(a_input/M_PI*180.0, 6, 'f', 2);
	std::cout << "\t" << QFileInfo(filenameSource).baseName() << "\t" << QFileInfo(filenameTarget).baseName();
	std::cout << std::endl;

	if (verbose)
	{
		std::cout << "======== ====================== ==============" << std::endl;
		std::cout << "======== VesselVesselReg complete: ==============" << std::endl;
	}

//	CPPUNIT_ASSERT(t_delta.length() < tol_dist);
//	CPPUNIT_ASSERT(fabs(a) < tol_angle);
}


