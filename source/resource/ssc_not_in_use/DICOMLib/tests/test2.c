// Unit test of conversion

#include <assert.h>
#include <unistd.h>

#include "sscLogger.h"
#include "DICOMLib.h"

int progress_func(int progress)
{
	return -1;	// abort at first opportunity
}

int main ( int argc, const char * argv[] )
{
	int result, i;
	struct study_t *study;
	const struct volume_t *data;
	const char *ptr;
	struct series_t *series;
	double c, w;

	SSC_Logging_Init_Default( "com.sonowand.test" );
	result = DICOMLib_Init();
	assert( result == 0 );

	study = DICOMLib_StudiesFromPath( "/", NULL, DICOMLIB_NO_CACHE );	// not allowed
	assert( study == NULL );

	study = DICOMLib_StudiesFromPath( "/testdata/Sheffield-01/", progress_func, DICOMLIB_NO_CACHE );	// cancel early
	DICOMLib_CloseStudies( study );

	study = DICOMLib_StudiesFromPath( "/testdata/Sheffield-01/", NULL, DICOMLIB_REINDEX_STUDY | DICOMLIB_NO_CACHE );
	assert( study );
	series = DICOMLib_GetSeries( study, NULL );
	assert( series );

	result = DICOMLib_SetWindow( series, 80.0, 40.0 );
	assert( result == 0 );
	result = DICOMLib_GetWindow( series, &c, &w );
	assert( c == 80.0 && w == 40.0 );

	result = DICOMLib_UseAutoVOI( series );
	assert( result == 0 );
	data = DICOMLib_GetVolume( series, NULL );
	assert( data );

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
