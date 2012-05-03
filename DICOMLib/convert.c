/*
 *  convert.c
 *  DICOMLib
 *
 *  Created by Per M on 07/4/24.
 *  Copyright 2007 SONOWAND AS. All rights reserved.
 *
 * Creates a 3D volume from the instances (slices) of a series.
 *
 */

#include <assert.h>

#include "sscLogger.h"
#include "DICOMLib.h"
#include "vector3d.h"

struct conv_global
{
	double voxel_size[3];   /* x,y,z */
	uint_fast32_t size_old[3];   /* x,y,z */
	uint_fast32_t size_new[3];   /* x,y,z */
	const char *dicomimage;
	char *buffer;
	uint_fast32_t index, slice_size, count;
	double pixel_xshift, pixel_yshift;
	int dstbytes, srcbytes, samples, encoding, planaroffset, rescale;
};

static unsigned int highval = 0, lowval = INT32_MAX;

static inline unsigned int readbuf( struct conv_global *global, uint_fast32_t i, uint_fast32_t j, int offsetpixel )
{
	const int offset = offsetpixel * global->planaroffset;
	const int wmul = global->srcbytes;
	const char *ptr = global->dicomimage + i * wmul + j * global->size_old[0] * wmul + offset * global->srcbytes;

	assert( i < global->size_old[0] );
	assert( j < global->size_old[1] );

	// Read out information depending on bytes length and signedness, and convert signedness while we're at it.
	switch (global->encoding)
	{
	case 1: return *((unsigned char *)ptr);
	case 2: return *((unsigned short *)ptr);
	case 4: return *((unsigned int *)ptr);
	case 5: return (unsigned int)(*ptr) + ((unsigned int)global->rescale);
	case 6: return (unsigned int)(*((signed short *)ptr)) + ((unsigned int)global->rescale);
	case 8: return (unsigned int)(*((signed int *)ptr)) + ((unsigned int)global->rescale);
	default: return 0;	// should absolutely never happen!
	}
}

static inline void setnewbuf( struct conv_global *global, uint_fast32_t i, uint_fast32_t j, uint_fast32_t k, unsigned int val, int offsetIdx )
{
	const int offset = offsetIdx * global->dstbytes;
	const int wmul = global->dstbytes * global->samples;
	void *dst = global->buffer + i * wmul + j * global->size_new[0] * wmul + k * global->slice_size + offset;

	if (val > highval) highval = val;
	if (val < lowval) lowval = val;

	assert( global->index < global->count );
	assert( i < global->size_new[0] );
	assert( j < global->size_new[1] );
	switch (global->dstbytes)
	{
	case 1: *((char *)dst) = val; break;
	case 2: *((unsigned short *)dst) = val; break;
	case 4: *((unsigned int *)dst) = val; break;
	default: *((char *)dst) = 0; break;	// should absolutely never happen!
	}
}

struct volume_t *DICOMLib_GetVolume( struct series_t *series, progress_func_t *callback )
{
	struct instance_t *instance, *last;
	double image_pos_vector[3], gantry_tilt_vector[3];
	double image_orientation_normal[3];
	bool do_abort = false;
	struct volume_t *volume;
	struct conv_global global;

	highval = 0;
	lowval = INT32_MAX;

	if ( series == NULL || series->frames < 2 || series->first_instance == NULL )
	{
		errno = EINVAL;
		SSC_LOG( "Bad parameter (series=%p)", series );
		return NULL;
	}
	if ( !series->valid )
	{
		errno = EPROTO;
		SSC_LOG( "Invalid series" );
		return NULL;
	}

	/* Grab first instance */
	instance = series->first_instance;

	/* Grab last instance */
	for ( last = series->first_instance; last->next_instance != NULL; last = last->next_instance );

	/*** Calculate slice size with spare room for shifting gantry tilt ***/

	/* Store some data for iterative calculations below */
	global.count = series->frames;
	global.index = 0;
	global.dstbytes = series->bits_per_sample / 8;
	global.samples = series->samples_per_pixel;
	global.size_old[0] = series->columns;
	global.size_old[1] = series->rows;
	global.size_old[2] = global.count;

	// Find image vector from first to last image through their image positions
	vector3d_subtract( image_pos_vector, last->image_position, instance->image_position );

	// Divide image_pos_vector by number of slices - 1
	vector3d_scalar_divide( image_pos_vector , global.count - 1 );

	// find the image orientation in z direction
	vector3d_cross_product( image_orientation_normal, &series->image_orientation[0],
	                        &series->image_orientation[3] );

	// Find the pixel size in mm (x and y given by DICOM, z is calculated)
	global.voxel_size[0] = series->pixel_spacing[0]; // given by DICOM data
	global.voxel_size[1] = series->pixel_spacing[1];
	global.voxel_size[2] = vector3d_inner_product( image_orientation_normal, image_pos_vector );

	// Save pixel spacing in z dimension for possible later use
	series->slice_thickness = global.voxel_size[2];

	// Image-to-image vector parallell to the normal vector.
	vector3d_scalar_multiply( image_orientation_normal, global.voxel_size[2] );

	// Gantry tilt vector is the difference between the orthogonal and the old coordinate systems.
	vector3d_subtract( gantry_tilt_vector, image_pos_vector, image_orientation_normal );

	// Calculate x shift
	global.pixel_xshift = vector3d_inner_product( gantry_tilt_vector, &series->image_orientation[0] ) / series->pixel_spacing[0];

	// Calculate y shift
	global.pixel_yshift = vector3d_inner_product( gantry_tilt_vector, &series->image_orientation[3] ) / series->pixel_spacing[1];

	// Calculate size of new volume
	global.size_new[0] = fabs( global.pixel_xshift * global.count ) + series->columns;
	global.size_new[1] = fabs( global.pixel_yshift * global.count ) + series->rows;
	global.size_new[2] = global.count;
	global.slice_size =  global.size_new[0] * global.size_new[1] * global.dstbytes * global.samples;
	global.buffer = calloc( global.count, global.slice_size );
	global.rescale = 0;	// set later when we have read the first slice... bit of a hack since that's only when we know
	SSC_LOG( "Converting volume with pixel shift (%.4f, %.4f), new size (%d, %d, %d), voxel_size (%f,%f,%f)", global.pixel_xshift, global.pixel_yshift,
		 (int)global.size_new[0], (int)global.size_new[1], (int)global.size_new[2], global.voxel_size[0], global.voxel_size[1], global.voxel_size[2] );
	if ( !global.buffer )
	{
		errno = ENOMEM;
		return NULL;
	}

	while ( !do_abort && instance != NULL )
	{
		uint_fast32_t i, j;
		uint_fast32_t xshift_int;
		uint_fast32_t yshift_int;
		int progress;

		global.dicomimage = DICOM_raw_image( series, instance, global.index );
		if ( global.dicomimage == NULL )
		{
			SSC_LOG( "DICOMLib_Conversion_Buffering: Error in internal buffering grabbing image %d", (int)global.index );
			errno = -1;
			free(global.buffer);
			return NULL;
		}
		if (global.index == 0 && instance->isSigned)	// hack
		{
			global.rescale = pow(2, series->bits_per_sample) / 2;
		}
		global.srcbytes = instance->internal_bits_per_sample / 8;
		global.encoding = global.srcbytes + instance->isSigned * 4;
		global.planaroffset = 1;	// default
		if (global.samples == 3)
		{
			// DCMTK's internal format for RGB data is planar, so we need to skip the read
			// pointer around like this
			global.planaroffset = global.size_old[0] * global.size_old[1] * global.srcbytes;
		}

		// roundoff to int (nearest neighbour interpolation)
		if ( global.pixel_xshift < 0 )
		{
			xshift_int = ( global.count - ( global.index + 1 ) ) * fabs( global.pixel_xshift );
		} else {
			xshift_int = global.index * global.pixel_xshift;
		}
		if ( global.pixel_yshift < 0 )
		{
			yshift_int = ( global.count - ( global.index + 1 ) ) * fabs( global.pixel_yshift );
		} else {
			yshift_int = global.index * global.pixel_yshift;
		}

		for ( j = 0; j < series->rows; j++ )
		{
			uint_fast32_t x_index, y_index;

			y_index = j + yshift_int;
			for ( i = 0; i < series->columns; i++ )
			{
				x_index = i + xshift_int;
				setnewbuf( &global, x_index, y_index, global.index, readbuf( &global, i, j, 0 ), 0 );
				if ( global.samples > 1 ) setnewbuf( &global, x_index, y_index, global.index, readbuf( &global, i, j, 1 ), 1 );
				if ( global.samples > 2 ) setnewbuf( &global, x_index, y_index, global.index, readbuf( &global, i, j, 2 ), 2 );
				if ( global.samples > 3 ) setnewbuf( &global, x_index, y_index, global.index, readbuf( &global, i, j, 3 ), 3 );
			}
		}
		global.index++;
		instance = instance->next_instance;
		progress = ( ( double ) global.index / ( double ) series->frames ) * 999.0f;
		if ( callback )
		{
			int retval = callback( progress );

			if ( retval != 0 )
			{
				do_abort = true;
			}
		}
	}

	if ( do_abort )
	{
		free(global.buffer);
		return NULL;
	}

	volume = malloc( sizeof( *volume ) );
	volume->volume = global.buffer;
	volume->x = global.size_new[0];
	volume->y = global.size_new[1];
	volume->z = global.size_new[2];
	volume->bits_per_sample = global.dstbytes * 8;
	volume->samples_per_pixel = global.samples;
	volume->firstpixel = lowval;
	volume->lastpixel = highval;
	volume->rescaleIntercept = global.rescale;
	volume->voi.width = series->VOI.current.width;
	volume->voi.center = series->VOI.current.center + volume->rescaleIntercept;
	memcpy( volume->image_orientation, series->image_orientation, sizeof( volume->image_orientation ) );
	memcpy( volume->pixel_spacing, global.voxel_size, sizeof( volume->pixel_spacing ) );

	return volume;
}

void DICOMLib_FreeVolume( struct volume_t *volume )
{
	free(volume->volume);
	free(volume);
}
