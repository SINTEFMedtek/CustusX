/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVNNCLALGORITHM_H_
#define CXVNNCLALGORITHM_H_

//TODO remove, should be defined in oul instead
#define __CL_ENABLE_EXCEPTIONS //telling the opencl c++ wrapper to throw exceptions

#include "cxUSFrameData.h"
#include "OpenCLManager.hpp"
#include "RuntimeMeasurementManager.hpp"

namespace cx
{
/**
 *  Original author Tord Øygard
 */
class VNNclAlgorithm
{
public:
	VNNclAlgorithm();
	~VNNclAlgorithm();

	/**
	 * A representation of a Frame block in CPU memory.
	 * Since OpenCL has an upper limit on how big memory objects can be, we need to
	 * partition the input US images into several blocks. This struct represents
	 * one such block
	 */
	typedef struct __frameBlock_t
	{
		unsigned char* data;
		size_t length;
	} frameBlock_t;

	/**
	 * Initialize OpenCL.
	 * This function initializes OpenCL and builds the kernel with the given parameters.
	 * Note that these parameters are set at compile time and not kernel run time.
	 * Uses buildCLProgram() to build the kernel.
	 * @param kernelFile Path to the OpenCL kernel source
	 * @param nMaxPlanes The MAX_PLANES parameter of the kernel, i.e. max value of how many planes to include in the reconstruction of one voxel
	 * @param nPlanes Number of image planes in the input data set
	 * @param method The method ID. See kernels.cl for more information
	 * @param planeMethod the plane method ID. See kernels.cl for more information
	 * @param nStarts number of starts for multistart search for close planes
	 * @param brightnessWeight The extra weight to give pixels brighter than mean
	 * @param newnessWeight The extra weight to give pixels newer than mean
	 * @return True on success
	 * @sa buildCLProgram
	 */
	virtual bool initCL(QString kernelFile,
	                    int nMaxPlanes,
	                    int nPlanes,
	                    int method,
	                    int planeMethod,
	                    int nStarts,
	                    float brightnessWeight,
	                    float newnessWeight);
	/**
	 * Build the OpenCL kernel
	 * @param program_src The kernel source code
	 * @param kernelPath path to kernel source
	 * @param nMaxPlanes The MAX_PLANES parameter of the kernel, i.e. max value of how many planes to include in the reconstruction of one voxel
	 * @param nPlanes Number of image planes in the input data set
	 * @param method The method ID. See kernels.cl for more information
	 * @param planeMethod the plane method ID. See kernels.cl for more information
	 * @param nStarts number of starts for multistart search for close planes
	 * @param brightnessWeight The extra weight to give pixels brighter than mean
	 * @param newnessWeight The extra weight to give pixels newer than mean
	 * @return True on suc
	 */
	virtual cl::Program buildCLProgram(std::string program_src,
									   std::string kernelPath,
	                                  int nMaxPlanes,
	                                  int nPlanes,
	                                  int method,
	                                  int planeMethod,
	                                  int nStarts,
	                                  float brightnessWeight,
	                                  float newnessWeight);
	/**
	 * Perform GPU Reconstruction.
	 * This function initializes the CL memory objects, calls the kernel and reads back the result,
	 * which is stored in outputData.
	 * @param input The input US data
	 * @param outputData The output volume is stored here
	 * @param radius The radius of the kernel - i.e. how far away to look for image planes to use
	 * @param nClosePlanes The number of close planes to search for
	 * @return true on success
	 */
	virtual bool reconstruct(ProcessedUSInputDataPtr input,
	                              vtkImageDataPtr outputData,
	                              float radius,
	                              int nClosePlanes);

	/**
	 * Split the US input into numBlock blocks of whole frames
	 * and store them in framePointers.
	 * \param[out] framePointers Pre-allocated array of numBlocks unsigned char* pointers
	 *             Data will be returned here
	 * \param[in] numBlocks Number of blocks to split US input into
	 * \param[in] inputFrames The input US B-scans
	 * \return true on success, false otherwise
	 */
	virtual bool initializeFrameBlocks(frameBlock_t* framePointers,
	                                   int numBlocks,
	                                   ProcessedUSInputDataPtr inputFrames);
	/**
	 * Free a set of frame blocks allocated by initializeFrameBlocks
	 * Does not free the actual frameBlock_t structure, but the data pointer is free'd.
	 * You still have to free the frameBlocks!
	 * @param framePointers Pointer to first element in array of frame blocks
	 * @param numBlocks number of blocks in the array
	 */
	virtual void freeFrameBlocks(frameBlock_t* framePointers,
								 int numBlocks);


	/**
	 * Fill the supplied array of floats with the values from the transformation matrices
	 * in input.
	 * The data is stored like this:
	 *  [ a b c d
	 *    e f g h    -> [ a b c d e f g h i j k l m n o p ]
	 *    i j k l
	 *    m n o p ]
	 * I.e: 16 floats per image plane, row major.
	 * @param planeMatrices Pointer to array of 16*number of image planes
	 *                       floats where the matrices will be stored
	 * @param input The US data - which also has position data
	 */
	virtual void fillPlaneMatrices(float *planeMatrices,
	                               ProcessedUSInputDataPtr input);

	/**
	 * Enable/diable OpenCL profiling for this algorithm
	 */
	void setProfiling(bool on);
	double getTotalExecutionTime();
	double getKernelExecutionTime();

private:
	void setKernelArguments(
			cl::Kernel kernel,
			int volume_xsize,
	        int volume_ysize,
	        int volume_zsize,
	        float volume_xspacing,
	        float volume_yspacing,
	        float volume_zspacing,
	        int in_xsize,
	        int in_ysize,
	        float in_xspacing,
	        float in_yspacing,
	        std::vector<cl::Buffer>& blocks,
	        cl::Buffer out_volume,
	        cl::Buffer plane_matrices,
	        cl::Buffer mask,
	        size_t plane_eqs_size,
	        size_t close_planes_size,
	        float radius);
	size_t calculateSpaceNeededForClosePlanes(cl::Kernel kernel, cl::Device device, size_t local_work_size, size_t nPlanes_numberOfInputImages, int nClosePlanes);
	bool isUsingTooMuchMemory(size_t outputVolumeSize, size_t inputBlocksLength, cl_ulong globalMemUse);
	void measureAndExecuteKernel(cl::CommandQueue queue, cl::Kernel kernel, size_t global_work_size, size_t local_work_size, std::string measurementName);
	void measureAndReadBuffer(cl::CommandQueue queue, cl::Buffer outputBuffer, size_t outputVolumeSize, void *outputData, std::string measurementName);

	void startProfiling(std::string name, cl::CommandQueue queue);
	void stopProfiling(std::string name, cl::CommandQueue queue);

	cl::Kernel mKernel;
	oul::ContextPtr mOulContex;
	oul::RuntimeMeasurementsManagerPtr mRuntime;
	std::set<std::string> mMeasurementNames;
	std::string mKernelMeasurementName;

};

typedef boost::shared_ptr<VNNclAlgorithm> VNNclAlgorithmPtr;

} /* namespace cx */
#endif /* CXVNNCLALGORITHM_H_ */
