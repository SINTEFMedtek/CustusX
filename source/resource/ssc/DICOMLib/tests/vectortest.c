#include <assert.h>

#include "vector3d.h"

/*** Comparison functions ***/

static void my_v3_normalize(double *v)
{
  double ir = 1.0 / sqrt (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  v[0] *= ir;
  v[1] *= ir;
  v[2] *= ir;
}
#define CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

/*** Main ***/

int main(void)
{
  double length;
  const double v3[3] = { 1.0f, 1.0f, 1.0f };
  double dst[3] = { 0.0f, 0.0f, 0.0f };
  double v9a[9];
  const double key1[3] = { 2.0f, 2.0f, 2.0f };
  const double v3_4[3] = { 4.0f, 4.0f, 4.0f };
  const double nkey[3] = { 0.5773f, 0.5773f, 0.5773f };

  assert(!vector3d_compare(v3, dst, 0.0));
  vector3d_copy(dst, v3);
  assert(vector3d_compare(dst, v3, 0.0));
  vector3d_add(dst, v3, v3);
  assert(vector3d_compare(dst, key1, 0.0));
  vector3d_subtract(dst, dst, v3);
  assert(vector3d_compare(dst, v3, 0.0));
  assert(fabsf(DOT(key1, nkey) - vector3d_inner_product(key1, nkey)) < 0.0001);
  assert(fabsf(DOT(v3_4, nkey) - vector3d_inner_product(v3_4, nkey)) < 0.0001);
  assert(fabsf(DOT(v3_4, v3) - vector3d_inner_product(v3_4, v3)) < 0.0001);
  vector3d_cross_product(dst, key1, nkey);
  CROSS(v9a, key1, nkey);
  assert(vector3d_compare(dst, v9a, 0.00001));

  vector3d_copy(dst, v3_4);
  vector3d_scalar_divide(dst, key1[0]);
  assert(vector3d_compare(dst, key1, 0.0));

  vector3d_add(dst, v3_4, v3);
  assert(dst[0] == 5.0f && dst[1] == 5.0f && dst[2] == 5.0f);

  length = vector3d_length(dst);
  assert(fabsf(length - 8.66025f) < 0.001f);
  vector3d_add(dst, v3_4, v3);

  vector3d_copy(dst, v3);
  vector3d_normalize(dst);
  assert(vector3d_compare(dst, nkey, 0.0001));
  vector3d_copy(dst, v3);
  my_v3_normalize(dst);
  assert(vector3d_compare(dst, nkey, 0.0001));

  return 0;
}
