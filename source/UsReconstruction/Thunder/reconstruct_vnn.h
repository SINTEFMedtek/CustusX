/*
 *  reconstruct_vnn.h
 *
 *  Created by Ole Vegard Solberg on 5/6/10.
 *
 */

/**
 * Container struct for input and output data for the reconstruction algorithm
 * input US frames merged in a volume. The 2 first dimesions (in input_dim) 
 * are x and y in the ultrasound plane, and the 3rd dimension is the number 
 * of US frames. input_spacing is similar to input_dim
 * input_pos_matrices The position matrices for each US frame. 
 * input_mask The clipping mask for the input data. The dimensions are input_dim[0], input_dim[1] and 1
 * Size = input_spacing[2]
 * 
 * output A preallocated volume where the algorithm should put the finished volume
 */
struct reconstruct_data
{
  unsigned char* input;
  int input_dim[3];
  double input_spacing[3];
  
  double* input_pos_matrices;
  unsigned char* input_mask;
  
  unsigned char* output;
  int output_dim[3];
  double output_spacing[3];
};

void reconstruct_vnn(reconstruct_data* data, const char* kernel_path);