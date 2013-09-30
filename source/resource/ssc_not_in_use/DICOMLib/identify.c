/*
 *  identify.c
 *  DICOMLib
 *
 *  Created by Per M on 07/3/7.
 *  Copyright 2007 MISON AS. All rights reserved.
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <strings.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <byteswap.h>

#include "sscLogger.h"
#include "identify.h"

#define BUFFER_SIZE 1024
static char buffer[BUFFER_SIZE];
//#define REPORT(...) do { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); } while(false);
#define REPORT(...)

#if __BYTE_ORDER == __BIG_ENDIAN
#define M_BIG_ENDIAN 1
#else
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define M_BIG_ENDIAN 0
#else
#error What - no endianness??
#endif
#endif

/* Scan very quickly to find ACR-NEMA tag in 8:10. Return standard version found:
 * 0 - not recognized
 * 1 - ACR-NEMA 1.0
 * 2 - ACR-NEMA 2.0
 * 3 - Possibly DICOM Implicit VR
 */
static int fastscan_acrnema( const char *path )
{
	char *pos = buffer;
	const char *end = buffer + BUFFER_SIZE;
	uint32_t val32;
	uint16_t group;
	bool big_endian = false;

	/* This approach to detecting endian type is taken from http://www.dclunie.com/medical-image-faq/html/part2.html */
	// examine probable group number ... assume <= 0x00ff
	if ( buffer[0] < buffer[1] ) {
		big_endian = true;
	} else if ( buffer[0] == 0 && buffer[1] == 0 ) {
		// blech ... group number is zero
		// no point in looking at element number
		// as it will probably be zero too (group length)
		// try the 32 bit value length of implicit vr
		if ( buffer[4] < buffer[7] ) {
			big_endian = true;
		}
	}

	/* Define convenience macros that ensure correct endianness, and return if we empty buffer. */
#define GRAB16(x) if (pos < end + 2) { if (big_endian != M_BIG_ENDIAN) { \
x = bswap_16(*(uint16_t *)pos); } else { x = *(uint16_t *)pos; } pos += 2; } else { return 0; }
#define GRAB32(x) if (pos < end + 4) { if (big_endian != M_BIG_ENDIAN) { \
x = bswap_32(*(uint32_t *)pos); } else { x = *(uint32_t *)pos; } pos += 4; } else { return 0; }

	GRAB16( group );

	/* Scan tags */
	while ( group < 0x9 )
	{
		uint32_t length;
		uint16_t element;

		GRAB16( element );
		GRAB32( length );
		if ( length == 0xffffffff )
		{
			REPORT( "Cannot handle undefined length here" );
			return 3; /* Only DICOM could be so dumb */
		}
		if ( length % 2 != 0 )
		{
			REPORT( "  The value length must be even to conform to standard." );
			return 0;
		}

		/* Check for group length tag (data element zero) for group != 8 and
		 * skip the whole group. */
		if ( group != 0x0 && group != 0x8 && element == 0 )
		{
			GRAB32( val32 );
			pos += val32;
			continue;
		}

		/* Check for ACR-NEMA recognition code */
		if ( group == 0x8 && element == 0x10 && pos < end + 11 )
		{
			if ( pos[0] != 'A' || pos[1] != 'C' || pos[2] != 'R' || pos[3] != '-' )
			{
				REPORT( "  Unknown ACR-NEMA recognition tag: %c%c%c%c.", pos[0], pos[1], pos[2], pos[3] );
				return 0; /* Not ACR-NEMA in ACR-NEMA recognition tag */
			}
			if ( pos[9] == '1' )
			{
				return 1;
			}
			else if ( pos[9] == '2' )
			{
				return 2;
			}
			else
			{
				REPORT( "  Unknown ACR-NEMA version." );
				SSC_LOG( "Scanning %s we found ACR-NEMA tag but it contains unknown version %c", path, pos[9] );
				return 0; /* Unknown ACR-NEMA version */
			}
		}

		/* Check for SOP Class UID tag */
		if ( group == 0x8 && element == 0x16 )
		{
			return 3; /* DICOM file */
		}

		/* Skip value */
		pos += length;

		/* Next tag! Grab new group number */
		GRAB16( group );
	}
	REPORT( "  No ACR-NEMA identification tag found." )
	return 0;
#undef GRAB16
#undef GRAB32
}

bool identify( const char *path )
{
	size_t result;
	FILE *fp;

	/* Read in 1024 bytes header */
	fp = fopen( path, "r" );

	if ( fp == NULL )
	{
		SSC_LOG( "Error opening file %s: %s\n", path, strerror( errno ) );
		return false;
	}
	result = fread( &buffer, 1024, 1, fp );
	if ( result != 1 && !feof( fp ) )
	{
		SSC_LOG( "Error reading file %s(%zd): %s\n", path, result, strerror( ferror( fp ) ) );
		fclose( fp );
		return false;
	}
	fclose( fp );

	/* Check DICM tag */
	if ( buffer[128] == 'D' && buffer[129] == 'I' && buffer[130] == 'C' && buffer[131] == 'M' ) {
		return true;
	}

#if 0
	/* Check NIfTI tag */
	if ( buffer[3] == 92 && buffer[2] == 1 && buffer[1] == 0 && buffer[0] == 0 ) {
		ft = false; /* Possibly little endian Analyze */
	}
	if ( buffer[0] == 92 && buffer[1] == 1 && buffer[2] == 0 && buffer[3] == 0 ) {
		ft = false; /* Possibly big endian Analyze */
	}
	if ( ft == DICOMLIB_FT_LAST && strncmp( &buffer[344], "ni1", 3 ) == 0 ) {
		return DICOMLIB_FT_NIFTI_1_HDR; /* NIfTI-1 file with separate image data file */
		/* FIXME: Should check if the separate file does exist */
	}
	if ( ft == DICOMLIB_FT_LAST && strncmp( &buffer[344], "n+1", 3 ) == 0 ) {
		return false; /* NIfTI-1 file with included image data in same file */
	}
#endif

	/* If it is DICOM 3.0 explicit VR (almost unconceivable without a part 10 header)
	 * then we will find two capital letters exactly in byte positions 5 and 6; also
	 * we can assume that exactly one of the two first pairs of bytes are zero, ie a low
	 * group number and element number for the first tag. */
	if ( ( buffer[0] == 0 || buffer[1] == 0 ) && ( buffer[2] == 0 || buffer[3] == 0 )
	        && isupper( buffer[4] ) && isupper( buffer[5] ) )
	{
		return true; /* Guess that this is an explicit VR non-part 10 DICOM file */
		/* FIXME: Not sure what further tests to carry out without parsing the file here. */
	}

	switch ( fastscan_acrnema( path ) )
	{
	case 1: /* ACR-NEMA 1.0 */
	case 2: /* ACR-NEMA 2.0 */
		return true;
	case 3: /* Possibly implicit VR DICOM */
		return true;
	default: /* error */
		break;
	}

	return false;
}
