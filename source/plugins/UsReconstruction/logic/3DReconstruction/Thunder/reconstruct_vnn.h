// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "cxForwardDeclarations.h"

/**
 * \addtogroup sscThunder
 * \{
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
	ssc::ProcessedUSInputDataPtr frameData;

	double* input_pos_matrices;
	unsigned char* input_mask;

	unsigned char* output;
	int output_dim[3];
	double output_spacing[3];
};

bool reconstruct_vnn(reconstruct_data* data, const char* kernel_path, QString processor, float distance);

/**
 * \}
 */

