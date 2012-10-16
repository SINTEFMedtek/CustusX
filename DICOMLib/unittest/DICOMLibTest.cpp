#include "DICOMLibTest.h"
#include <QTemporaryFile>

void TestDICOMLib::setUp()
{
	studyList = NULL;
	CPPUNIT_ASSERT( DICOMLib_Init()==0 );

	// Pass settings to DICOMLib
	DICOMLib_Config(DICOMLIB_CONF_SPLIT_SERIES_DESCR, 0);
	DICOMLib_Config(DICOMLIB_CONF_MIN_DIST_SLICES, 0.0001);
	DICOMLib_Config(DICOMLIB_CONF_MAX_ORIENT_DIFF, 0.001);
	DICOMLib_Network("SONOWAND_SCP", "SONOWAND_SCP","SONOWAND_SCP", "127.0.0.1", 5104);
}

void TestDICOMLib::tearDown()
{
	if (studyList)
	{
		DICOMLib_CloseStudies(studyList);
	}
	CPPUNIT_ASSERT( DICOMLib_Done() == 0 );
}

void TestDICOMLib::testDummy()
{
	CPPUNIT_ASSERT( true );
}

void TestDICOMLib::testDump()
{
	studyList = DICOMLib_StudiesFromPath( "/testdata", NULL, DICOMLIB_NO_CACHE );
	CPPUNIT_ASSERT ( studyList );
	while ( studyList != NULL )
	{
		struct series_t *series = DICOMLib_GetSeries( studyList, NULL );

		fprintf( stdout, "Patient: %s (%s)\n", studyList->patientName, studyList->patientID );
		fprintf( stdout, "Study: %s - %s\n", studyList->studyID, studyList->rootpath );
		if ( !series )
		{
			fprintf( stderr, "\tNo series found!\n" );
		}
		while ( series != NULL )
		{
			CPPUNIT_ASSERT( fprintf( stdout, "\tSeries %s (%d frames%s) - %s\n", series->seriesID, series->frames,
				 series->valid ? "" : " -- INVALID", series->rootpath ) );
			series = series->next_series;
		}
		studyList = studyList->next_study;
	}
	DICOMLib_CloseStudies( studyList );

}


CPPUNIT_TEST_SUITE_REGISTRATION( TestDICOMLib );


