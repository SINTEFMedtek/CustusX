// Convert general DICOM files to SONOWAND multi-frame volume

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "sscLogger.h"
#include "DICOMLib.h"

#define SW_EPSILON 0.0001

int main( int argc, char **argv )
{
	int result, numFid, numCut;
	struct study_t *study;
	struct series_t *series;
	struct volume_t *data = NULL;
	uint16_t *lut, x;
	double fiducials[6] = { 0.0, 1.1, 2.2, 3.3, 4.4, 5.5 };
	double cutplanes[9] = { 3.1, 2.2, 1.3, 4.1, 5.2, 5.3, 5.4, 5.5 };
	double gforce[16] = { 0.0, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.10, 11.11, 12.12, 13.13, 14.14, 15.15 };
	double fiducials2[6];
	double cutplanes2[9];
	double gforce2[16];

	SSC_Logging_Init_Default( "com.sonowand.test" );
	result = DICOMLib_Init();
	assert( result == 0 );

	study = DICOMLib_StudiesFromPath( "/testdata/Sheffield-01/", NULL, DICOMLIB_NO_CACHE );
	if ( !study )
	{
		fprintf( stderr, "Nothing found!\n" );
		SSC_Logging_Done();
		exit( 1 );
	}
	series = DICOMLib_GetSeries( study, NULL );
	while ( series && !data )
	{
		if ( series->valid )
		{
			DICOMLib_UseAutoVOI( series );
			data = DICOMLib_GetVolume( series, NULL );
		}
		if ( !data )
		{
			fprintf( stderr, "Skipping series %s with %d frames: %s\n", series->seriesID, series->frames, series->series_info );
			series = series->next_series;	// try next
		}
	}
	assert( series );

	data = DICOMLib_GetVolume( series, NULL );
	assert( data );
	lut = calloc( 256, sizeof( *lut ) );
	for ( x = 0; x < 256; x++ ) lut[x] = x;

	result = DICOMLib_WriteRegistration( "/Data/Fiducials.dcm", study, series, false, 2, fiducials, 1, cutplanes );
	assert( result == 0 );
	result = DICOMLib_WriteGravity( "/Data/Gravity.dcm", study, series, false, gforce );
	assert( result == 0 );
	study->first_series = series;	// HACK, may lead to memory leak in test
	series->volume = data;		// HACK, may lead to memory leak in test
	result = DICOMLib_WriteSeries( "/Data/SonoWand.dcm", study, false, DICOMLIB_PROFILE_DVDRAM, true );
	assert( result == 0 );

	numFid = 2;
	numCut = 1;
	result = DICOMLib_LoadRegistration( "/Data/Fiducials.dcm", &numFid, fiducials2, &numCut, cutplanes2 );
	assert( result == 0 );
	assert( numFid == 2 );
	assert( numCut == 1 );
	for ( x = 0; x < numFid; x++ ) assert( fabs( fiducials[x] - fiducials2[x] ) < SW_EPSILON );
	for ( x = 0; x < numCut; x++ ) assert( fabs( cutplanes[x] - cutplanes2[x] ) < SW_EPSILON );

	result = DICOMLib_LoadGravity( "/Data/Gravity.dcm", gforce2 );
	assert( result == 0 );
	for ( x = 0; x < 16; x++ ) assert( fabs( gforce[x] - gforce2[x] ) < SW_EPSILON );

	result = DICOMLib_Done();
	assert( result == 0 );

	SSC_Logging_Done();

	return 0;
}
