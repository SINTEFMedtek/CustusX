// Unit test of nifti export

#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>

#include "sscLogger.h"
#include "DICOMLib.h"

int teststudy( const char *name )
{
	int num = 0;
	struct study_t *study;
	struct series_t *series;

	study = DICOMLib_StudiesFromPath( name, NULL, DICOMLIB_NO_CACHE );
	assert( study );

	series = DICOMLib_GetSeries( study, NULL );
	assert( series );

	num = DICOMLib_WriteNifti( "/Data/Temporary", study );

	return num;
}

int main ( int argc, const char * argv[] )
{
	int result;
	SSC_Logging_Init_Default( "com.sonowand.test" );
	result = DICOMLib_Init();
	assert( result == 0 );

	printf("Parsing: %s \n", argv[1] );

	// result = teststudy( "/testdata/Export/dicom001" );
	// assert( result == 0 );

	result = teststudy( argv[1] );
	printf("result = %d\n", result);

	result = DICOMLib_Done();
	assert( result == 0 );

	SSC_Logging_Done();

	return 0;
}
