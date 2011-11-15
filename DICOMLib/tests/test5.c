#include <assert.h>
#include <stdio.h>

#include "sscLogger.h"
#include "DICOMLib.h"

#define WINX 640
#define WINY 640

extern bool DICOMLib_INTERNAL_TEST(void);

int main ( int argc, const char * argv[] )
{
	int result;
	struct study_t *study, *iter;
	struct series_t *series;

	SSC_Logging_Init_Default( "com.sonowand.test" );
	result = DICOMLib_Init();
	assert( result == 0 );

	DICOMLib_INTERNAL_TEST();	// should not crash

	study = DICOMLib_GetStudies( NULL, NULL, NULL, DICOMLIB_NO_CACHE );
	assert( study == NULL );

	study = DICOMLib_GetStudies( NULL, "/testdata/empty/", NULL, DICOMLIB_NO_CACHE );
	assert( study == NULL );

	study = DICOMLib_GetStudies( NULL, "/testdata/Sheffield-01/", NULL, DICOMLIB_NO_CACHE );
	assert( study && study->first_series );
	assert( study->series_count == 1 );
	series = DICOMLib_GetSeries( study, NULL );
	assert( series );
	assert( series->frames == 250 );
	assert( study->next_study == NULL );
	assert( DICOMLib_DescribeOrientation( series, 0, true ) == 'R' );
	assert( DICOMLib_NoVOI( series ) == 0 );

	study = DICOMLib_GetStudies( study, "/testdata/SONOWAND-06-Eirik-Mo/", NULL, DICOMLIB_NO_CACHE );
	assert( study );
	iter = study->next_study;
	assert( iter );
	assert( iter->series_count == 1 );
	series = DICOMLib_GetSeries( iter, NULL );
	assert( series );
	assert( series->frames == 160 );
	assert( iter->next_study == NULL );
	assert( DICOMLib_DescribeOrientation( series, 0, false ) == 'P' );
	assert( DICOMLib_DescribeOrientation( series, 0, true ) == 'A' );
	assert( DICOMLib_DescribeOrientation( series, 1, false ) == 'I' );
	assert( DICOMLib_DescribeOrientation( series, 1, true ) == 'S' );

	study = DICOMLib_GetStudies( study, "/testdata/Stockholm-02-CT/PA1", NULL, DICOMLIB_NO_DICOMDIR | DICOMLIB_NO_CACHE );
	assert( study );
	iter = study->next_study->next_study;
	assert( iter );
	assert( iter->series_count == 2 );
	series = DICOMLib_GetSeries( iter, NULL );
	assert( series );
	if ( series->frames == 1 ) series = series->next_series;	// order may be random
	assert( series->frames == 138 );

	result = DICOMLib_CloseStudies( study );
	assert( result == 0 );

	result = DICOMLib_Done();
	assert( result == 0 );

	SSC_Logging_Done();

	return 0;
}
