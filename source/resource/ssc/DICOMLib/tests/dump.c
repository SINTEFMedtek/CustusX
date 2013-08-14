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
#include "vector3d.h"

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

	study = DICOMLib_GetStudies( NULL, argv[1], NULL, DICOMLIB_NO_CACHE | DICOMLIB_NO_DICOMDIR );
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
		fprintf( stdout, "Study: %s\n", study->studyID );
		if ( !series )
		{
			fprintf( stderr, "\tNo series found!\n" );
		}
		while ( series != NULL )
		{
			double c, w;
			struct instance_t *prev = NULL, *instance = series->first_instance;

			DICOMLib_UseAutoVOI( series );
			DICOMLib_GetWindow( series, &c, &w );
			fprintf( stdout, "\tSeries %s (%d frames, %.02f center %.02f width) - %s %s (split by %s)\n", series->seriesID, series->frames, c, w,
				 series->valid ? "valid" : "INVALID -- ", series->valid ? "" : series->error, series->splitReason );

			while ( instance )
			{
				fprintf( stdout, "\t\tInstance %d dist %.02f (pos %.02f, %.02f, %.02f; ori %.02f,%.02f,%.02f,%.02f,%.02f,%.02f) %s", 
				         (int)instance->instance_id, instance->slice_normal_distance,
				         instance->image_position[0], instance->image_position[1], instance->image_position[2], 
				         series->image_orientation[0], series->image_orientation[1], series->image_orientation[2],
				         series->image_orientation[3], series->image_orientation[4], series->image_orientation[5],
				         instance->path );
				if (prev)
				{
					double dir[3];

					// Calculate direction vector (diff between 2 image positions, mm units)
					vector3d_subtract( dir, instance->image_position, prev->image_position );

					// check if we have valid positions in the files
					if ( fabs( dir[0] ) < 0.0001 && fabs( dir[1] ) < 0.0001 && fabs( dir[2] ) < 0.0001 )
					{
						fprintf( stdout, " - CLASH" );
					}
				}
				fprintf( stdout, "\n" );
				prev = instance;
				instance = instance->next_instance;
			}
			series = series->next_series;
		}
		study = study->next_study;
	}
	DICOMLib_CloseStudies( study );

	DICOMLib_Done();

	SSC_Logging_Done();

	return 0;
}
