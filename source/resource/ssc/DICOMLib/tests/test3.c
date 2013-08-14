// Unit test of DICOMDIR

#include <assert.h>
#include <unistd.h>

#include "sscLogger.h"
#include "DICOMLib.h"

int main ( int argc, const char * argv[] )
{
	int result, i;
	struct study_t *study;
	const struct volume_t *data;
	const char *ptr;
	struct series_t *series;

	SSC_Logging_Init_Default( "com.sonowand.test" );
	result = DICOMLib_Init();
	assert( result == 0 );

	// This should fail
	study = DICOMLib_StudiesFromDICOMDIR( "/asdlkfjsdlfj11" );
	assert( study == NULL );
	study = DICOMLib_StudiesFromDICOMDIR( "/Data/Tools/" );
	assert( study == NULL );

	// This should work
	study = DICOMLib_StudiesFromDICOMDIR( "/testdata/Gent-01-MR+JPEG/DICOMDIR" );
	assert( study != NULL && study->first_series == NULL );

	// This should fail
	result = DICOMLib_UseAutoVOI( NULL );
	assert( result != 0 );

	// This should fail
	result = DICOMLib_NoVOI( NULL );
	assert( result != 0 );

	// This should fail
	data = DICOMLib_GetVolume( NULL, NULL );
	assert( data == NULL );

	// This should fail
	series = DICOMLib_GetSeries( NULL, NULL );
	assert( series == NULL );

	// This should work
	series = DICOMLib_GetSeries( study, NULL );
	assert( series );
	assert( series->frames > 2 );

	// This should work
	data = DICOMLib_GetVolume( series, NULL );
	assert( data );
	assert( data->volume );

	// This should work
	result = DICOMLib_UseAutoVOI( study->first_series );
	assert( result == 0 );

	ptr = data->volume;
	for ( i = 0; i < series->frames; i++ )
	{
		ptr += data->x * data->y;
		if ( *ptr == 3 ) usleep( 1 );	// just to access the memory
	}
	assert( i == series->frames );

	result = DICOMLib_CloseStudies( study );
	assert( result == 0 );

	result = DICOMLib_Done();
	assert( result == 0 );

	SSC_Logging_Done();

	return 0;
}
