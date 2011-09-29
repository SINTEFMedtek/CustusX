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

#include <vtkImageData.h>

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
                     plane_eq* bscan_plane_equations,
                     float distance)
{
	//TODO: Use distance (from GUI) = kernel_radius in .ocl code
  int volume_w = data->output_dim[0];
  int volume_h = data->output_dim[1];
  int volume_n = data->output_dim[2];
//  int* outputDim;
//  data->outputData->getBaseVtkImageData()->GetDimensions(outputDim);
//  int volume_w = outputDim[0];
//  int volume_h = outputDim[1];
//  int volume_n = outputDim[2];

  float volume_spacing = data->output_spacing[0];
//  float volume_spacing = data->outputData->getBaseVtkImageData()->GetSpacing()[0];

  int* inputDim = data->frameData->getDimensions();
//  int bscan_w = data->input_dim[0];
//  int bscan_h = data->input_dim[1];
//  int bscan_n = data->input_dim[2];
  int bscan_w = inputDim[0];
  int bscan_h = inputDim[1];
  int bscan_n = inputDim[2];

  ssc::Vector3D inputSpacing = data->frameData->getSpacing();
//  float bscan_spacing_x = data->input_spacing[0];
//  float bscan_spacing_y = data->input_spacing[1];
  float bscan_spacing_x = inputSpacing[0];
  float bscan_spacing_y = inputSpacing[1];
  
//  unsigned char* bscans = data->input;
  unsigned char* volume = data->output;
  unsigned char* mask = data->input_mask;
  
  int volume_size = volume_n * volume_h * volume_w * sizeof(cl_uchar);
  //int bscans_size = bscan_n * bscan_h * bscan_w * sizeof(cl_uchar);
  int mask_byte_size = bscan_h * bscan_w * sizeof(cl_uchar);
  int plane_eq_size = sizeof(float) * 4 * bscan_n;
  int plane_points_size = sizeof(float) * 4 * bscan_n * 3;
  int printings_size = sizeof(float)*volume_h*10;//??
  
  float * h_dev_plane_eq = (float *) malloc(plane_eq_size);
  float * h_dev_plane_points = (float *) malloc(plane_points_size);
  memcpy(h_dev_plane_eq, bscan_plane_equations, plane_eq_size);
  //memset(h_dev_plane_points, 0, plane_points_size);//test
  for (int n = 0; n < bscan_n; n++)
  {
    for (int m = 0; m < 3; m++)
    {
      //Might be slower, but easier to read 
      //memcpy(&h_dev_plane_points[n*3*4+m*4], &plane_points[n*3+m], sizeof(float)*3);//Also remember to set mem of last float to 0.0
      h_dev_plane_points[n*3*4+m*4+0] = plane_points[n*3+m].x;
      h_dev_plane_points[n*3*4+m*4+1] = plane_points[n*3+m].y;
      h_dev_plane_points[n*3*4+m*4+2] = plane_points[n*3+m].z;
      h_dev_plane_points[n*3*4+m*4+3] = 0;
    }
  }
  
  float * printings = (float *) malloc(printings_size);
  memset(printings, 0, printings_size);
  
  // The GPU (openCL) can't handle very large memory blocks, and the only pointers that can be used
  // are the input parameters. Our solution is therefore to split the input data into a set of
  // data blocks (10)

  int numBlocks = 10;
  unsigned char** framePointers = new unsigned char*[numBlocks];

//  unsigned char* frames0, frames1, frames2, frames3, frames4, frames5, frames6, frames7, frames8, frames9;

  int frameSize = bscan_h * bscan_w;//assuming 8 bit data

  int blocksize = bscan_n/numBlocks * frameSize;
  int divRemainder = bscan_n%numBlocks;// The reminder after the integer division

  //Allocate memory for the data blocks
  for (int i = 0; i < numBlocks; i++)
  {
    if (divRemainder != 0)
    {
      divRemainder--;
      framePointers[i] = new unsigned char[blocksize + frameSize];// The first blocks gets an extra image each if the integer division got a reminder
    } else
    {
      framePointers[i] = new unsigned char[blocksize];
    }

  }
  //Copy the frames into the data blocks
  for (int i = 0; i < bscan_n; i++)
  {
//    std::cout << i << " ";
//    void* framePtr = (unsigned char*)(framePointers[i%numBlocks][i/numBlocks*frameSize]);
    unsigned char* framePtr = (framePointers[i%numBlocks]);
    framePtr += (i/numBlocks*frameSize);
    memcpy(framePtr, data->frameData->getFrame(i), frameSize);
  }

  //Allocate GPU (cl) memory for each frame block
  divRemainder = bscan_n%numBlocks;
  cl_mem* clFramePointers = new cl_mem[numBlocks];
  for (int i = 0; i < numBlocks; i++)
  {
    int allocSize = blocksize;
    if (divRemainder != 0)
    {
      divRemainder--;
      allocSize = blocksize + frameSize;
    }
    cl_mem clFrameBlock = ocl_create_buffer(context->context, CL_MEM_READ_ONLY, allocSize, framePointers[i]);
    clFramePointers[i] = clFrameBlock;
  }
  //free the data blocks memory
  for (int i = 0; i < numBlocks; i++)
  {
  	delete framePointers[i];
  }

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
    int i = 0;
    clSetKernelArg(vnn, i++, sizeof(cl_mem), &dev_mask);
    clSetKernelArg(vnn, i++, sizeof(cl_int), &bscan_w);
    clSetKernelArg(vnn, i++, sizeof(cl_int), &bscan_h);
    clSetKernelArg(vnn, i++, sizeof(cl_int), &bscan_n);
    clSetKernelArg(vnn, i++, sizeof(cl_float), &bscan_spacing_x);
    clSetKernelArg(vnn, i++, sizeof(cl_float), &bscan_spacing_y);
    clSetKernelArg(vnn, i++, sizeof(cl_mem), &dev_volume);
    clSetKernelArg(vnn, i++, sizeof(cl_int), &volume_n);
    clSetKernelArg(vnn, i++, sizeof(cl_int), &volume_h);
    clSetKernelArg(vnn, i++, sizeof(cl_int), &volume_w);
    clSetKernelArg(vnn, i++, sizeof(cl_float), &volume_spacing);
    clSetKernelArg(vnn, i++, sizeof(cl_mem), &dev_plane_eq);
    clSetKernelArg(vnn, i++, sizeof(cl_mem), &dev_plane_points);
    clSetKernelArg(vnn, i++, sizeof(cl_mem), &dev_printings);
    clSetKernelArg(vnn, i++, sizeof(cl_int), &section);
    clSetKernelArg(vnn, i++, sizeof(cl_float), &distance);

    // Add the frame memory blocks (10 for now)
    for (int bn = 0; bn < numBlocks; bn++)
      clSetKernelArg(vnn, i++, sizeof(cl_mem), &clFramePointers[bn]);
    
    size_t * global_work_size = (size_t *) malloc(sizeof(size_t)*1);
    global_work_size[0] = (volume_w*volume_n/1/256+1)*256;//TODO: Find better number? 256?
    size_t * local_work_size = (size_t *) malloc(sizeof(size_t)*1);//TODO: Not in use?
    local_work_size[0] = 256;
    std::cout << "Start openCL code. global_work_size[0]: " << global_work_size[0] << std::endl;
    ocl_check_error(clEnqueueNDRangeKernel(context->cmd_queue, vnn, 1, NULL, global_work_size, NULL, 0, NULL, NULL));
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

  //Release GPU memory for all frames
  for (int i = 0; i < numBlocks; i++)
  {
    clReleaseMemObject(clFramePointers[i]);
  }
  clReleaseMemObject(dev_printings);
  clReleaseMemObject(dev_mask);
  clReleaseMemObject(dev_plane_eq);
  clReleaseMemObject(dev_plane_points);
  clReleaseMemObject(dev_volume);

  ocl_check_error(clFinish(context->cmd_queue));

  //holger_time(1, "Fill volume");
  //holger_time_print(1);
}

//void reconstruct_vnn(reconstruct_data* data, const char* kernel_path)
void reconstruct_vnn(reconstruct_data* data, const char* kernel_path, QString processor, float distance)
{
  const char* program_src = file2string(kernel_path);
  if (program_src == NULL) {
	  std::cout << "OpenCL Error: Cannot find file " << kernel_path << std::endl;
	  return;
//		printf("ERROR: did not find kernels.ocl\n");
//		exit(-1);
	}
  
  if (!ocl_has_device_type(processor))
  {
	  std::cout << "OpenCL Error: Device type " << processor.toStdString() << " not available" <<  std::endl;
	  return;
  }

  ocl_context* context = ocl_init(processor);

	cl_program program = ocl_create_program(context->context, context->device, program_src, kernel_path);
  //TODO: free program_src
	
	cl_kernel vnn	= ocl_kernel_build(program, context->device, "vnn");

//  float3* plane_points = generate_plane_points(data->input_pos_matrices,
//                                               data->input_dim[0],
//                                               data->input_dim[1],
//                                               data->input_dim[2],
//                                               data->input_spacing[0],
//                                               data->input_spacing[1]);
  int* inputDims = data->frameData->getDimensions();
  ssc::Vector3D inputSpacing = data->frameData->getSpacing();
  float3* plane_points = generate_plane_points(data->input_pos_matrices,
      inputDims[0],
      inputDims[1],
      inputDims[2],
      inputSpacing[0],
      inputSpacing[1]);
  
  //plane_eq* bscan_plane_equations = generate_plane_equations(plane_points, data->input_dim[2]);
  plane_eq* bscan_plane_equations = generate_plane_equations(plane_points, inputDims[2]);

  call_vnn_kernel(vnn, context, data, plane_points, bscan_plane_equations, distance);
  
  
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
