#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/Xlib.h>
#include <unistd.h>

#include "sscLogger.h"
#include "DICOMLib.h"

#define WINX 640
#define WINY 640

int main ( int argc, const char * argv[] )
{
	struct study_t *study;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: dump <path>\n" );
		exit( 1 );
	}

	SSC_Logging_Init_Default( "com.sonowand.dump" );
	DICOMLib_Init();

	study = DICOMLib_StudiesFromPath( argv[1], NULL, DICOMLIB_NO_CACHE );
	if ( !study )
	{
		fprintf( stderr, "Nothing found!\n" );
		SSC_Logging_Done();
		exit( 1 );
	}
	while ( study != NULL )
	{
		struct series_t *series = DICOMLib_GetSeries( study, NULL );

		fprintf( stdout, "Patient: %s (%s)\n", study->patientName, study->patientID );
		fprintf( stdout, "Study: %s - %s\n", study->studyID, study->rootpath );
		if ( !series )
		{
			fprintf( stderr, "\tNo series found!\n" );
		}
		while ( series != NULL )
		{
			fprintf( stdout, "\tSeries %s (%d frames%s) - %s\n", series->seriesID, series->frames, 
			         series->valid ? "" : " -- INVALID", series->rootpath );
			series = series->next_series;
		}
		study = study->next_study;
	}
	DICOMLib_CloseStudies( study );

	DICOMLib_Done();

	SSC_Logging_Done();

	return 0;
}
