/*
 * cxTestRegistration.cpp
 *
 *  \date Oct 24, 2011
 *      \author christiana
 */

#include "cxTest_Registration.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "cxDataLocations.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "sscRegistrationTransform.h"
#include "sscTypeConversions.h"
#include <QFileInfo>

void TestRegistration::setUp()
{

}

void TestRegistration::tearDown()
{
	ssc::DataManager::shutdown();
}

void TestRegistration::testVessel2VesselRegistration()
{
	QString fname0 = cx::DataLocations::getTestDataPath() + "/testing/Centerline/US_aneurism_cl_size0.vtk";
	QString fname1 = cx::DataLocations::getTestDataPath() + "/testing/Centerline/US_aneurism_cl_size1.vtk";
	QString fname2 = cx::DataLocations::getTestDataPath() + "/testing/Centerline/US_aneurism_cl_size2.vtk";
	QString fname3 = cx::DataLocations::getTestDataPath() + "/testing/Centerline/US_aneurism_cl_size3.vtk";

	// accepted tolerances, lo for self-test, hi for differing source and target
	double tol_dist_low = 1.0E-4;
	double tol_ang_low = 1.0E-4;
	double tol_dist_hi = 5;
	double tol_ang_hi = 5.0/180.0*M_PI;

	// This is a set of perturbations that work on the input dataset.
	// Larger values cause failure.

	ssc::Transform3D T_center = ssc::createTransformTranslate(ssc::Vector3D(-33,-47,-17)); // from inspection of the datasets
	ssc::Transform3D T_111 = ssc::createTransformTranslate(ssc::Vector3D(1,1,1));
	ssc::Transform3D T_110 = ssc::createTransformTranslate(ssc::Vector3D(1,1,0));
	ssc::Transform3D T_100 = ssc::createTransformTranslate(ssc::Vector3D(1,0,0));
	ssc::Transform3D T_222 = ssc::createTransformTranslate(ssc::Vector3D(2,2,2));
	ssc::Transform3D R_x5 = ssc::createTransformRotateX(5 / 180.0 * M_PI);
	ssc::Transform3D R_xy3 = ssc::createTransformRotateY(3 / 180.0 * M_PI) * ssc::createTransformRotateX(3 / 180.0 * M_PI);
	ssc::Transform3D R_xy5 = ssc::createTransformRotateY(5 / 180.0 * M_PI) * ssc::createTransformRotateX(5 / 180.0 * M_PI);

	std::vector<ssc::Transform3D> pert;
	pert.push_back(ssc::Transform3D::Identity());
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
//	pert.push_back(ssc::createTransformRotateY(120 / 180.0 * M_PI) * ssc::createTransformRotateX(90 / 180.0 * M_PI) * ssc::createTransformTranslate(ssc::Vector3D(90, 50, 0)));

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


void TestRegistration::doTestVessel2VesselRegistration(ssc::Transform3D perturbation, QString filenameSource, QString filenameTarget, double tol_dist, double tol_angle)
{
	ssc::DataPtr source = ssc::dataManager()->loadData("source_"+filenameSource, filenameSource, ssc::rtAUTO);
	ssc::DataPtr target = ssc::dataManager()->loadData("target_"+filenameTarget, filenameTarget, ssc::rtAUTO);
	source->get_rMd_History()->setRegistration(perturbation);
	CPPUNIT_ASSERT(source!=0);
	CPPUNIT_ASSERT(target!=0);

	//Default values
//	int lts_ratio = 80;
	int lts_ratio = 80;
	double stop_delta = 0.001;
	double lambda = 0;
	double sigma = 1.0;
	bool lin_flag = 1;
	int sample = 1;
	int single_point_thre = 1;
	bool verbose = 1;

//	QString info = QString("======== VesselVesselReg pre info:\n\tSource=%1\n\tTarget=%2\n\tPerturbation=\n%3")
//		.arg(filenameSource)
//		.arg(filenameTarget)
//		.arg(qstring_cast(perturbation));
//	std::cout << info << std::endl;

	cx::SeansVesselReg vesselReg(lts_ratio, stop_delta, lambda, sigma, lin_flag, sample, single_point_thre, verbose);
	vesselReg.setDebugOutput(false); // too much information

	bool success = vesselReg.doItRight(source, target, cx::DataLocations::getTestDataPath() + "/Log");
	CPPUNIT_ASSERT(success);

	ssc::Transform3D linearTransform = vesselReg.getLinearTransform();

	QString fail = QString("======== VesselVesselReg info:\n\tSource=%1\n\tTarget=%2\n\tPerturbation=\n%3\nResult^-1=\n%4")
		.arg(filenameSource)
		.arg(filenameTarget)
		.arg(qstring_cast(perturbation))
		.arg(qstring_cast(linearTransform));

	// characterize the difference matrix in angle-axis form:
	ssc::Transform3D diff = linearTransform * perturbation.inv();
	ssc::Vector3D t_delta = diff.matrix().block<3,1>(0, 3);
	Eigen::AngleAxisd aa = Eigen::AngleAxisd(diff.matrix().block<3,3>(0, 0));
	double a = aa.angle();
	ssc::Vector3D axis = aa.axis();

	// characterize the input perturbation in angle-axis form:
	ssc::Transform3D diff_input = perturbation;
	ssc::Vector3D t_delta_input = diff_input.matrix().block<3,1>(0, 3);
	Eigen::AngleAxisd aa_input = Eigen::AngleAxisd(diff_input.matrix().block<3,3>(0, 0));
	double a_input = aa_input.angle();


//	if (ssc::similar(linearTransform, perturbation, tol))
//	{
//		QString suc = QString("Success:\n\tSource=%1\n\tTarget=%2")
//			.arg(filenameSource)
//			.arg(filenameTarget);
//		std::cout << suc << std::endl;
//	}
//	else
//	{
//		std::cout << fail << std::endl;
//	}

	// output a one-line string with essential info suitable for pasting into Excel:
	std::cout << QString("dist=\t%1\tangle=\t%2").arg(t_delta.length(), 6, 'f', 2).arg(a/M_PI*180.0, 6, 'f', 2);
	std::cout << "\t" << QString("pert\tdist=\t%1\tangle=\t%2").arg(t_delta_input.length(), 6, 'f', 2).arg(a_input/M_PI*180.0, 6, 'f', 2);
	std::cout << "\t" << QFileInfo(filenameSource).baseName() << "\t" << QFileInfo(filenameSource).baseName();
	std::cout << std::endl;

	//	CPPUNIT_ASSERT(ssc::similar(linearTransform, perturbation, tol));
	CPPUNIT_ASSERT(t_delta.length() < tol_dist);
	CPPUNIT_ASSERT(fabs(a) < tol_angle);
}


