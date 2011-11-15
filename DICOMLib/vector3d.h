/*
 *  vector3d.h
 *  DICOMLib
 *
 *  Created by Per M on 07/5/15.
 *  Copyright 2007 MISON AS. All rights reserved.
 *
 * Adapted from some code I wrote for the Buckyballs project.
 *
 */

#ifndef _VECTOR3D_H
#define _VECTOR3D_H

/* This code is not optimized in any way yet. I want to do that
 * when the impact can be measured. */

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************
  Copy src -> dst.
**************************************************************************/
static inline void vector3d_copy( double *dst, const double *src )
{
	memcpy( dst, src, sizeof( double ) * 3 );
}

/**************************************************************************
  Flip vector into opposite direction.
**************************************************************************/
static inline void vector3d_flip( double *dest )
{
	dest[0] *= -1;
	dest[1] *= -1;
	dest[2] *= -1;
}

/**************************************************************************
  Add vx to vy and put the result in dest.
**************************************************************************/
static inline void vector3d_add( double *dest, const double *vx, const double *vy )
{
	dest[0] = vx[0] + vy[0];
	dest[1] = vx[1] + vy[1];
	dest[2] = vx[2] + vy[2];
}

/**************************************************************************
   Deduct vy from vx and put the result in dest.
**************************************************************************/
static inline void vector3d_subtract( double *dest, const double *vx, const double *vy )
{
	dest[0] = vx[0] - vy[0];
	dest[1] = vx[1] - vy[1];
	dest[2] = vx[2] - vy[2];
}

/**************************************************************************
  Divide each element of the vector v by the scalar d.
**************************************************************************/
static inline void vector3d_scalar_divide( double *v, double d )
{
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
}

/**************************************************************************
  Multiply each element of the vector v by the scalar d.
**************************************************************************/
static inline void vector3d_scalar_multiply( double *v, double d )
{
	v[0] *= d;
	v[1] *= d;
	v[2] *= d;
}

/**************************************************************************
  Return the length of v.
**************************************************************************/
static inline double vector3d_length( const double *v )
{
	return sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

/**************************************************************************
  Normalize v.
**************************************************************************/
static inline void vector3d_normalize( double *v )
{
	double len = vector3d_length( v );

	if ( len == 0.0f ) {
		/* I am not entire sure if this is the correct action here. - Per */
		return;
	}
	vector3d_scalar_divide( v, len );
}

/**************************************************************************
  Put the crossproduct of vx and vy into dest.
**************************************************************************/
static inline void vector3d_cross_product( double *dest, const double *vx,
        const double *vy )
{
	dest[0] = vx[1] * vy[2] - vx[2] * vy[1];
	dest[1] = vx[2] * vy[0] - vx[0] * vy[2];
	dest[2] = vx[0] * vy[1] - vx[1] * vy[0];
}

/**************************************************************************
  Return the inner product (also called dot product) of v and q.
**************************************************************************/
static inline double vector3d_inner_product( const double *v, const double *q )
{
	return ( v[0] * q[0] + v[1] * q[1] + v[2] * q[2] );
}

/**************************************************************************
  Do an approximate comparison between two vectors.
**************************************************************************/
static inline bool vector3d_compare( const double *v1, const double *v2, double precision )
{
	int i;

	for ( i = 0; i < 3; i++ ) {
		if ( fabs( v1[i] - v2[i] ) > precision ) {
			return FALSE;
		}
	}
	return TRUE;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
