/*
 *  reconstruct_vnn.c
 *
 *  Created by Ole Vegard Solberg on 5/6/10.
 *
 */

#include "reconstruct_vnn.h"

#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#else
#include <CL/cl.h>
#endif //__APPLE__

#include <string.h>
#include "setup.h"
#include "holger_time.h"
#include "utils.h"


float3* generate_plane_points(double* pos_matrices, 
                              int bscan_w,
                              int bscan_h,
                              int bscan_n,
                              double bscan_spacing_x,
                              double bscan_spacing_y)
{
  
	float3 * plane_points = (float3 *) malloc(sizeof(float3)*bscan_n*3);
  
#define plane_points_c(n,i) (plane_points[(n)*3 + (i)])
#define pos_matrices_a(n,x,y) (pos_matrices[n*12 + y*4 + x])
  
  // Fill plane_points
  for (int n = 0; n < bscan_n; n++)
  {
    float3 corner0 = {0.0f,0.0f,0.0f};
    plane_points_c(n, 0) = corner0;
    
    //float3 cornerx = {0.0f, bscan_w*bscan_spacing_x, 0.0f};
    float3 cornerx = {bscan_w*bscan_spacing_x, 0.0f, 0.0f};
    plane_points_c(n, 1) = cornerx;
    
    //float3 cornery = {0.0f, 0.0f, bscan_h*bscan_spacing_y};
    float3 cornery = {0.0f, bscan_h*bscan_spacing_y, 0.0f};
    plane_points_c(n, 2) = cornery;
  }
  
#ifdef PRINT
  printf("plane_points:\n");
  for (int n = 0; n < bscan_n; n+=bscan_n)
  {	
    for (int i = 0; i < 3; i++)
    {	
      printf("%5.2f ", plane_points_c(n, i).x);
      printf("%5.2f ", plane_points_c(n, i).y);
      printf("%5.2f ", plane_points_c(n, i).z);
      printf(" \t ");
    }
    printf("\n");
  }
#endif
  
  // Transform plane_points
  float * sums = (float *) malloc(sizeof(float)*3);
  for (int n = 0; n < bscan_n; n++)
  {	
    for (int i = 0; i < 3; i++)
    {	
      for (int y = 0; y < 3; y++)
      {
        float sum = 0;
        sum += pos_matrices_a(n,0,y)*plane_points_c(n,i).x;
        sum += pos_matrices_a(n,1,y)*plane_points_c(n,i).y;
        sum += pos_matrices_a(n,2,y)*plane_points_c(n,i).z;
        sum += pos_matrices_a(n,3,y);
        sums[y] = sum;
      }
      memcpy(&plane_points_c(n,i), sums, 3*sizeof(float));
    }
  }
  
/*  printf("plane_points:\n");
  for (int n = 0; n < bscan_n; n++)
  {	
    for (int i = 0; i < 3; i++)
    {	
      printf("%5.2f ", plane_points_c(n, i).x);
      printf("%5.2f ", plane_points_c(n, i).y);
      printf("%5.2f ", plane_points_c(n, i).z);
      printf(" \t ");
    }
    printf("\n");
  }*/
  
  free(sums);
#undef plane_points_c
#undef pos_matrices_a
  return plane_points;
}

plane_eq* generate_plane_equations(float3* plane_points, int bscan_n)
{
	plane_eq * bscan_plane_equations = (plane_eq *) malloc(sizeof(plane_eq)*bscan_n);
  
#define plane_points_c(n,i) (plane_points[(n)*3 + (i)])
  
  // Fill bscan_plane_equations
	for (int n = 0; n < bscan_n; n++) {
		float3 a = plane_points_c(n,0);
		float3 b = plane_points_c(n,1);
		float3 c = plane_points_c(n,2);
		float3 normal = normalize(cross(sub(a,b), sub(c,a)));
    
    //printf("ax: %f, ay: %f, az: %f\n", a.x, a.y, a.z);
    
		bscan_plane_equations[n].a = normal.x;
		bscan_plane_equations[n].b = normal.y;
		bscan_plane_equations[n].c = normal.z;
		bscan_plane_equations[n].d = -normal.x*a.x - normal.y*a.y - normal.z*a.z;
	}
  
#ifdef PRINT
	printf("bscan_plane_equations:\n");
	for (int n = 0; n < bscan_n; n++)
		//printf("bscan_plane_equations[%d]: %f %f %f %f\n",n, bscan_plane_equations[n*4+0], bscan_plane_equations[n*4+1], bscan_plane_equations[n*4+2], bscan_plane_equations[n*4+3]);
    printf("bscan_plane_equations[%d]: %f %f %f %f\n",n, bscan_plane_equations[n].a, bscan_plane_equations[n].b, bscan_plane_equations[n].c, bscan_plane_equations[n].d);
#endif
  
#undef plane_points_c
  return bscan_plane_equations;
}



void call_vnn_kernel(cl_kernel vnn, 
                     ocl_context* context,
                     reconstruct_data* data, 
                     float3* plane_points, 
                     plane_eq* bscan_plane_equations)
{
  int volume_w = data->output_dim[0];
  int volume_h = data->output_dim[1];
  int volume_n = data->output_dim[2];
  
  float volume_spacing = data->output_spacing[0];
  
  int bscan_w = data->input_dim[0];
  int bscan_h = data->input_dim[1];
  int bscan_n = data->input_dim[2];
  
  float bscan_spacing_x = data->input_spacing[0];
  float bscan_spacing_y = data->input_spacing[1];
  
  unsigned char* bscans = data->input;
  unsigned char* volume = data->output;
  unsigned char* mask = data->input_mask;
  
  int volume_size = volume_n * volume_h * volume_w * sizeof(cl_uchar);
  int bscans_size = bscan_n * bscan_h * bscan_w * sizeof(cl_uchar);
  int mask_byte_size = bscan_h * bscan_w * sizeof(cl_uchar);
  int plane_eq_size = sizeof(float) * 4 * bscan_n;
  int plane_points_size = sizeof(float) * 4 * bscan_n * 3;
  int printings_size = sizeof(float)*volume_h*10;//??
  
  float * h_dev_plane_eq = (float *) malloc(plane_eq_size);
  float * h_dev_plane_points = (float *) malloc(plane_points_size);
  memcpy(h_dev_plane_eq, bscan_plane_equations, plane_eq_size);
  for (int n = 0; n < bscan_n; n++)
    for (int m = 0; m < 3; m++)
      memcpy(&h_dev_plane_points[n*3*4+m*4], &plane_points[n*3+m], sizeof(float)*3);
  float * printings = (float *) malloc(printings_size);
  memset(printings, 0, printings_size);
  
  cl_mem dev_bscans0 = ocl_create_buffer(context->context, CL_MEM_READ_ONLY, bscans_size/2, bscans);
  cl_mem dev_bscans1 = ocl_create_buffer(context->context, CL_MEM_READ_ONLY, bscans_size/2+bscans_size%2, bscans + bscans_size/2);//Make sure we allocate the last byte
  
  ///* // with byte adressable memory:
  cl_mem dev_volume = ocl_create_buffer(context->context, CL_MEM_WRITE_ONLY, volume_size, volume);
  cl_mem dev_mask = ocl_create_buffer(context->context, CL_MEM_READ_ONLY, mask_byte_size, mask);
  //*/
  
  
  /* // AMD does not have byte adressable memory:
   unsigned int * int_volume = (unsigned int *) malloc(volume_size*4);
   memset(int_volume, 0, volume_size*4);
   unsigned int * int_mask = (unsigned int *) malloc(bscan_w*bscan_h*4);
   for (int i = 0; i < bscan_w*bscan_h; i++)
   int_mask[i] = mask[i];
   cl_mem dev_volume = ocl_create_buffer(context, CL_MEM_WRITE_ONLY, volume_size*4, int_volume);
   cl_mem dev_mask = ocl_create_buffer(context, CL_MEM_READ_ONLY, mask_byte_size*4, int_mask);
   */
  
  cl_mem dev_plane_eq = ocl_create_buffer(context->context, CL_MEM_READ_ONLY, plane_eq_size, h_dev_plane_eq);
  cl_mem dev_plane_points = ocl_create_buffer(context->context, CL_MEM_READ_ONLY, plane_points_size, h_dev_plane_points);
  cl_mem dev_printings = ocl_create_buffer(context->context, CL_MEM_WRITE_ONLY, printings_size, printings);
  ocl_check_error(clFinish(context->cmd_queue));



  for (int section = 0; section < 1; section++) {
    clSetKernelArg(vnn, 0, sizeof(cl_mem), &dev_bscans0);
    clSetKernelArg(vnn, 1, sizeof(cl_mem), &dev_bscans1);
    clSetKernelArg(vnn, 2, sizeof(cl_mem), &dev_mask);
    clSetKernelArg(vnn, 3, sizeof(cl_int), &bscan_w);
    clSetKernelArg(vnn, 4, sizeof(cl_int), &bscan_h);
    clSetKernelArg(vnn, 5, sizeof(cl_int), &bscan_n);
    clSetKernelArg(vnn, 6, sizeof(cl_float), &bscan_spacing_x);
    clSetKernelArg(vnn, 7, sizeof(cl_float), &bscan_spacing_y);
    clSetKernelArg(vnn, 8, sizeof(cl_mem), &dev_volume);
    clSetKernelArg(vnn, 9, sizeof(cl_int), &volume_n);
    clSetKernelArg(vnn, 10, sizeof(cl_int), &volume_h);
    clSetKernelArg(vnn, 11, sizeof(cl_int), &volume_w);
    clSetKernelArg(vnn, 12, sizeof(cl_float), &volume_spacing);
    clSetKernelArg(vnn, 13, sizeof(cl_mem), &dev_plane_eq);
    clSetKernelArg(vnn, 14, sizeof(cl_mem), &dev_plane_points);
    clSetKernelArg(vnn, 15, sizeof(cl_mem), &dev_printings);
    clSetKernelArg(vnn, 16, sizeof(cl_int), &section);
    
    size_t * global_work_size = (size_t *) malloc(sizeof(size_t)*1);
    global_work_size[0] = (volume_w*volume_n/1/256+1)*256;//TODO: Find better number? 256?
    size_t * local_work_size = (size_t *) malloc(sizeof(size_t)*1);//TODO: Not in use?
    local_work_size[0] = 256;
    ocl_check_error(clEnqueueNDRangeKernel(context->cmd_queue, vnn, 1, NULL, global_work_size, NULL, NULL, NULL, NULL));
    //ocl_check_error(clFinish(context->cmd_queue));
  }

  // with byte adressable memory:
  ocl_check_error(clEnqueueReadBuffer(context->cmd_queue, dev_volume, CL_TRUE, 0, volume_size, volume, 0, 0, 0));

  // AMD does not have byte adressable memory:
  /*ocl_check_error(clEnqueueReadBuffer(cmd_queue, dev_volume, CL_TRUE, 0, volume_size*4, int_volume, 0, 0, 0));
   for (int i = 0; i < volume_h*volume_w*volume_n; i++)
   volume[i] = int_volume[i];
   free(int_volume);
   free(int_mask);
   */

  /*ocl_check_error(clEnqueueReadBuffer(cmd_queue, dev_printings, CL_TRUE, 0, printings_size, printings, 0, 0, 0));
   for (int i = 0; i < printings_size/sizeof(float); i++) {
   printf("%f \n", printings[i]);
   }*/

  clReleaseMemObject(dev_bscans0);
  clReleaseMemObject(dev_bscans1);
  clReleaseMemObject(dev_printings);
  clReleaseMemObject(dev_mask);
  clReleaseMemObject(dev_plane_eq);
  clReleaseMemObject(dev_plane_points);
  clReleaseMemObject(dev_volume);

  ocl_check_error(clFinish(context->cmd_queue));

  holger_time(1, "Fill volume");
  holger_time_print(1);
}

void reconstruct_vnn(reconstruct_data* data, const char* kernel_path)
{
  const char* program_src = file2string(kernel_path);
  if (program_src == NULL) {
		printf("ERROR: did not find kernels.ocl\n");
		exit(-1);
	}
  
  ocl_context* context = ocl_init();
  
	cl_program program = ocl_create_program(context->context, context->device, program_src);
  //TODO: free program_src
	
	cl_kernel vnn	= ocl_kernel_build(program, context->device, "vnn");
  
  float3* plane_points = generate_plane_points(data->input_pos_matrices, 
                                               data->input_dim[0],
                                               data->input_dim[1],
                                               data->input_dim[2],
                                               data->input_spacing[0],
                                               data->input_spacing[1]);
  
  plane_eq* bscan_plane_equations = generate_plane_equations(plane_points,
                                                             data->input_dim[2]);

  call_vnn_kernel(vnn, context, data, plane_points, bscan_plane_equations);
  
  
  clReleaseKernel(vnn);
  
  clReleaseProgram(program);
  ocl_release(context);
  
  free(plane_points);
  free(bscan_plane_equations);
}
  
/*void ocl_release(ocl_context* context) {
  printf("ocl_release\n");
  //clReleaseMemObject(dev_volume);
 
  //clReleaseKernel(vnn);
  
  //clReleaseProgram(program);
}  */
