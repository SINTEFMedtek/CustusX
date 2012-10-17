#include "DICOMLibTest.h"
#include <QTemporaryFile>

void TestDICOMLib::setUp()
{
	studyList = NULL;
	CPPUNIT_ASSERT( DICOMLib_Init() == 0 );

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

void TestDICOMLib::testInternalTest()
{
	CPPUNIT_ASSERT( DICOMLib_INTERNAL_TEST() == true );
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

void TestDICOMLib::testGetNullStudies()
{
	CPPUNIT_ASSERT( DICOMLib_GetStudies( NULL, NULL, NULL, DICOMLIB_NO_CACHE ) == NULL );
}

void TestDICOMLib::testGetStudiesFromEmptyFolder()
{
	CPPUNIT_ASSERT( DICOMLib_GetStudies( NULL, "/testdata/empty/", NULL, DICOMLIB_NO_CACHE ) == NULL );
}

void TestDICOMLib::testGetStudies()
{
	struct study_t *iter;
	struct series_t *series;

	studyList = DICOMLib_GetStudies( NULL, "/testdata/Sheffield-01/", NULL, DICOMLIB_NO_CACHE );
	CPPUNIT_ASSERT( studyList && studyList->first_series );
	CPPUNIT_ASSERT( studyList->series_count == 1 );
	series = DICOMLib_GetSeries( studyList, NULL );
	CPPUNIT_ASSERT( series );
	CPPUNIT_ASSERT( series->frames == 250 );
	CPPUNIT_ASSERT( studyList->next_study == NULL );
	CPPUNIT_ASSERT( DICOMLib_DescribeOrientation( series, 0, true ) == 'R' );
	CPPUNIT_ASSERT( DICOMLib_NoVOI( series ) == 0 );

	studyList = DICOMLib_GetStudies( studyList, "/testdata/SONOWAND-06-Eirik-Mo/", NULL, DICOMLIB_NO_CACHE );
	CPPUNIT_ASSERT( studyList );
	iter = studyList->next_study;
	CPPUNIT_ASSERT( iter );
	CPPUNIT_ASSERT( iter->series_count == 1 );
	series = DICOMLib_GetSeries( iter, NULL );
	CPPUNIT_ASSERT( series );
	CPPUNIT_ASSERT( series->frames == 160 );
	CPPUNIT_ASSERT( iter->next_study == NULL );
	CPPUNIT_ASSERT( DICOMLib_DescribeOrientation( series, 0, false ) == 'P' );
	CPPUNIT_ASSERT( DICOMLib_DescribeOrientation( series, 0, true ) == 'A' );
	CPPUNIT_ASSERT( DICOMLib_DescribeOrientation( series, 1, false ) == 'I' );
	CPPUNIT_ASSERT( DICOMLib_DescribeOrientation( series, 1, true ) == 'S' );

	studyList = DICOMLib_GetStudies( studyList, "/testdata/Stockholm-02-CT/PA1", NULL, DICOMLIB_NO_DICOMDIR | DICOMLIB_NO_CACHE );
	CPPUNIT_ASSERT( studyList );
	iter = studyList->next_study->next_study;
	CPPUNIT_ASSERT( iter );
	CPPUNIT_ASSERT( iter->series_count == 2 );
	series = DICOMLib_GetSeries( iter, NULL );
	CPPUNIT_ASSERT( series );
	if ( series->frames == 1 ) series = series->next_series;	// order may be random
	CPPUNIT_ASSERT( series->frames == 138 );
}




CPPUNIT_TEST_SUITE_REGISTRATION( TestDICOMLib );


