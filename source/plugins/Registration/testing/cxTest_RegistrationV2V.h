/*
 * cxTestRegistrationV2V.h
 *
 *  \date Oct 24, 2011
 *      \author christiana
 */

#ifndef CXTEST_REGISTRATIONV2V_H_
#define CXTEST_REGISTRATIONV2V_H_

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Message.h>

#include <vector>
#include "sscTransform3D.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

/**
 */
class TestRegistrationV2V : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testVessel2VesselRegistration();
	void testV2V_synthetic_data();

public:
	CPPUNIT_TEST_SUITE( TestRegistrationV2V );
		CPPUNIT_TEST( testVessel2VesselRegistration );
		CPPUNIT_TEST( testV2V_synthetic_data );
	CPPUNIT_TEST_SUITE_END();
private:
	void doTestVessel2VesselRegistration(ssc::Transform3D perturbation, QString filenameSource, QString filenameTarget, double tol_dist, double tol_angle);
	vtkPolyDataPtr generatePolyData(std::vector<ssc::Vector3D> pts);
	QStringList generateTestData();
	void saveVTKFile(vtkPolyDataPtr data, QString filename);
	QString saveVTKFile(std::vector<ssc::Vector3D>, QString filename);
	ssc::Vector3D append_line(std::vector<ssc::Vector3D>* pts, ssc::Vector3D a, ssc::Vector3D b, double spacing);
	ssc::Vector3D append_pt(std::vector<ssc::Vector3D>* pts, ssc::Vector3D a);
	std::vector<ssc::Transform3D> generateTransforms();
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestRegistrationV2V );


#endif /* CXTEST_REGISTRATIONV2V_H_ */
