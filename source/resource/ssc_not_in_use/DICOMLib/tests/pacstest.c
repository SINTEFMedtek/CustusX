#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "sscLogger.h"
#include "DICOMLib.h"

int main ( int argc, const char * argv[] )
{
	int result;
	struct study_t *study;

	if ( argc != 1 )
	{
		fprintf( stderr, "Usage: gl2dpacsviewer <path>\n" );
		exit( 1 );
	}

	SSC_Logging_Init_Default( "com.sonowand.test" );
	result = DICOMLib_Init();
	assert( result == 0 );

	DICOMLib_Network( "SONOWAND", "ANY-SCP", "STORESCP", "10.200.31.178", 5104 );

	study = DICOMLib_StudiesFromPACS( NULL, DICOMLIB_SEARCH_NONE, NULL );
	if ( !study )
	{
		fprintf( stderr, "No PACS found?\n" );
		SSC_Logging_Done();
		exit( 1 );
	}

	DICOMLib_Done();
	SSC_Logging_Done();
	return 0;
}
