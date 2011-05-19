#include "sscTestSpaceOrganizer.h"

#include <math.h>

using namespace ssc;

void TestSpaceOrganizer::setUp()
{
	mSession = "20081111T120000";
	mSessionTimestamp = mSession;
	
	Transform3D M0 = Transform3D::Identity();
	Transform3D M1 = createTransformTranslate(Vector3D(30,40,50))*createTransformRotateX(M_PI/4.0);
	Transform3D M2 = createTransformTranslate(Vector3D(35,-20,0))*createTransformRotateY(M_PI/8.0);
	Transform3D M3 = createTransformTranslate(Vector3D(315,-200,100))*createTransformRotateZ(-M_PI/8.0);
	Transform3D M4 = createTransformTranslate(Vector3D(0,0,0))*createTransformRotateZ(M_PI/5);

	s_US0_nr		= SpaceProperties("US0 NonReg", "patient", M1, "20081111T120001", false, true); // created right after session start
	s_US1_r			= SpaceProperties("US1 Reg", "global",     M1, "20081111T120101", false, true);
	s_US2_old		= SpaceProperties("US2 Reg", "global",     M1, "20081111T110000", false, true); // created before session start (impossible)
	s_patientOrientation = SpaceProperties("patientOrientation", "patient", M4, "20081111T120100", true, false);
	s_patient		= SpaceProperties("patient", "global",     M1, "20081111T120100", false, false);
	s_patient_nr	= SpaceProperties("patient", "global",     M0, "20081111T120100", false, false);
	s_MR0_r			= SpaceProperties("MR0 Reg", "global",     M2, "20081111T120100", true, false);
	s_MR1_r			= SpaceProperties("MR1 Reg", "global",     M3, "20081111T120101", true, false);
	s_MR2_nr		= SpaceProperties("MR1 NonReg", "none",    M0, "20081111T120102", true, false);
	
	s_patientOrientation_badref = SpaceProperties("patientOrientation", "nothingAtAll", M4, "20081111T120100", true, false);
	s_patientOrientation_badtime = SpaceProperties("patientOrientation", "nothingAtAll", M4, "20081111T120100", true, false);
	s_crap_1			= SpaceProperties("Crap 1", "floink",    M0, "200HappyNewYear!1111T120102", true, false);
	s_crap_2			= SpaceProperties("Crap 2", "global",    M0, "200HappyNewYear!1111T120102", false, true);
}

void TestSpaceOrganizer::tearDown()
{
}

void TestSpaceOrganizer::testInitialize()
{
//		QSettings settings("/Data/test.ini", QSettings::IniFormat);
//		QStringList a;
//		a.push_back("hello");
//		a.push_back("file");
//		settings.setValue("alpha", a);
	
	SpaceOrganizer boss(mSession, mSessionTimestamp);
}
void TestSpaceOrganizer::testSingleImage()
{
	SpaceOrganizer boss(mSession, mSessionTimestamp);

	// add a single registrated space but nothing else
	boss.addSpace(s_MR0_r);
	CPPUNIT_ASSERT( boss.worldSpace() == s_MR0_r.mUid ); 
	CPPUNIT_ASSERT( boss.isAligned(s_MR0_r.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR0_r.mUid) );
	CPPUNIT_ASSERT( ! boss.isNavigable(s_MR0_r.mUid) );
	
	// add the patient orientation, enables navigation
	boss.addSpace(s_patient);
	CPPUNIT_ASSERT( boss.worldSpace() == s_MR0_r.mUid ); 
	CPPUNIT_ASSERT( boss.isAligned(s_MR0_r.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR0_r.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_MR0_r.mUid) );
	
}	

void TestSpaceOrganizer::testRelateToWorld()
{
	SpaceOrganizer boss(mSession, mSessionTimestamp);
	// add a single registrated space but nothing else
	boss.addSpace(s_MR0_r);
	// add the patient orientation, enables navigation
	boss.addSpace(s_patient);
	
	CPPUNIT_ASSERT(worldTransformIs(boss, s_MR0_r,   Transform3D::Identity() )); // in its own ref space
	CPPUNIT_ASSERT(worldTransformIs(boss, s_patient, s_MR0_r.m_rMs.inv() * s_patient.m_rMs )); 		
}

void TestSpaceOrganizer::test2Reg1NonReg()
{
	SpaceOrganizer boss(mSession, mSessionTimestamp);

	// add 2 registrated and 1 nonregistrated image
	boss.addSpace(s_MR0_r);
	boss.addSpace(s_MR1_r);
	boss.addSpace(s_MR2_nr);
	boss.addSpace(s_patient);

	CPPUNIT_ASSERT( boss.worldSpace() == s_MR0_r.mUid ); 
	CPPUNIT_ASSERT( boss.isAligned(s_MR0_r.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR0_r.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_MR0_r.mUid) ); 	

	CPPUNIT_ASSERT( boss.isAligned(s_MR1_r.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR1_r.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_MR1_r.mUid) ); 	

	CPPUNIT_ASSERT( boss.isAligned(s_MR2_nr.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR2_nr.mUid) );
	CPPUNIT_ASSERT( ! boss.isNavigable(s_MR2_nr.mUid) ); 	
}	

/**Test that known spaces with bad data are handled
 */
void TestSpaceOrganizer::testBadSpaces()
{
	SpaceOrganizer boss(mSession, mSessionTimestamp);

	// add 1 us nonreg ->check
	boss.addSpace(s_US0_nr);

	CPPUNIT_ASSERT( boss.worldSpace() == "patient" ); 
	CPPUNIT_ASSERT( !boss.isAligned(s_US0_nr.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_US0_nr.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_US0_nr.mUid) ); 	

	// BAD patient orient ->check
	boss.addSpace(s_patientOrientation_badref);		

	CPPUNIT_ASSERT( boss.worldSpace() == s_patientOrientation.mUid ); 
	CPPUNIT_ASSERT( !boss.isAligned(s_US0_nr.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_patientOrientation_badref.mUid) );
	CPPUNIT_ASSERT( !boss.isNavigable(s_US0_nr.mUid) ); 	

	// BAD patient orient ->check
	boss.addSpace(s_patientOrientation_badtime); // replaces old pator space		

	CPPUNIT_ASSERT( boss.worldSpace() == s_patientOrientation.mUid ); 
	CPPUNIT_ASSERT( !boss.isAligned(s_US0_nr.mUid) ); 
	CPPUNIT_ASSERT( !boss.isNavigable(s_US0_nr.mUid) ); 		
}

/**Same as test2Reg1NonReg, but add two invalid spaces and assert class is stable.
 * 
 */
void TestSpaceOrganizer::testInvalidSpaces()
{
	SpaceOrganizer boss(mSession, mSessionTimestamp);

	// add 2 registrated and 1 nonregistrated image
	boss.addSpace(s_MR0_r);
	boss.addSpace(s_MR1_r);
	boss.addSpace(s_MR2_nr);
	boss.addSpace(s_patient);
	
	boss.addSpace(s_crap_1);

	CPPUNIT_ASSERT( boss.worldSpace() == s_MR0_r.mUid ); 
	CPPUNIT_ASSERT( boss.isAligned(s_MR0_r.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR0_r.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_MR0_r.mUid) ); 	

	CPPUNIT_ASSERT( boss.isAligned(s_MR1_r.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR1_r.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_MR1_r.mUid) ); 	

	CPPUNIT_ASSERT( boss.isAligned(s_MR2_nr.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR2_nr.mUid) );
	CPPUNIT_ASSERT( ! boss.isNavigable(s_MR2_nr.mUid) );
	
	boss.addSpace(s_crap_2);
	
	CPPUNIT_ASSERT( boss.worldSpace() == s_MR0_r.mUid ); 
	CPPUNIT_ASSERT( boss.isAligned(s_MR0_r.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR0_r.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_MR0_r.mUid) ); 	

	CPPUNIT_ASSERT( boss.isAligned(s_MR1_r.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR1_r.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_MR1_r.mUid) ); 	

	CPPUNIT_ASSERT( boss.isAligned(s_MR2_nr.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR2_nr.mUid) );
	CPPUNIT_ASSERT( ! boss.isNavigable(s_MR2_nr.mUid) );
}	


void TestSpaceOrganizer::testFirstUSThenPOThenRegister()
{
	SpaceOrganizer boss(mSession, mSessionTimestamp);

	// add 1 us nonreg ->check
	boss.addSpace(s_US0_nr);

	CPPUNIT_ASSERT( boss.worldSpace() == "patient" ); 
	CPPUNIT_ASSERT( !boss.isAligned(s_US0_nr.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_US0_nr.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_US0_nr.mUid) ); 	

	// patient orient ->check
	boss.addSpace(s_patientOrientation);		

	CPPUNIT_ASSERT( boss.worldSpace() == s_patientOrientation.mUid ); 
	CPPUNIT_ASSERT( boss.isAligned(s_US0_nr.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_US0_nr.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_US0_nr.mUid) ); 	

	CPPUNIT_ASSERT(worldTransformIs(boss, s_US0_nr,  s_patientOrientation.m_rMs.inv() * s_US0_nr.m_rMs )); 		
	
	// mr nonreg -> check
	boss.addSpace(s_MR2_nr);
			
	CPPUNIT_ASSERT( boss.worldSpace() == s_patientOrientation.mUid ); 
	CPPUNIT_ASSERT( boss.isAligned(s_US0_nr.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_US0_nr.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_US0_nr.mUid) ); 	

	CPPUNIT_ASSERT( boss.isAligned(s_MR2_nr.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR2_nr.mUid) );
	CPPUNIT_ASSERT( !boss.isNavigable(s_MR2_nr.mUid) ); 	

	CPPUNIT_ASSERT(worldTransformIs(boss, s_MR2_nr,  Transform3D::Identity() ));

	// mr reg + patient reg ->check
	boss.addSpace(s_patient);
	boss.addSpace(s_MR0_r);

	CPPUNIT_ASSERT( boss.worldSpace() == s_MR0_r.mUid ); 
	CPPUNIT_ASSERT( boss.isAligned(s_MR0_r.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR0_r.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_MR0_r.mUid) ); 	
	
	CPPUNIT_ASSERT( !boss.isNavigable(s_US0_nr.mUid) ); // us volume gets disbled after patient registration. 	

	CPPUNIT_ASSERT( !boss.isNavigable(s_MR2_nr.mUid) ); 	
	
	CPPUNIT_ASSERT(worldTransformIs(boss, s_MR0_r,  Transform3D::Identity() ));
	CPPUNIT_ASSERT(worldTransformIs(boss, s_US0_nr,  s_MR0_r.m_rMs.inv() * s_patient.m_rMs * s_US0_nr.m_rMs )); 		
	
	// us reg -> check
	boss.addSpace(s_MR1_r);

	CPPUNIT_ASSERT( boss.worldSpace() == s_MR0_r.mUid ); 
	CPPUNIT_ASSERT( boss.isAligned(s_MR0_r.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR0_r.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_MR0_r.mUid) ); 	
	
	CPPUNIT_ASSERT( !boss.isNavigable(s_US0_nr.mUid) ); 	

	CPPUNIT_ASSERT( !boss.isNavigable(s_MR2_nr.mUid) ); 	

	CPPUNIT_ASSERT( boss.isNavigable(s_MR1_r.mUid) );
	
	CPPUNIT_ASSERT(worldTransformIs(boss, s_MR1_r,  s_MR0_r.m_rMs.inv() * s_MR1_r.m_rMs ));		
}	

void TestSpaceOrganizer::testOldUS()
{
	SpaceOrganizer boss(mSession, mSessionTimestamp);

	// add 1 mr + patient ->check
	boss.addSpace(s_patient);
	boss.addSpace(s_MR0_r);

	CPPUNIT_ASSERT( boss.worldSpace() == s_MR0_r.mUid ); 
	CPPUNIT_ASSERT( boss.isAligned(s_MR0_r.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR0_r.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_MR0_r.mUid) ); 	
			
	// add old us image ->check
	boss.addSpace(s_US2_old);

	CPPUNIT_ASSERT( boss.worldSpace() == s_MR0_r.mUid ); 
	CPPUNIT_ASSERT( boss.isAligned(s_MR0_r.mUid) ); 
//	CPPUNIT_ASSERT( boss.isUpToDate(s_MR0_r.mUid) );
	CPPUNIT_ASSERT( boss.isNavigable(s_MR0_r.mUid) ); 	
	
	CPPUNIT_ASSERT( !boss.isNavigable(s_US2_old.mUid) ); 	
//	CPPUNIT_ASSERT( !boss.isUpToDate(s_US2_old.mUid) );
}	

bool TestSpaceOrganizer::worldTransformIs(const SpaceOrganizer& boss, SpaceProperties a, const Transform3D& wMa)
{
	return similar(boss.relateToWorld(a.mUid).m_rMs, wMa);
}
