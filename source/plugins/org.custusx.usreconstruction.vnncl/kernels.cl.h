/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef KERNELS_CLH_
#define KERNELS_CLH_

/**
 *  Original author Tord �ygard
 */

/*******************/
/* Begin compile time definitions */
/*******************/
//MAX_PLANES - number of planes to include in closest planes
//N_PLANES - number of planes/images (the z dimension)
//METHOD - the reconstruction method
//PLANE_METHOD - the method used to find the closes planes
//MAX_MULTISTART_STARTS - how many guesses the algorithm should produce as starting points for finding the closes planes
//NEWNESS_FACTOR - Newness weight, should newer (pixels in newer planes) be preferred
//BRIGHTNESS_FACTOR - Brightness weight, when selecting pixrls, should similarity in intensity count

/*******************/
/* End compile time definitions */
/*******************/

/*******************/
/* Begin constants */
/*******************/

#define CUBE_SIZE 4

#define N_BLOCKS 10

// Reconstruction methods
#define METHOD_VNN 0
#define METHOD_VNN2 1
#define METHOD_DW 2
#define METHOD_ANISOTROPIC 3

// Plane searching methods
#define PLANE_HEURISTIC 0
#define PLANE_CLOSEST 1

// Anisotropic method specific constants
#define ANISOTROPIC_SIZE 3
#define ANISOTROPIC_GAUSSIAN_SIZE 3

#define ANISOTROPIC_METHOD_INCLUDE_ALL 0
#define ANISOTROPIC_METHOD_BILINEAR_ON_PLANE 1
#define ANISOTROPIC_METHOD_GAUSSIAN_ON_PLANE 2

#define ANISOTROPIC_WEIGHT_METHOD_DISTANCE 0
#define ANISOTROPIC_WEIGHT_METHOD_BRIGHTNESS 1
#define ANISOTROPIC_WEIGHT_METHOD_LATENESS 2
#define ANISOTROPIC_WEIGHT_METHOD_BOTH 3

#define ANISOTROPIC_METHOD ANISOTROPIC_METHOD_BILINEAR_ON_PLANE

// Local search related parameters

//#define LOCAL_SEARCH_DISTANCE (N_PLANES/(MAX_MULTISTART_STARTS)) 
//#define LOCAL_SEARCH_DISTANCE 10
/*****************/
/* End constants */
/*****************/

/*****************/
/* Begin structs */
/*****************/

typedef struct _close_plane
{
	float dist;
	short plane_id;
	unsigned char intensity;
	unsigned char padding; // Align with 4
} close_plane_t;

typedef struct _output_volume_type
{
	int3 size;
	float3 spacing;
	__global unsigned char* volume;
} output_volume_type;

/***************/
/* End structs */
/***************/

/****************/
/* Begin macros */
/****************/

//#define DEBUG
#define CHECK_PLANE_INDICES

#ifdef DEBUG
	#define DEBUG_PRINTF(...) if((get_global_id(0) % 5000) == 0) printf(##__VA_ARGS__)
	//#define DEBUG_PRINTF(...) printf(##__VA_ARGS__)
	//#define BOUNDS_CHECK(x, min, ma x) if(x < min || x >= max) printf("Line %d: %s out of range: %d min: %d max: %d\n", __LINE__, #x, x, min, max)
	#define BOUNDS_CHECK(x, min, max)
#else
//	#define DEBUG_PRINTF(...)
	#define BOUNDS_CHECK(x, min, max)
#endif

//#define PLANE_DIST(voxel, matrix) (dot(matrix.s26AE,voxel) - dot(matrix.s26AE, matrix.s37BF))

#define EUCLID_DIST(a, b, c) sqrt((a)*(a) + (b)*(b) + (c)*(c))

#define PROJECTONTOPLANE(voxel, matrix, dist) (voxel - dist*(matrix.s26AE))

#define PROJECTONTOPLANEEQ(voxel, eq, dist) (voxel - dist*(eq))

#define ISINSIDE(x, size) ((x) >= 0 && (x) < (size))
#define ISNOTMASKED(x, y, mask, xsize) ((mask)[(x) + (y)*(xsize)] > 0)
//#define ISNOTMASKED(x, y, mask, xsize) true

#define VOXEL(v,x,y,z) v[x + y*volume_xsize + z*volume_ysize*volume_xsize]

#define WEIGHT_INV(x) (1.0f/fabs(x))
#define WEIGHT_INV2(x) (1.0f/fabs(x*x))
#define WEIGHT_INV4(x) (1.0f/fabs(x*x*x*x))
#define WEIGHT_SUB(x) (1.0f - fabs(x))

#define WEIGHT_TERNARY(val, mean, factor)                            \
    ((val) >= (mean) ? (factor) : 0.0f)

#define ANISOTROPIC_GAUSS_WEIGHT(px, var, mean, mean_id, sigma) WEIGHT_GAUSS(px.dist, sigma)

#ifndef ANISOTROPIC_WEIGHT_METHOD
	#define ANISOTROPIC_WEIGHT_METHOD ANISOTROPIC_WEIGHT_METHOD_BOTH
#endif

#ifndef BRIGHTNESS_FACTOR
	#define BRIGHTNESS_FACTOR 5.0f
#endif

#ifndef NEWNESS_FACTOR
	#define NEWNESS_FACTOR 5.0f
#endif

#define ANISOTROPIC_WEIGHT_BRIGHTNESS(px, var, mean, mean_id, sigma)    \
    ((WEIGHT_GAUSS(px.dist, sigma)) + (WEIGHT_TERNARY(px.intensity, mean, BRIGHTNESS_FACTOR)))

#define ANISOTROPIC_WEIGHT_LATENESS(px, var, mean, mean_id, sigma)      \
    ((WEIGHT_GAUSS(px.dist, sigma)) + (WEIGHT_TERNARY(px.plane_id, mean_id, NEWNESS_FACTOR)))

#define ANISOTROPIC_WEIGHT_BOTH(px, var, mean, mean_id, sigma) \
    ((WEIGHT_GAUSS(px.dist, sigma)) \
     + (WEIGHT_TERNARY(px.plane_id, mean_id, NEWNESS_FACTOR)) \
     + (WEIGHT_TERNARY(px.intensity, mean, BRIGHTNESS_FACTOR)))

#if ANISOTROPIC_WEIGHT_METHOD == ANISOTROPIC_WEIGHT_METHOD_DISTANCE
	#define ANISOTROPIC_WEIGHT(px, var, mean, mean_id, sigma) ANISOTROPIC_GAUSS_WEIGHT(px, var, mean, mean_id, sigma)
#elif ANISOTROPIC_WEIGHT_METHOD == ANISOTROPIC_WEIGHT_METHOD_BRIGHTNESS
	#define ANISOTROPIC_WEIGHT(px, var, mean, mean_id, sigma) ANISOTROPIC_WEIGHT_BRIGHTNESS(px, var, mean, mean_id, sigma)
#elif ANISOTROPIC_WEIGHT_METHOD == ANISOTROPIC_WEIGHT_METHOD_LATENESS
	#define ANISOTROPIC_WEIGHT(px, var, mean, mean_id, sigma) ANISOTROPIC_WEIGHT_LATENESS(px, var, mean, mean_id, sigma)
#elif ANISOTROPIC_WEIGHT_METHOD == ANISOTROPIC_WEIGHT_METHOD_BOTH
	#define ANISOTROPIC_WEIGHT(px, var, mean, mean_id, sigma) ANISOTROPIC_WEIGHT_BOTH(px, var, mean, mean_id, sigma)
#endif

// Gaussian weight function
#define WEIGHT_GAUSS_SIGMA (0.05f)

#define WEIGHT_GAUSS_SQRT_2PI 2.506628275f

#define WEIGHT_GAUSS_NONEXP_PART(sigma) (1.0f/((sigma)*WEIGHT_GAUSS_SQRT_2PI))
#define WEIGHT_GAUSS_EXP_PART(dist, sigma) exp(-((dist)*(dist))/(2*(sigma)*(sigma)))

#define WEIGHT_GAUSS(x, sigma) (WEIGHT_GAUSS_NONEXP_PART(sigma)*WEIGHT_GAUSS_EXP_PART(x, sigma))

#define DW_WEIGHT(x) WEIGHT_INV(x)
#define VNN2_WEIGHT(x) WEIGHT_INV(x)

#define CLOSE_PLANE_IDX(p, i) p[get_local_id(0)*(MAX_PLANES+1)+(i)]

#define CREATE_OUTPUT_VOLUME_TYPE(name, in_size, in_spacing, in_volume) \
        output_volume_type name; \
        name.size = in_size; \
        name.spacing = in_spacing; \
        name.volume = in_volume;

/**************/
/* End macros */
/**************/

/********************/
/* Begin prototypes */
/********************/

// Declare all the functions, as Apple seems to need that
//---------------------DEBUGGING-FUNCTIONALITY---------------------
#ifdef DEBUG
	void printMatrix(float16 matrix);
#endif /* DEBUG */

//---------------------DEBUGGING-FUNCTIONALITY---------------------

int isValidPixel(int2 point, const __global unsigned char* mask, int2 in_size);

int findHighestIdx(__local close_plane_t *planes, int n);

int2 findClosestPlanes_heuristic(__local close_plane_t *close_planes,
		__local float4* const plane_eqs,
		__global float16* const plane_matrices,
		const float4 voxel,
		const float radius,
		int guess,
		bool doTermDistance,
		__global const unsigned char* mask,
		int2 in_size,
		float2 in_spacing);

int2 findClosestPlanes_multistart(__local close_plane_t *close_planes,
		__local float4* const plane_eqs,
		__global float16* const plane_matrices,
		const float4 voxel,
		const float radius,
		int *multistart_guesses,
		int n_multistart_guesses,
		bool doTermDistance,
		__global const unsigned char* mask,
		int2 in_size,
		float2 in_spacing);

#if PLANE_METHOD == PLANE_EXACT
	#define FIND_CLOSE_PLANES(a, b, c, d, e, f, g, h, i, j) findClosestPlanes_multistart(a, b, c, d, e, f, g, 1, h, i, j)
#elif PLANE_METHOD == PLANE_CLOSEST
	#ifdef MAX_MULTISTART_STARTS
	#undef MAX_MULTISTART_STARTS
	#define MAX_MULTISTART_STARTS 1
	#endif

	#define FIND_CLOSE_PLANES(a, b, c, d, e, f, g, h, i, j) findClosestPlanes_multistart(a, b, c, d, e, f, g, 0, h, i, j)
#endif

__global const unsigned char* getImageData(int plane_id,
		__global const unsigned char* bscans_blocks[],
		int2 in_size);

float4 transform(float16 matrix, float4 voxel);

float4 transform_inv(float16 matrix, float4 voxel);

float2 transform_inv_xy(float16 matrix, float4 voxel);

int2 round_int(float2 value);

int2 toImgCoord_int(float4 voxel, float16 plane_matrix, float2 in_spacing);

float2 toImgCoord_float(float4 voxel, float16 plane_matrix, float2 in_spacing);

float bilinearInterpolation(float x,
		float y,
		const __global unsigned char* image,
		int in_xsize);

#if METHOD == METHOD_VNN
	#define PERFORM_INTERPOLATION(a, b, c, d, e, f, g, h, i)	  \
			performInterpolation_vnn(a, b, c, d, e, f, g, h, i)
#elif METHOD == METHOD_VNN2
	#define PERFORM_INTERPOLATION(a, b, c, d, e, f, g, h, i)	  \
			performInterpolation_vnn2(a, b, c, d, e, f, g, h, i)
#elif METHOD == METHOD_DW
	#define PERFORM_INTERPOLATION(a, b, c, d, e, f, g, h, i)	  \
			performInterpolation_dw(a, b, c, d, e, f, g, h, i)
#elif METHOD == METHOD_ANISOTROPIC
	#define PERFORM_INTERPOLATION(a, b, c, d, e, f, g, h, i)	  \
			performInterpolation_anisotropic(a, b, c, d, e, f, g, h, i)
#endif

unsigned char performInterpolation_vnn(__local close_plane_t *close_planes,
		int n_close_planes,
		__global const float16 *plane_matrices,
		__local const float4 *plane_eqs,
		__global const unsigned char* bscans_blocks[],
		int2 in_size,
		float2 in_spacing,
		__global const unsigned char* mask,
		float4 voxel);

unsigned char performInterpolation_vnn2(__local close_plane_t *close_planes,
		int n_close_planes,
		__global const float16 *plane_matrices,
		__local const float4 *plane_eqs,
		__global const unsigned char* bscans_blocks[],
		int2 in_size,
		float2 in_spacing,
		__global const unsigned char* mask,
		float4 voxel);

unsigned char performInterpolation_dw(__local close_plane_t *close_planes,
		int n_close_planes,
		__global const float16 *plane_matrices,
		__local const float4 *plane_eqs,
		__global const unsigned char* bscans_blocks[],
		int2 in_size,
		float2 in_spacing,
		__global const unsigned char* mask,
		float4 voxel);

unsigned char performInterpolation_anisotropic(__local close_plane_t *close_planes,
		int n_close_planes,
		__global const float16 *plane_matrices,
		__local const float4 *plane_eqs,
		__global const unsigned char* bscans_blocks[],
		int2 in_size,
		float2 in_spacing,
		__global const unsigned char* mask,
		float4 voxel);

unsigned char anisotropicFilter(__local const close_plane_t *pixels,
		int n_planes);

void prepare_plane_eqs(__global float16 *plane_matrices,
		__local float4 *plane_eqs);

int findLocalMinimas(int *guesses,
		__local const float4 *plane_eqs,
		float radius,
		float4 voxel,
		float3 out_spacing,
		__global const float16 *plane_matrices,
		__global const unsigned char *mask);

__kernel void voxel_methods(int volume_xsize,
		int volume_ysize,
		int volume_zsize,
		float volume_xspacing,
		float volume_yspacing,
		float volume_zspacing,
		int in_xsize,
		int in_ysize,
		float in_xspacing,
		float in_yspacing,
		// TODO: Wouldn't it be kind of nice if the bscans was an image sampler object?
		__global unsigned char* in_bscans_b0,
		__global unsigned char* in_bscans_b1,
		__global unsigned char* in_bscans_b2,
		__global unsigned char* in_bscans_b3,
		__global unsigned char* in_bscans_b4,
		__global unsigned char* in_bscans_b5,
		__global unsigned char* in_bscans_b6,
		__global unsigned char* in_bscans_b7,
		__global unsigned char* in_bscans_b8,
		__global unsigned char* in_bscans_b9,
		__global unsigned char* out_volume,
		__global float16 *plane_matrices,
		__global unsigned char* mask,
		__local float4 *plane_eqs,
		__local close_plane_t *planes,
		float radius);

/******************/
/* End prototypes */
/******************/

#endif /* KERNELS_CLH_ */
