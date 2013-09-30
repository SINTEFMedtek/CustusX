// Test multi-frame import, and read of exported data

#include <assert.h>
#include <stdio.h>

#include "sscLogger.h"
#include "DICOMLib.h"

int main ( int argc, const char * argv[] )
{
	int result;
	struct study_t *study;
	struct series_t *series;

	SSC_Logging_Init_Default( "com.sonowand.test" );
	result = DICOMLib_Init();
	assert( result == 0 );

	study = DICOMLib_GetStudies( NULL, "/testdata/Exported-1/DICOM000", NULL, DICOMLIB_NO_CACHE );
	assert( study && study->first_series );
	assert( study->series_count == 1 );
	series = DICOMLib_GetSeries( study, NULL );
	assert( series );
	assert( series->frames == 210 );
	assert( study->next_study == NULL );
	assert( DICOMLib_DescribeOrientation( series, 0, true ) == 'R' );

	result = DICOMLib_CloseStudies( study );
	assert( result == 0 );

	result = DICOMLib_Done();
	assert( result == 0 );

	SSC_Logging_Done();

	return 0;
}
