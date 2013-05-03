// Unit test of conversion of our own exported data (angio)

#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>

#include "sscLogger.h"
#include "DICOMLib.h"

int main ( int argc, const char * argv[] )
{
	int result, i, length;
	struct study_t *study;
	const struct volume_t *data;
	const char *ptr;
	struct series_t *series;
	FILE *fp;
	void *buffer;

	SSC_Logging_Init_Default( "com.sonowand.test" );
	result = DICOMLib_Init();
	assert( result == 0 );

	study = DICOMLib_StudiesFromPath( "/testdata/Export/dicom001", NULL, DICOMLIB_NO_CACHE );
	assert( study );

	series = DICOMLib_GetSeries( study, NULL );
	assert( series );
	assert( series->frames == 306 );

	data = series->volume = DICOMLib_GetVolume( series, NULL );
	assert( data );
	assert( data->samples_per_pixel == 1 );
	assert( data->bits_per_sample == 8 );

	ptr = data->volume;
	for ( i = 0; i < series->frames; i++ )
	{
		if ( *ptr == 3 ) usleep( 1 );	// just to access the memory
		ptr += data->x * data->y * data->samples_per_pixel * (data->bits_per_sample / 8);
	}
	assert( i == series->frames );

	(void) mkdir("/tmp/testexport", 0777);

	/* Export to JPEG */
	result = DICOMLib_WriteSeries("/tmp/testexport/jpeg", study, false, DICOMLIB_PROFILE_USB, DICOMLIB_ALIGNMENT_REGISTRATION);
	assert(result == 0);

	/* Export to unenhanced */
	result = DICOMLib_WriteUnenhancedSeries("/tmp/testexport/blah", study, false, DICOMLIB_PROFILE_DVDRAM_UNENHANCED);
	assert(result == 0);
	result = DICOMLib_WriteUnenhancedSeries("/tmp/testexport/jpblah", study, false, DICOMLIB_PROFILE_USB_UNENHANCED);
	assert(result == 0);

	/* Export PDF */
	fp = fopen("/testdata/Export/NASA_FMEA_Presentation.pdf", "r");
	assert(fp);
	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buffer = malloc(length + 1);
	result = fread(buffer, 1, length, fp);
	assert(result == length);
	fclose(fp);
	result = DICOMLib_WriteSnapshot("/tmp/pdf.dcm", buffer, length, study);

	result = DICOMLib_CloseStudies( study );
	assert( result == 0 );

	result = DICOMLib_Done();
	assert( result == 0 );

	SSC_Logging_Done();

	return 0;
}
