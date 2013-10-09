// Unit test for reading single files and single images

#include <assert.h>

#include "sscLogger.h"
#include "DICOMLib.h"

int main ( int argc, const char * argv[] )
{
	bool done = false;
	int result;
	struct study_t *study;
	struct series_t *series;
	char image[100 * 100 * 4];

	SSC_Logging_Init_Default( "com.sonowand.test" );
	result = DICOMLib_Init();
	assert( result == 0 );

	DICOMLib_Config( DICOMLIB_CONF_MIN_NUM_SLICES, 5.0 );	// test this call alone, no relevance to rest

	// Test compression support
	study = DICOMLib_StudiesFromPath( "/testdata/Gent-01-MR+Compression", NULL, DICOMLIB_NO_CACHE );
	assert( study && study->first_series );
	assert( study->series_count == 1 );
	result = DICOMLib_CloseStudies( study );
	assert( result == 0 );

	study = DICOMLib_StudiesFromPath( "/testdata/Sheffield-01/", NULL, DICOMLIB_NO_CACHE );
	assert( study && study->first_series );
	assert( study->series_count == 1 );
	series = DICOMLib_GetSeries( study, NULL );
	assert( series );
	assert( series->frames == 250 );

	while ( series != NULL && !done )
	{
		int frame;
		double c, w;

		result = DICOMLib_UseAutoVOI( series );
		assert( result == 0 );
		result = DICOMLib_GetWindow( series, &c, &w );
		assert( result == 0 );

		for ( frame = 0; frame < series->frames; frame++ )
		{
			int x = 0;
			int y = 0;
			const void *buffer = DICOMLib_Image( series, &x, &y, 8, frame );

			assert( buffer );
			(void)buffer;
			done = true;
		}

		if (!done)
		{
			series = series->next_series;
		}
	}

	// Test image generation
	result = DICOMLib_Image_RGB_Fill( series, -1, -1, -1, NULL );
	assert( result != 0 );
	result = DICOMLib_Image_RGB_Fill( series, 100, 100, 0, image );
	assert( result == 0 );

	result = DICOMLib_CloseStudies( study );
	assert( result == 0 );

	result = DICOMLib_Done();
	assert( result == 0 );

	SSC_Logging_Done();

	printf("\n Test succeeded! \n\n");
	return 0;
}
