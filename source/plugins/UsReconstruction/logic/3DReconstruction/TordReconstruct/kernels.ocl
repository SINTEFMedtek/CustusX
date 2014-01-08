

// At current, this is a temporary playground file for Tord

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


/****************/
/* Begin macros */
/****************/

//#define DEBUG
#define CHECK_PLANE_INDICES

#ifdef DEBUG
#define DEBUG_PRINTF(...) if((get_global_id(0) % 5000) == 0) printf(##__VA_ARGS__)
//#define DEBUG_PRINTF(...) printf(##__VA_ARGS__)
//#define BOUNDS_CHECK(x, min, max) if(x < min || x >= max) printf("Line %d: %s out of range: %d min: %d max: %d\n", __LINE__, #x, x, min, max)
#define BOUNDS_CHECK(x, min, max)

#else
#define DEBUG_PRINTF(...)
#define BOUNDS_CHECK(x, min, max)
#endif


#define plane_dist(voxel, matrix) (dot(matrix.s26AE,voxel) - dot(matrix.s26AE, matrix.s37BF))

#define euclid_dist(a, b, c) sqrt((a)*(a) + (b)*(b) + (c)*(c))

#define projectOntoPlane(voxel, matrix, dist) (voxel - dist*(matrix.s26AE))

#define projectOntoPlaneEq(voxel, eq, dist) (voxel - dist*(eq))

#define isInside(x, size) ((x) >= 0 && (x) < (size))
#define isNotMasked(x, y, mask, xsize) ((mask)[(x) + (y)*(xsize)] > 0)
//#define isNotMasked(x, y, mask, xsize) true

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

/**************/
/* End macros */
/**************/

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

/***************/
/* End structs */
/***************/

/********************/
/* Begin prototypes */
/********************/

// Declare all the functions, as Apple seems to need that


int isValidPixel(int x,
                 int y,
                 const __global unsigned char* mask,
                 int in_xsize,
                 int in_ysize);

int findHighestIdx(__local close_plane_t *planes, int n);

int2 findClosestPlanes_heuristic(__local close_plane_t *close_planes,
                            __local float4* const plane_eqs,
                            __global float16* const plane_matrices,
                            const float4 voxel,
                            const float radius,
                            int guess,
                            bool doTermDistance,
                            __global const unsigned char* mask,
                            int in_xsize,
                            int in_ysize,
                            float in_xspacing,
                            float in_yspacing);

int2 findClosestPlanes_multistart(__local close_plane_t *close_planes,
                             __local float4* const plane_eqs,
                             __global float16* const plane_matrices,
                             const float4 voxel,
                             const float radius,
                             int *multistart_guesses,
                             int n_multistart_guesses,
                             bool doTermDistance,
                             __global const unsigned char* mask,
                             int in_xsize,
                             int in_ysize,
                             float in_xspacing,
                             float in_yspacing);

#if PLANE_METHOD == PLANE_EXACT
#define FIND_CLOSE_PLANES(a, b, c, d, e, f, g, h, i, j, k, l) findClosestPlanes_multistart(a, b, c, d, e, f, g, 1, h, i, j, k, l)
#elif PLANE_METHOD == PLANE_CLOSEST

#ifdef MAX_MULTISTART_STARTS
#undef MAX_MULTISTART_STARTS
#define MAX_MULTISTART_STARTS 1
#endif

#define FIND_CLOSE_PLANES(a, b, c, d, e, f, g, h, i, j, k, l) findClosestPlanes_multistart(a, b, c, d, e, f, g, 0, h, i, j, k, l)
#endif

__global const unsigned char* getImageData(int plane_id,
             __global const unsigned char* bscans_blocks[],
             int in_xsize,
             int in_ysize);

float4 transform(float16 matrix, float4 voxel);

float4 transform_inv(float16 matrix, float4 voxel);

float2 transform_inv_xy(float16 matrix, float4 voxel);

void printMatrix(float16 matrix);

void toImgCoord_int(int* x,
               int* y,
               float4 voxel,
               float16 plane_matrix,
               float in_xspacing,
               float in_yspacing);

void toImgCoord_float(float* x,
                 float* y,
                 float4 voxel,
                 float16 plane_matrix,
                 float in_xspacing,
                 float in_yspacing);

float bilinearInterpolation(float x,
                      float y,
                      const __global unsigned char* image,
                      int in_xsize);

unsigned char anisotropicFilter(__local const close_plane_t *pixels,
                                int n_planes);

#if METHOD == METHOD_VNN
unsigned char performInterpolation_vnn(__local close_plane_t *close_planes,
                         int n_close_planes,
                         __global const float16  *plane_matrices,
                         __local const float4 *plane_eqs,
                         __global const unsigned char* bscans_blocks[],
                         int in_xsize,
                         int in_ysize,
                         float in_xspacing,
                         float in_yspacing,
                         __global const unsigned char* mask,
                         float4 voxel);
#endif

#if METHOD == METHOD_VNN2
unsigned char performInterpolation_vnn2(__local close_plane_t *close_planes,
                          int n_close_planes,
                          __global const float16  *plane_matrices,
                          __local const float4 *plane_eqs,
                          __global const unsigned char* bscans_blocks[],
                          int in_xsize,
                          int in_ysize,
                          float in_xspacing,
                          float in_yspacing,
                          __global const unsigned char* mask,
                          float4 voxel);
#endif

#if METHOD == METHOD_DW
unsigned char performInterpolation_dw(__local close_plane_t *close_planes,
                        int n_close_planes,
                        __global const float16  *plane_matrices,
                        __local const float4 *plane_eqs,
                        __global const unsigned char* bscans_blocks[],
                        int in_xsize,
                        int in_ysize,
                        float in_xspacing,
                        float in_yspacing,
                        __global const unsigned char* mask,
                        float4 voxel);
#endif

#if METHOD == METHOD_ANISOTROPIC
unsigned char performInterpolation_anisotropic(__local close_plane_t *close_planes,
                            int n_close_planes,
                            __global const float16  *plane_matrices,
                            __local const float4 *plane_eqs,
                            __global const unsigned char* bscans_blocks[],
                            int in_xsize,
                            int in_ysize,
                            float in_xspacing,
                            float in_yspacing,
                            __global const unsigned char* mask,
                            float4 voxel);
#endif

void prepare_plane_eqs(__global float16 *plane_matrices,
                  __local float4 *plane_eqs);


int findLocalMinimas(int *guesses,
                 __local const float4 *plane_eqs,
                 float radius,
                 float4 voxel,
                 float out_xspacing,
                 float out_yspacing,
                 float out_zspacing,
                 float in_xspacing,
                 float in_yspacing,
                 __global const float16 *plane_matrices,
                 __global const unsigned char *mask,
                 int in_xsize,
                 int in_ysize);


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



int isValidPixel(int x,
                 int y,
                 const __global unsigned char* mask,
                 int in_xsize,
                 int in_ysize)
{
#ifndef DEBUG
	return (isInside(x, in_xsize)
	        && isInside(y, in_ysize)
	        && isNotMasked(x, y, mask, in_xsize));
#else
	if((isInside(x, in_xsize)
	    && isInside(y, in_ysize)
	    && isNotMasked(x, y, mask, in_xsize)))
	{
		return 1;
	}
	else {
		//		DEBUG_PRINTF("Pixel %d, %d is not valid! Sizes: %d, %d\n",
		//           x, y, in_xsize, in_ysize);
		return 0;
	}
#endif

}


/**
 * Find the plane with the highest distance to the voxel
 * i.e. the plane with the highest absolute value of dist.
 * Return the index of that plane
 * @param *planes Pointer to first element of plane array
 * @param n size of array pointed to *planes
 */
int findHighestIdx(__local close_plane_t *planes,
               int n)
{
	int maxidx = 0;
	float maxval = -1.0f;
	planes = &CLOSE_PLANE_IDX(planes, 0);

	for(int i = 0; i < n; i++)
	{
		float abs = fabs(planes->dist);
		if(abs > maxval)
		{
			maxidx = i;
			maxval = abs;
		}
		planes++;
	}
	//	DEBUG_PRINTF("New maxidx: %d maxdist = %f\n", maxidx, maxval);
	BOUNDS_CHECK(maxidx, 0, MAX_PLANES);
	return maxidx;
}

int2 findClosestPlanes_multistart(__local close_plane_t *close_planes,
                            __local float4* const plane_eqs,
                            __global float16* const plane_matrices,
                            const float4 voxel,
                            const float radius,
                            int *multistart_guesses,
                             int n_multistart_guesses,
                             bool doTermDistance,
                             __global const unsigned char* mask,
                            int in_xsize,
                            int in_ysize,
                            float in_xspacing,
                            float in_yspacing)
{
	close_plane_t  tmp;
	tmp.dist = INFINITY;
	tmp.plane_id = -1;
	for(int i = 0; i < MAX_PLANES; i++)
	{
		CLOSE_PLANE_IDX(close_planes, i) = tmp;
	}

	int2 ret;
	int found = 0;
	for(int i = 0; i < n_multistart_guesses; i++)
	{
		ret = findClosestPlanes_heuristic(close_planes,
		                                  plane_eqs,
		                                  plane_matrices,
		                                  voxel,
		                                  radius,
		                                  multistart_guesses[i],
		                                  doTermDistance,
		                                  mask,
		                                  in_xsize,
		                                  in_ysize,
		                                  in_xspacing,
		                                  in_yspacing	);
		if(ret.x > 0)
		{
			multistart_guesses[i] = ret.y;
		}
		found += ret.x;

	}

	ret.x = min(found, MAX_PLANES);
	ret.y = 0;

#ifdef DEBUG
#ifdef CHECK_PLANE_INDICES
	for(int i = 0; i < min(found, MAX_PLANES); i++)
	{
		BOUNDS_CHECK(CLOSE_PLANE_IDX(close_planes, i).plane_id, 0, N_PLANES);
	}
#endif
#endif

	return ret;
}

/**
 * Find planes that are within radius of voxel.
 * Search in both directions in the plane array, starting at guess
 * The assumption is that as you move away from the guess,
 * the distance to this voxel will increase. That assumption may not always be true, for instance
 * if the US probe was swept back and forth.
 * Finds the closest MAX_PLANES planes within radius,
 * provided no plane with distance greater than
 * 2x radius is found before any of the MAX_PLANES closest planes.
 */
int2 findClosestPlanes_heuristic(__local close_plane_t *close_planes,
                            __local float4* const plane_eqs,
                            __global float16* const plane_matrices,
                            const float4 voxel,
                            const float radius,
                            int guess,
                            bool doTermDistance,
                            __global const unsigned char* mask,
                            int in_xsize,
                            int in_ysize,
                            float in_xspacing,
                            float in_yspacing)
{

	
	// Number of planes found so far
	int found = 0;

	// Done condition. .x  = up, .y = down
	int2 done = {0,0};

	// The index of the plane with the smallest distance found so far
	int smallest_idx = guess;

	float term_condition = clamp(fabs(dot(voxel, plane_eqs[guess])), radius, 3*radius);

	// The smallest distance found so far
	float smallest_dist = 99999.9f;

	// The index of the plane with the biggest index so far
	int max_idx = findHighestIdx(close_planes, MAX_PLANES);

	// The biggest distance found so far
	float max_dist = min(fabs(CLOSE_PLANE_IDX(close_planes, max_idx).dist), radius);

	close_plane_t tmp;
	tmp.intensity = 0;

	// If guess is 0, we will try to access data for plane id -1, which does not exist.
	// Assume plane 1 is close enough in that case
	if(guess == 0) guess = 1;

	// We won't be changing the guess, but the compiler wouldn't know that
	const int tmp_guess = guess;
	BOUNDS_CHECK(tmp_guess, 1, N_PLANES);
	float2 dists = {dot(voxel, plane_eqs[guess]), dot(voxel, plane_eqs[guess-1])};
	float2 abs_dists = {fabs(dists.x), fabs(dists.y)}; // .x = abs_dist_up, .y = abs_dist_down,

	for(int i = 0; !done.x || !done.y ; i++)
	{				
		// Compute the indices of the planes we want to look at.
		int2 idx = {tmp_guess + i, tmp_guess - i - 1};
		
		//SINTEF BUGFIX?
		if(idx.y <=0)
			idx.y = 0;
		if(idx.x >= N_PLANES-1)
			idx.x = N_PLANES-1;
		
		//float2 prev_abs_dists = abs_dists;

		// Compute the distances to those planes
		dists.x = dot(voxel, plane_eqs[idx.x]);
		dists.y = dot(voxel, plane_eqs[idx.y]);
		// Compute the absolute distances to those planes
		abs_dists.x = fabs(dists.x);
		abs_dists.y = fabs(dists.y); // .x = abs_dist_up, .y = abs_dist_down,


		//float2 diff_dists = prev_abs_dists - abs_dists;

		// Check if the plane is closer than the one farthest away we have included so far
		if(!done.x && abs_dists.x < max_dist)
		{
			BOUNDS_CHECK(idx.x, 0, N_PLANES);
			BOUNDS_CHECK(max_idx, 0, MAX_PLANES);
			int px, py;
			float4 translated_voxel = projectOntoPlaneEq(voxel,
			                                           plane_eqs[idx.x],
			                                           dists.x);
			toImgCoord_int(&px,
			               &py,
			               translated_voxel,
			               plane_matrices[idx.x],
			               in_xspacing,
			               in_yspacing);

			if(isValidPixel(px, py, mask, in_xsize, in_ysize))
			{

				// If yes, swap out the one with the longest distance for this plane
				tmp.dist = dists.x;
				tmp.plane_id = idx.x;
				CLOSE_PLANE_IDX(close_planes, max_idx) = tmp;
				found++;

				// We have found MAX_PLANES planes, but we don't know they're the closest ones.
				// Find the next candidate for eviction -
				// the plane with the longest distance to the voxel
				max_idx = findHighestIdx(close_planes, MAX_PLANES);
				max_dist = min(fabs(CLOSE_PLANE_IDX(close_planes, max_idx).dist), radius);

				if(smallest_dist > abs_dists.x)
				{
					// Update next guess
					smallest_dist = abs_dists.x;
					smallest_idx = idx.x;
				}
			}
		}

		// And the same in the down direction
		// Check if the plane is closer than the one farthest away we have included so far
		if(!done.y && abs_dists.y < max_dist)
		{
			BOUNDS_CHECK(idx.y, 0, N_PLANES);
			BOUNDS_CHECK(max_idx, 0, MAX_PLANES);
			// If yes, swap out the one with the longest distance for this plane
			int px, py;
			float4 translated_voxel = projectOntoPlaneEq(voxel,
			                                           plane_eqs[idx.y],
			                                           dists.y);

			toImgCoord_int(&px,
			               &py,
			               translated_voxel,
			               plane_matrices[idx.y],
			               in_xspacing,
			               in_yspacing);
			if(isValidPixel(px, py, mask, in_xsize, in_ysize))
			{
				tmp.dist = dists.y;
				tmp.plane_id = idx.y;
				CLOSE_PLANE_IDX(close_planes, max_idx) = tmp;

				found++;
				max_idx = findHighestIdx(close_planes, MAX_PLANES);
				max_dist = min(fabs(CLOSE_PLANE_IDX(close_planes, max_idx).dist), radius);
				if(smallest_dist > abs_dists.y)
				{
					// Update next guess
					smallest_dist = abs_dists.y;
					smallest_idx = idx.y;
				}
			}
		}


		// FIXME: The constant here represents a tradeoff between
		// guaranteeing to find all relevant planes, and performance (terminating earlier).
		// This should be investigate further.

		int2 term_dists = {(abs_dists.x > term_condition)*doTermDistance, (abs_dists.y > term_condition)*doTermDistance };

		//int2 term_radius_jump = {fabs(diff_dists.x) > radius, fabs(diff_dists.y) > radius};
		int2 term_boundaries = {idx.x == N_PLANES-1, idx.y == 0};

		done = done + term_dists + term_boundaries;// + term_radius_jump;
	}

	int2 ret;
	ret.x = min(found, MAX_PLANES);
	ret.y = smallest_idx;
	return ret;
}


/**
 * Get a pointer to the first pixel of the image frame given by plane_id
 */
__global const unsigned char*
getImageData(int plane_id,
             __global const unsigned char* bscans_blocks[],
             int in_xsize,
             int in_ysize)
{
	int scans_per_block = N_PLANES / N_BLOCKS;
	int n_big_blocks = N_PLANES % N_BLOCKS;
	int block, idx_in_block;
	// Check if plane is in one of the big blocks
	if(plane_id < n_big_blocks * (scans_per_block+1))
	{
		// It's in a big block
		block = plane_id / (scans_per_block+1);
		idx_in_block = plane_id % (scans_per_block+1);
		BOUNDS_CHECK(idx_in_block, 0, scans_per_block+1);
	}
	else
	{
		// It's in a small block
		int index_into_small_blocks = plane_id - (n_big_blocks * (scans_per_block+1));
		idx_in_block = index_into_small_blocks % scans_per_block;
		block = n_big_blocks + (index_into_small_blocks / scans_per_block);
		BOUNDS_CHECK(idx_in_block, 0, scans_per_block);
	}
	BOUNDS_CHECK(block, 0, 10);

	return &bscans_blocks[block][idx_in_block*in_xsize*in_ysize];
}

/**
 * Perform a standard forward transformation of voxel - eqvivalent to multiplying matrix with voxel
 */
float4
transform(float16 matrix,
          float4 voxel)
{
	float4 ret;

	ret.x = matrix.s0*voxel.x + matrix.s1*voxel.y + matrix.s2*voxel.z + matrix.s3;
	ret.y = matrix.s4*voxel.x + matrix.s5*voxel.y + matrix.s6*voxel.z + matrix.s7;
	ret.z = matrix.s8*voxel.x + matrix.s9*voxel.y + matrix.sA*voxel.z + matrix.sB;

	ret.w = 1.0f;
	return ret;

}

/**
 * Perform an inverse transformation of voxel - eqvivalent to multiplying the inverse of the matrix with voxel
 */
float4 transform_inv(float16 matrix,
                     float4 voxel)
{
	float4 ret;
	float4 col0 = matrix.s048C;
	float4 col1 = matrix.s159D;
	float4 col2 = matrix.s26AE;
	float4 col3 = matrix.s37BF;

	ret.x = dot(voxel,col0) - dot(col3,col0);
	ret.y = dot(voxel,col1) - dot(col3,col1);
	ret.z = dot(voxel,col2) - dot(col3,col2);
	ret.w = 1.0f;
	return ret;
}

/**
 * Perform an inverse transformation of voxel, as in transform_inv, but only transform the x and y coordinates. This is useful
 * when finding image coordinates.
 */
float2 transform_inv_xy(float16 matrix, float4 voxel)
{
	float2 ret;
	float4 col0 = matrix.s048C;
	float4 col1 = matrix.s159D;
	float4 col3 = matrix.s37BF;

	ret.x = dot(voxel,col0) - dot(col3,col0);
	ret.y = dot(voxel,col1) - dot(col3,col1);
	return ret;
}

/**
 * Print a matrix - useful for debugging
 */
#ifdef DEBUG
void printMatrix(float16 matrix)
{
	printf("[ %f, %f, %f, %f\n   %f, %f, %f, %f\n   %f, %f, %f, %f\n   %f, %f, %f, %f\n",
	       matrix.s0,
	       matrix.s1,
	       matrix.s2,
	       matrix.s3,
	       matrix.s4,
	       matrix.s5,
	       matrix.s6,
	       matrix.s7,
	       matrix.s8,
	       matrix.s9,
	       matrix.sA,
	       matrix.sB,
	       matrix.sC,
	       matrix.sD,
	       matrix.sE,
	       matrix.sF);
}
#endif

/**
 * Transform to integer image coordinates - i.e. pixel coordinates
 */
void toImgCoord_int(int* x,
                    int* y,
                    float4 voxel,
                    float16 plane_matrix,
                    float in_xspacing,
                    float in_yspacing)
{

	float2 transformed_voxel = transform_inv_xy(plane_matrix, voxel);

	*x = ((transformed_voxel.x/in_xspacing) + 0.5f);
	*y = ((transformed_voxel.y/in_yspacing) + 0.5f);
}

/**
 * Transform to floating point image coordinates
 */
void toImgCoord_float(float* x,
                float* y,
                float4 voxel,
                float16 plane_matrix,
                float in_xspacing,
                float in_yspacing)
{

	float2 transformed_voxel = transform_inv_xy(plane_matrix, voxel);

	*x = ((transformed_voxel.x/in_xspacing));
	*y = ((transformed_voxel.y/in_yspacing));
}


/**
 * Perform bilinear interpolation to retrieve an interpolated pixel value from an image.
 */
float bilinearInterpolation(float x,
                            float y,
                            const __global unsigned char* image,
                            int in_xsize)
{

	// SOURCE: https://en.wikipedia.org/w/index.php?title=Bilinear_interpolation&oldid=574742881 (need better source for report)

	int2 pos = {x, y};
	float2 offset = {x - pos.x, y - pos.y};

	float4 values = { image[pos.x + pos.y*in_xsize],        // Lower left
	                  image[pos.x+1 + pos.y*in_xsize],      // Lower right
	                  image[pos.x + 1 + (pos.y+1)*in_xsize],// Upper right
	                  image[pos.x + (pos.y+1)*in_xsize] };  // Upper left

	float4 weights = { (1.0f - offset.x)*(1.0f - offset.y), // Lower left
	                   (offset.x)*(1.0f - offset.y),        // Lower right
	                   (offset.x)*(offset.y),               // Upper right
	                   (1.0f - offset.x)*(offset.y) };      // Upper left

	return dot(values, weights);

}



#if METHOD == METHOD_VNN
#define PERFORM_INTERPOLATION(a, b, c, d, e, f, g, h, i , j, k)	  \
	performInterpolation_vnn(a, b, c, d, e, f, g, h, i, j, k)

/**
 * Perform interpolation using the Voxel Nearest Neighbour method.
 * This works by taking finding the plane closest to the voxel,
 * projecting the voxel orthogonally onto the image plane to find pixel coordinates
 * and taking the pixel value
 */
unsigned char
performInterpolation_vnn(__local close_plane_t *close_planes,
                         int n_close_planes,
                         __global const float16  *plane_matrices,
                         __local const float4 *plane_eqs,
                         __global const unsigned char* bscans_blocks[],
                         int in_xsize,
                         int in_ysize,
                         float in_xspacing,
                         float in_yspacing,
                         __global const unsigned char* mask,
                         float4 voxel)
{
	if(n_close_planes == 0) return 1;

	int plane_id = 0;
	float lowest_dist = 10.0f;
	int close_plane_id = 0;
	close_plane_t plane;
	// Find the closest plane
	for(int i = 0; i < n_close_planes;  i++)
	{
		plane = CLOSE_PLANE_IDX(close_planes, i);
		float fabs_dist = fabs(plane.dist);
		if(fabs_dist < lowest_dist)
		{
			lowest_dist = fabs_dist;
			plane_id = plane.plane_id;
			close_plane_id = i;
		}
	}
	BOUNDS_CHECK(plane_id, 0, N_PLANES);
	const __global unsigned char* image = getImageData(plane_id,
	                                                   bscans_blocks,
	                                                   in_xsize,
	                                                   in_ysize);

	// Now we project the voxel onto the plane by translating the voxel along the
	// normal vector of the plane.
	float4 translated_voxel = projectOntoPlane(voxel,
	                                           plane_matrices[plane_id],
	                                           CLOSE_PLANE_IDX(close_planes ,close_plane_id).dist);
	translated_voxel.w = 1.0f;


	// And then we get the pixel space coordinates
	int x, y;
	toImgCoord_int(&x,
	               &y,
	               translated_voxel,
	               plane_matrices[plane_id],
	               in_xspacing,
	               in_yspacing);

	if(!isValidPixel(x,y, mask, in_xsize, in_ysize))
	{
		return 1;
	}
	BOUNDS_CHECK(x, 0, in_xsize);
	BOUNDS_CHECK(y, 0, in_ysize);
	return max((unsigned char)1, image[y*in_xsize + x]);

}
#endif

#if METHOD == METHOD_VNN2
#define PERFORM_INTERPOLATION(a, b, c, d, e, f, g, h, i ,j, k)	  \
	performInterpolation_vnn2(a, b, c, d, e, f, g, h, i, j, k)

/**
 * Perform interpolation using the VNN2 method. For each close plane, add (1/dist)*closest_pixel_value to the sum.
 * In the end, divide sum by sum(1/dist), and you have your voxel value.
 */
unsigned char
performInterpolation_vnn2(__local close_plane_t *close_planes,
                          int n_close_planes,
                          __global const float16  *plane_matrices,
                          __local const float4 *plane_eqs,
                          __global const unsigned char* bscans_blocks[],
                          int in_xsize,
                          int in_ysize,
                          float in_xspacing,
                          float in_yspacing,
                          __global const unsigned char* mask,
                          float4 voxel)
{
	if(n_close_planes == 0) return 1;


	float scale = 0.0f;

	float val = 0;
	for(int i = 0; i < n_close_planes; i++)
	{
		close_plane_t plane = CLOSE_PLANE_IDX(close_planes, i);
		int plane_id = plane.plane_id;
		const __global unsigned char* image = getImageData(plane_id,
		                                                   bscans_blocks,
		                                                   in_xsize,
		                                                   in_ysize);


		// Now we project the voxel onto the plane by translating the voxel along the
		// normal vector of the plane.
		voxel.w = 1.0f;
		float4 translated_voxel = projectOntoPlaneEq(voxel,
		                                             plane_eqs[plane_id],
		                                             plane.dist);

		translated_voxel.w = 1.0f;
		// And then we get the pixel space coordinates
		int x, y;
		toImgCoord_int(&x,
		               &y,
		               translated_voxel,
		               plane_matrices[plane_id],
		               in_xspacing,
		               in_yspacing);

		if(!isValidPixel(x,y, mask, in_xsize, in_ysize))
		{
			continue;
		}
		float dist = fabs(plane.dist);

		if(dist < 0.001f)
			dist = 0.001f;
		float weight = VNN2_WEIGHT(dist);

		scale += weight;
		val += (image[y*in_xsize + x] * weight);
	}


	return max((unsigned char)1, (unsigned char) (val / scale));

}
#endif

#if METHOD == METHOD_DW
#define PERFORM_INTERPOLATION(a, b, c, d, e, f, g, h, i ,j, k)	  \
	performInterpolation_dw(a, b, c, d, e, f, g, h, i, j, k)

/**
 * Perform interpolation using the DW method. Works the same as VNN2, but instead of taking the closest pixel on each image plane,
 * the value from each plane is a bilinearly interpolated from that plane.
 */
unsigned char
performInterpolation_dw(__local close_plane_t *close_planes,
                        int n_close_planes,
                        __global const float16  *plane_matrices,
                        __local const float4 *plane_eqs,
                        __global const unsigned char* bscans_blocks[],
                        int in_xsize,
                        int in_ysize,
                        float in_xspacing,
                        float in_yspacing,
                        __global const unsigned char* mask,
                        float4 voxel)
{


	if(n_close_planes == 0) return 1;


	float scale = 0.0f;

	float val = 0;
	for(int i = 0; i < n_close_planes; i++)
	{
		close_plane_t plane = CLOSE_PLANE_IDX(close_planes, i);
		int plane_id = plane.plane_id;
		const __global unsigned char* image = getImageData(plane.plane_id,
		                                                   bscans_blocks,
		                                                   in_xsize,
		                                                   in_ysize);


		// Now we project the voxel onto the plane by translating the voxel along the
		// normal vector of the plane.
		voxel.w = 1.0f;
		float4 translated_voxel = projectOntoPlaneEq(voxel,
		                                             plane_eqs[plane_id],
		                                             plane.dist);

		translated_voxel.w = 1.0f;
		// And then we get the pixel space coordinates
		float x, y;
		toImgCoord_float(&x,
		           &y,
		           translated_voxel,
		           plane_matrices[plane_id],
		           in_xspacing,
		           in_yspacing);


		int ix, iy;
		ix = x;
		iy = y;
		if(!isValidPixel(ix,iy, mask, in_xsize, in_ysize)
		   || !isValidPixel(ix+1, iy, mask, in_xsize, in_ysize)
		   || !isValidPixel(ix+1, iy+1, mask, in_xsize, in_ysize)
		   || !isValidPixel(ix, iy+1, mask, in_xsize, in_ysize)
			)
		{
			continue;
		}

		float interpolated_value = bilinearInterpolation(x, y, image, in_xsize);

		float dist = fabs(plane.dist);
		if(dist < 0.001f) dist = 0.001f;
		float weight = DW_WEIGHT(dist);
		scale += weight;
		val += (interpolated_value * weight);
	}


	return max((unsigned char)1, (unsigned char) (val / scale));

}
#endif

#if METHOD == METHOD_ANISOTROPIC
#define PERFORM_INTERPOLATION(a, b, c, d, e, f, g, h, i ,j, k)	  \
	performInterpolation_anisotropic(a, b, c, d, e, f, g, h, i, j, k)

/**
 * Perform interpolation using an anisotropic filter
 */
unsigned char
performInterpolation_anisotropic(__local close_plane_t *close_planes,
                            int n_close_planes,
                            __global const float16  *plane_matrices,
                            __local const float4 *plane_eqs,
                            __global const unsigned char* bscans_blocks[],
                            int in_xsize,
                            int in_ysize,
                            float in_xspacing,
                            float in_yspacing,
                            __global const unsigned char* mask,
                            float4 voxel)
{


	if(n_close_planes == 0)
	{
		return 1;
	}
	for(int i = 0; i < n_close_planes; i++)
	{
		close_plane_t plane = CLOSE_PLANE_IDX(close_planes ,i);
		const int plane_id = plane.plane_id;
		const __global unsigned char* image = getImageData(plane_id,
		                                             bscans_blocks,
		                                             in_xsize,
		                                             in_ysize);

		// Project onto plane
		voxel.w = 1.0f;
		float4 translated_voxel = projectOntoPlaneEq(voxel,
		                                             plane_eqs[plane_id],
		                                             plane.dist);
		translated_voxel.w = 1.0f;

		float x, y;
		toImgCoord_float(&x,
		                 &y,
		                 translated_voxel,
		                 plane_matrices[plane_id],
		                 in_xspacing,
		                 in_yspacing);


		int ix, iy;
		ix = x;
		iy = y;
		if(!isValidPixel(ix,iy, mask, in_xsize, in_ysize)
		   || !isValidPixel(ix+1, iy, mask, in_xsize, in_ysize)
		   || !isValidPixel(ix+1, iy+1, mask, in_xsize, in_ysize)
		   || !isValidPixel(ix, iy+1, mask, in_xsize, in_ysize)
				)
			{
			continue;
		}
		CLOSE_PLANE_IDX(close_planes, i).intensity = bilinearInterpolation(x,
		                                                                   y,
		                                                                   image,
		                                                                   in_xsize);
	}

	return max((unsigned char)1, anisotropicFilter(close_planes, n_close_planes));

}



unsigned char anisotropicFilter(__local const close_plane_t *pixels,
                                int n_planes)
{
	// Calculate the variance

	float mean_value = 0.0f;
	int sum_ids = 0.0f;
	close_plane_t tmp;
	for(int i = 0; i < n_planes; i++)
	{
		tmp = CLOSE_PLANE_IDX(pixels, i);
		mean_value += tmp.intensity;
		sum_ids += tmp.plane_id;
	}
	float mean_id = (float)sum_ids / n_planes;
	mean_value = mean_value / n_planes;

	float variance = 0.0f;
	for(int i = 0; i < n_planes; i++)
	{
		float tmp = CLOSE_PLANE_IDX(pixels, i).intensity - mean_value;
		variance += mad(tmp, tmp, variance);
 	}

	// We want high variance regions to have a sharp weight function
	// and small variance regions to have a smooth weight function.

	variance = clamp(variance/(n_planes-1), 1.0f, 10000.0f);
	float gauss_sigma = mean_value/sqrt(variance);

#ifdef DEBUG
	if(variance > 0.1f && mean_value> 10.0f)
		DEBUG_PRINTF("Mean: %f, variance: %f, sigma: %f\n", mean_value, variance, gauss_sigma);
#endif

	float sum_weights = 0.0f;
	float sum = 0.0f;
	// Use the resulting gauss sigma to calcualte weights
	for(int i = 0; i < n_planes; i++)
	{
		tmp = CLOSE_PLANE_IDX(pixels, i);
		float weight = ANISOTROPIC_WEIGHT(tmp, variance, mean_value, mean_id, gauss_sigma);
		sum = mad(tmp.intensity,weight, sum);
		sum_weights += weight;
	}
	return sum / sum_weights;
}



#endif
/**
 * Build the plane equations from the matrices and store them in local memory
 */
void
prepare_plane_eqs(__global float16 *plane_matrices,
                 __local float4 *plane_eqs)
{
	int id = get_local_id(0);
	int max_local_id = get_local_size(0);
	const int n_planes_pr_thread = (N_PLANES / max_local_id) + 1;

	for(int i = 0; i < n_planes_pr_thread; i++)
	{
		int idx = i + n_planes_pr_thread * id;
		if(idx >= N_PLANES) break;
		plane_eqs[idx].xyz = plane_matrices[idx].s26A;
		plane_eqs[idx].w = -dot(plane_matrices[idx].s26AE, plane_matrices[idx].s37BF);
	}
	barrier(CLK_LOCAL_MEM_FENCE);
}

int findLocalMinimas(int *guesses,
                     __local const float4 *plane_eqs,
                     float radius,
                     float4 voxel,
                     float out_xspacing,
                     float out_yspacing,
                     float out_zspacing,
                     float in_xspacing,
                     float in_yspacing,
                     __global const float16 *plane_matrices,
                     __global const unsigned char *mask,
                     int in_xsize,
                     int in_ysize)

{
	// Find all valleys in the search space of distances.
	// We don't need the _exact_ minima, however it should be inside the sweep we want.
	// However, the input data are noisy, so local minima in its strictest sense does not work for us.
	// But if we can find two indices a and b, such that dist(i) < dist(a) and dist(i) < dist(b)
	// and b - a = LOCAL_SEARCH_DISTANCE, it's a good chance it's a minima.

	int nMinima = 1;

	// Now with the cube-ish way of doing things, we may simply find all guesses that are closer than CUBE_SIZE * voxel_scale
	float max_dist = euclid_dist(out_xspacing * CUBE_SIZE, out_zspacing*CUBE_SIZE, out_yspacing*CUBE_SIZE) +  radius;
	DEBUG_PRINTF("Max dist is %f\n", max_dist);
	int prev_pos = 0;
	//float smallest_dist = fabs(dot(voxel, plane_eqs[0]));
	guesses[0] = 0;
	int hasHighSinceLastTaken = 1;
	for(int i = 0;
	    i < N_PLANES;
	    i++)
	{
		float dist = fabs(dot(voxel, plane_eqs[i]));
		if(dist < max_dist)
		{

			// We are inside a local minima. Now, how do we know we haven't found this minima before?
			// We require that they be spaced by at least LOCAL_SEARCH_DISTANCE.
			// However, if this minima is better (i.e. closer) than the previous one
			// inside LOCAL_SEARCH_DISTANCE,
			// of course we want to use this one.
			if(!hasHighSinceLastTaken)
			{
				DEBUG_PRINTF("Minima %d: Found nearby minima: %d : %f\n", nMinima, i, dist);
				// We have a previous minima, and it's too close.
				float prev_dist = fabs(dot(plane_eqs[guesses[prev_pos]], voxel));
				if(dist < prev_dist)
				{
					DEBUG_PRINTF("Taking it\n");
					// But this one is better, lets use it
					guesses[prev_pos] = i;
					hasHighSinceLastTaken = 0;
				}
			}
			else if(nMinima < MAX_MULTISTART_STARTS)
			{
				// We may simply store this minima
				DEBUG_PRINTF("Minima %d: Found new minima: %d : %f\n", nMinima, i, dist);
				guesses[nMinima] = i;
				prev_pos = nMinima;
				hasHighSinceLastTaken = 0;
				nMinima++;
			}
			else {
				// We already have MAX_MULTISTART_STARTS minimas, so now pick the "worst" minima
				// and toss it out for this one

				float biggest = -INFINITY;
				float tmp;
				int biggest_idx = 0;
				hasHighSinceLastTaken = 0;
				for(int j = 0; j < nMinima; j++)
				{
					tmp = fabs(dot(plane_eqs[guesses[j]], voxel));
					if(tmp > biggest)
					{
						biggest_idx = j;
						biggest = tmp;
					}
				}
				if(biggest > dist)
				{
					DEBUG_PRINTF("Switching out %d for %d: %f vs %f\n", biggest_idx, i, biggest, dist);
					guesses[biggest_idx] = i;
					prev_pos = biggest_idx;
				}
			}
		}
		else
		{
			hasHighSinceLastTaken = 1;
		}
	}

	DEBUG_PRINTF("Found %d minima in total\n", nMinima);

	return nMinima;
}

/** The entry point for this set of reconstruction methods.
 * Parameters:
 * @param volume_xsize Size of output volume, X direction
 * @param volume_ysize Size of output volume, Y direction
 * @param volume_zsize Size of output volume, Z direction
 * @param volume_xspacing Voxel size of output volume, X direction
 * @param volume_yspacing Voxel size of output volume, Y direction
 * @param volume_zspacing Voxel size of output volume, Z direction
 * @param in_xsize Size of each ultrasound input image in pixels, X direction
 * @param in_ysize Size of each ultrasound input image in pixels, Y direction
 * @param in_xspacing Size of each pixel in input ultrasound images, X direction
 * @param in_yspacing Size of each pixel in input ultrasound images, Y direction
 * @param in_bscans_b_ Ultrasound input images
 * @param out_volume Output volume - reconstructed volume goes here
 * @param plane_matrices One matrix per image plane specifying the transform from pixel space to voxel space
 * @param plane_eqs Pointer to local memory where we will store plane equations
 * @param radius The radius of the kernel - how far away to accept voxels from.
 */
__kernel void
voxel_methods(int volume_xsize,
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
              __global unsigned char *mask,
              __local float4 *plane_eqs,
              __local close_plane_t *close_planes,
              float radius
	)
{

	int id = get_global_id(0);

	int xcubes = (volume_xsize / CUBE_SIZE) + 1;
	int ycubes = (volume_ysize / CUBE_SIZE) + 1;

	int x_cube_id = id % xcubes;
	int y_cube_id = (id / xcubes) % ycubes;
	int z_cube_id = (id / (xcubes*ycubes));

	int x_origin = x_cube_id * CUBE_SIZE;
	int y_origin = y_cube_id * CUBE_SIZE;
	int z_origin = z_cube_id * CUBE_SIZE;
	/*	int x_origin = ((id % (volume_xsize / CUBE_SIZE)) + 1) * CUBE_SIZE;
	int y_origin = (((id / (volume_xsize / CUBE_SIZE) + 1) % ((volume_ysize / CUBE_SIZE) + 1))) * CUBE_SIZE;
	int z_origin = (((id / (volume_xsize / CUBE_SIZE) + 1) / ((volume_ysize / CUBE_SIZE) + 1))) * CUBE_SIZE; */


	#ifdef DEBUG
	if(id == 5000)
		BOUNDS_CHECK(id, 0, 1);
	#endif
	// Aggregate pointers to the bscan blocks into one array for convenience

	const __global unsigned char *bscans_blocks[] = { in_bscans_b0,
	                                            in_bscans_b1,
	                                            in_bscans_b2,
	                                            in_bscans_b3,
	                                            in_bscans_b4,
	                                            in_bscans_b5,
	                                            in_bscans_b6,
	                                            in_bscans_b7,
	                                            in_bscans_b8,
	                                            in_bscans_b9 };



	int n_close_planes;

	/* float4 voxel = {(x_origin + CUBE_SIZE/2) * volume_xspacing, */
	/*                 (y_origin + CUBE_SIZE/2) * volume_yspacing, */
	/*                 (z_origin + CUBE_SIZE/2)* volume_zspacing, */
	/*                 1.0f}; */

	float4 voxel = {(x_origin) * volume_xspacing,
	                (y_origin) * volume_yspacing,
	                (z_origin)* volume_zspacing,
	                1.0f};


	prepare_plane_eqs(plane_matrices, plane_eqs);


	// Return if x/z is invalid

	if(z_origin >= volume_zsize) return;
	if(x_origin >= volume_xsize) return;
	if(y_origin >= volume_ysize) return;

	BOUNDS_CHECK(x_origin, 0, volume_xsize);
	BOUNDS_CHECK(y_origin, 0, volume_ysize);
	BOUNDS_CHECK(z_origin, 0, volume_zsize);

	int multistart_guesses[MAX_MULTISTART_STARTS];

	int nGuesses = findLocalMinimas(multistart_guesses,
	                                plane_eqs,
	                                radius,
	                                voxel,
	                                volume_xspacing,
	                                volume_yspacing,
	                                volume_zspacing,
	                                in_xspacing,
	                                in_yspacing,
	                                plane_matrices,
	                                mask,
	                                in_xsize,
	                                in_ysize);

#ifdef DEBUG
	for(int i = 0; i < nGuesses; i++)
	{
		DEBUG_PRINTF("Multistart %d: idx %d dist %f\n",i,
		             multistart_guesses[i],
		             fabs(dot(voxel, plane_eqs[multistart_guesses[i]])));
	}
#endif

/* #ifdef DEBUG */
/* 	int hasSmaller = 0; */
/* 	for(int i = 0; i < nGuesses; i++) */
/* 	{ */
/* 		if(fabs(dot(voxel, plane_eqs[multistart_guesses[i]])) < radius) */
/* 			hasSmaller = 1; */
/* 	} */
/* 	if(hasSmaller == 0) */
/* 	{ */
/* 		printf("Voxel %d, %d, %d has no close guess. Guesses:\n", */
/* 		       x_origin, */
/* 		       y_origin, */
/* 		       z_origin); */
/* 		for(int i = 0; i < nGuesses; i++) */
/* 		{ */
/* 			printf("%d: %f\n", multistart_guesses[i], */
/* 			       fabs(dot(voxel, plane_eqs[multistart_guesses[i]]))); */
/* 		} */
/* 	} */
/* #endif */

	int2 close_planes_ret;
	// Iterate over the axes such that the the
	// next voxel is always a neighbour of the previous voxel
	for(int xoffset = 0; xoffset < CUBE_SIZE; xoffset++)
	{
		int x = x_origin + xoffset;
		if(x >= volume_xsize) break;
		BOUNDS_CHECK(x, 0, volume_xsize);

		int ystart, yend, ydir;
		if(xoffset % 2)
		{
			ystart = CUBE_SIZE-1;
			yend = -1;
			ydir = -1;
		}
		else {
			ystart = 0;
			yend = CUBE_SIZE;
			ydir = 1;
		}

		for(int yoffset = ystart; yoffset != yend ; yoffset+=ydir)
		{
			int y = y_origin + yoffset;
			if(y >= volume_ysize) continue;
			BOUNDS_CHECK(y, 0, volume_ysize);

			int zstart, zend, zdir;
			if(yoffset % 2 && xoffset % 2)
			{
				zstart = CUBE_SIZE-1;
				zend = -1;
				zdir = -1;
			}
			else {
				zstart = 0;
				zend = CUBE_SIZE;
				zdir = 1;
			}
			for(int zoffset = zstart; zoffset != zend ; zoffset+=zdir)
			{
				int z = z_origin + zoffset;
				if(z >= volume_zsize) continue;
				BOUNDS_CHECK(z, 0, volume_zsize);
				BOUNDS_CHECK(x, 0, volume_xsize);
				BOUNDS_CHECK(y, 0, volume_ysize);
				voxel.x = x * volume_xspacing;
				voxel.y = y * volume_yspacing;
				voxel.z = z * volume_zspacing;

				// Find all planes closer than radius

				close_planes_ret = FIND_CLOSE_PLANES(close_planes,
				                                     plane_eqs,
				                                     plane_matrices,
				                                     voxel,
				                                     radius,
				                                     multistart_guesses,
				                                     nGuesses,
				                                     mask,
				                                     in_xsize,
				                                     in_ysize,
				                                     in_xspacing,
				                                     in_yspacing);
				n_close_planes = close_planes_ret.x;



					// Call appropriate method to determine pixel value
					VOXEL(out_volume,x,y,z) = PERFORM_INTERPOLATION(close_planes,
					                                                n_close_planes,
					                                                plane_matrices,
					                                                plane_eqs,
					                                                bscans_blocks,
					                                                in_xsize,
					                                                in_ysize,
					                                                in_xspacing,
					                                                in_yspacing,
					                                                mask,
					                                                voxel);


				}
		}
	}
}
