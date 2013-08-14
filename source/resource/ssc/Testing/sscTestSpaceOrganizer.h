#ifndef SSCTESTSPACEORGANIZER_H_
#define SSCTESTSPACEORGANIZER_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "sscSpaceOrganizer.h"

/**Unit tests for class SpaceOrganizer.
 */
class TestSpaceOrganizer : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();
	void testInitialize();
	void testSingleImage();
	void testRelateToWorld();
	void test2Reg1NonReg();
	void testFirstUSThenPOThenRegister();
	void testOldUS();	
	void testInvalidSpaces();
	void testBadSpaces();
	
public:
	CPPUNIT_TEST_SUITE( TestSpaceOrganizer );
		CPPUNIT_TEST( testInitialize );
		CPPUNIT_TEST( testSingleImage );
		CPPUNIT_TEST( testRelateToWorld );
		CPPUNIT_TEST( test2Reg1NonReg );
		CPPUNIT_TEST( testFirstUSThenPOThenRegister );
		CPPUNIT_TEST( testOldUS );
		CPPUNIT_TEST( testInvalidSpaces );
		CPPUNIT_TEST( testBadSpaces );
				
	CPPUNIT_TEST_SUITE_END();
private:
	bool worldTransformIs(const ssc::SpaceOrganizer& boss, ssc::SpaceProperties a, const ssc::Transform3D& wMa);

private:
	QString mSession;
	QString mSessionTimestamp;
	ssc::SpaceProperties s_US0_nr, s_US1_r, s_US2_old,
		s_patientOrientation, s_patient, s_patient_nr, 
		s_MR0_r, s_MR1_r, s_MR2_nr, s_crap_1, s_crap_2, 
		s_patientOrientation_badref, s_patientOrientation_badtime;
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestSpaceOrganizer );

#endif /*NAVTESTSPACEORGANIZER_H_*/
