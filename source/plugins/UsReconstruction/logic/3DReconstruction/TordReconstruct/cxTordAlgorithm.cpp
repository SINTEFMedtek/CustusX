#include "cxTordAlgorithm.h"

#include "sscMessageManager.h"
#include <vtkImageData.h>

namespace cx
{
TordAlgorithm::TordAlgorithm()
{}

TordAlgorithm::~TordAlgorithm()
{}

bool TordAlgorithm::initCL(QString kernelPath, int nMaxPlanes, int nPlanes, int method, int planeMethod, int nStarts, float brightnessWeight, float newnessWeight)
{
	// INIT
	mOpenCL = OpenCL::init(CL_DEVICE_TYPE_GPU);

	// READ KERNEL FILE
	messageManager()->sendInfo(QString("Kernel path: %1").arg(kernelPath));
	size_t sourceLen;
	char* sSource = OpenCLUtilities::file2string(kernelPath.toLocal8Bit().data(), &sourceLen);

	// BUILD PROGRAM
	cl::Program clprogram = this->buildCLProgram(sSource, nMaxPlanes, nPlanes, method, planeMethod, nStarts,brightnessWeight, newnessWeight, kernelPath, sourceLen);

	// CREATE KERNEL
	mKernel = OpenCL::createKernel(clprogram, "voxel_methods");

	return true;
}

cl::Program TordAlgorithm::buildCLProgram(const char* program_src, int nMaxPlanes, int nPlanes, int method, int planeMethod, int nStarts, float newnessWeight, float brightnessWeight, QString kernelPath, size_t sourceLen)
{
	cl::Program retval;
	cl_int err = 0;
	VECTOR_CLASS<cl::Device> devices;
	try
	{
		cl::Program::Sources sources;
		sources.push_back(std::pair<const char*, ::size_t>(program_src, sourceLen));
		retval = cl::Program(mOpenCL->context, sources, &err);
		check_error(err);

		QString define = "-D MAX_PLANES=%1 -D N_PLANES=%2 -D METHOD=%3 -D PLANE_METHOD=%4 -D MAX_MULTISTART_STARTS=%5 -D NEWNESS_FACTOR=%6 -D BRIGHTNESS_FACTOR=%7";
		define = define.arg(nMaxPlanes).arg(nPlanes).arg(method).arg(planeMethod).arg(nStarts).arg(newnessWeight).arg(brightnessWeight);

		devices = mOpenCL->context.getInfo<CL_CONTEXT_DEVICES>();
		err = retval.build(devices, define.toStdString().c_str(), NULL, NULL);
	} catch (cl::Error &error)
	{
		messageManager()->sendError("Could not build a OpenCL program. Reason: "+QString(error.what()));
		for(int i=0; i<devices.size(); i++)
		{
	    cl::STRING_CLASS log;
	    retval.getBuildInfo(devices[i], CL_PROGRAM_BUILD_LOG, &log);
	    messageManager()->sendInfo("Build log: \n"+QString(log.c_str()));

		}

		check_error(error.err());
	}
	return retval;
}

bool TordAlgorithm::initializeFrameBlocks(frameBlock_t* framePointers, int numBlocks, ProcessedUSInputDataPtr inputFrames)
{
	// Compute the size of each frame in bytes
	Eigen::Array3i dims = inputFrames->getDimensions();
	size_t frameSize = dims[0] * dims[1];
	size_t numFrames = dims[2];
	messageManager()->sendInfo(QString("Input dims: (%1, %2, %3)").arg(dims[0]).arg(dims[1]).arg(dims[2]));

	// Find out how many frames needs to be in each block
	size_t framesPerBlock = numFrames / numBlocks;
	messageManager()->sendInfo(QString("Frames: %1, Blocks: %2, Frames per block: %3").arg(numFrames).arg(numBlocks).arg(framesPerBlock));

	// Some blocks will need to contain one extra frame
	// (numFrames and numBlocks is probably not evenly divisible)
	size_t numBigBlocks = numFrames % numBlocks;

	// Allocate the big blocks
	messageManager()->sendInfo(QString("Allocating %1 big blocks outside of OpenCL").arg(numBigBlocks));
	for (unsigned int block = 0; block < numBigBlocks; block++)
	{
		framePointers[block].length = (1 + framesPerBlock) * frameSize;
		framePointers[block].data = new unsigned char[framePointers[block].length];
	}

	// Then the small ones
	messageManager()->sendInfo(QString("Allocating %1 small blocks outside of OpenCL").arg(numBlocks - numBigBlocks));
	for (int block = numBigBlocks; block < numBlocks; block++)
	{
		framePointers[block].length = (framesPerBlock) * frameSize;
		framePointers[block].data = new unsigned char[framePointers[block].length];
	}

	// Now fill them
	unsigned int frame = 0;
	for (int block = 0; block < numBlocks; block++)
	{
		for (unsigned int frameInThisBlock = 0; frameInThisBlock < framePointers[block].length / frameSize; frameInThisBlock++)
		{
			memcpy(&(framePointers[block].data[frameInThisBlock * frameSize]), inputFrames->getFrame(frame), frameSize);
			frame++;
		}
	}
	return true;
}

bool TordAlgorithm::reconstruct(ProcessedUSInputDataPtr input, vtkImageDataPtr outputData, float radius, int nClosePlanes)
{
	OpenCL::ocl_version opencl_version = OpenCL::versionSupported(mOpenCL);
	// If the device supports OpenCL version 1.2 or higher, we want to use an image array for the bscans
	// instead of a normal buffer block
	int numBlocks = 10; // FIXME? needs to be the same as the number of input bscans to the voxel_method kernel
#ifdef CL_VERSION_1_2
	bool use_image_array = false; //opencl_version >= OpenCL::V_1_2;
	messageManager()->sendInfo(QString("Use image array: %1").arg(use_image_array));
	if(use_image_array)
	{
		// Put all B-scans in a big block, we don't need to split it up when using an image array.
		numBlocks = 1;
	}
#endif
	// Split input US into blocks
	// Splits and copies data from the processed input in the way the kernel will processes it, which is per frameBlock
	frameBlock_t* inputBlocks = new frameBlock_t[numBlocks];
	size_t nPlanes_numberOfInputImages = input->getDimensions()[2];
	this->initializeFrameBlocks(inputBlocks, numBlocks, input);

	VECTOR_CLASS<cl::Buffer> clBlocks;
#ifdef CL_VERSION_1_2
	cl::Image2DArray clBscans;
	if(use_image_array)
	{
		clBscans = OpenCL::createImage2DArray(mOpenCL->context,
		                                      CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
		                                      cl::ImageFormat(CL_R, CL_UNSIGNED_INT8),
		                                      nPlanes_numberOfInputImages,
		                                      input->getDimensions()[0],
		                                      input->getDimensions()[1],
		                                      input->getDimensions()[0],
		                                      input->getDimensions()[0]*input->getDimensions()[1],
		                                      inputBlocks[0].data,
		                                      "BScans image array");
	}
	else
	{
#endif
		// Allocate CL memory for each frame block
		messageManager()->sendInfo("Allocating OpenCL input block buffers");
		for (int i = 0; i < numBlocks; i++)
		{
			//TODO why does the context suddenly contain a "dummy" device?
			cl::Buffer buffer = OpenCL::createBuffer(mOpenCL->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, inputBlocks[i].length, inputBlocks[i].data, "block buffer "+QString::number(i).toStdString());
			clBlocks.push_back(buffer);
		}
#ifdef CL_VERSION_1_2
	}
#endif
	// Allocate output memory
	int *outputDims = outputData->GetDimensions();

	size_t outputVolumeSize = outputDims[0] * outputDims[1] * outputDims[2] * sizeof(unsigned char);

	messageManager()->sendInfo(QString("Allocating CL output buffer, size %1").arg(outputVolumeSize));

	cl_ulong globalMemUse = 10 * inputBlocks[0].length + outputVolumeSize + sizeof(float) * 16 * nPlanes_numberOfInputImages + sizeof(cl_uchar) * input->getDimensions()[0] * input->getDimensions()[1];
	if(isUsingTooMuchMemory(outputVolumeSize, inputBlocks[0].length, globalMemUse))
		return false;

	cl::Buffer outputBuffer = OpenCL::createBuffer(mOpenCL->context, CL_MEM_WRITE_ONLY, outputVolumeSize, NULL, "output volume buffer");

	// Fill the plane matrices
	float *planeMatrices = new float[16 * nPlanes_numberOfInputImages]; //4x4 (matrix) = 16
	this->fillPlaneMatrices(planeMatrices, input);

	cl::Buffer clPlaneMatrices = OpenCL::createBuffer(mOpenCL->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, nPlanes_numberOfInputImages * sizeof(float) * 16, planeMatrices, "plane matrices buffer");

	// US Probe mask
	cl::Buffer clMask = OpenCL::createBuffer(mOpenCL->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(cl_uchar) * input->getMask()->GetDimensions()[0] * input->getMask()->GetDimensions()[1],
			input->getMask()->GetScalarPointer(), "mask buffer");

	double *out_spacing = outputData->GetSpacing();
	float spacings[2];

	float f_out_spacings[3];
	f_out_spacings[0] = out_spacing[0];
	f_out_spacings[1] = out_spacing[1];
	f_out_spacings[2] = out_spacing[2];



	spacings[0] = input->getSpacing()[0];
	spacings[1] = input->getSpacing()[1];

	//TODO why 4? because float4 is used??
	size_t planes_eqs_size =  sizeof(cl_float)*4*nPlanes_numberOfInputImages;

	// Find the optimal local work size
	size_t local_work_size;
	mKernel.getWorkGroupInfo(mOpenCL->device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, &local_work_size);

	size_t close_planes_size = this->calculateSpaceNeededForClosePlanes(mKernel, mOpenCL->device, local_work_size, nPlanes_numberOfInputImages, nClosePlanes);

	this->setKernelArguments(
			mKernel,
			outputDims[0],
			outputDims[1],
			outputDims[2],
			f_out_spacings[0],
			f_out_spacings[1],
			f_out_spacings[2],
			input->getDimensions()[0],
			input->getDimensions()[1],
			spacings[0],
			spacings[1],
			clBlocks,
			outputBuffer,
			clPlaneMatrices,
			clMask,
			planes_eqs_size,
			close_planes_size,
#ifdef CL_VERSION_1_2
			radius,
			clBscans,
			use_image_array
#else
			radius
#endif
		);

	messageManager()->sendInfo(QString("Using %1 as local workgroup size").arg(local_work_size));

	// We will divide the work into cubes of CUBE_DIM^3 voxels. The global work size is the total number of voxels divided by that.
	int cube_dim = 4;
	int cube_dim_pow3 = cube_dim * cube_dim * cube_dim;
	// Global work items:
	size_t global_work_size = (((outputDims[0] + cube_dim) * (outputDims[1] + cube_dim) * (outputDims[2] + cube_dim)) / cube_dim_pow3); // = number of cubes = number of kernels to run

	// Round global_work_size up to nearest multiple of local_work_size
	if (global_work_size % local_work_size)
		global_work_size = ((global_work_size / local_work_size) + 1) * local_work_size; // ceil(...)

	OpenCL::executeKernel(mOpenCL->cmd_queue, mKernel, global_work_size, local_work_size);
	OpenCL::readResultingVolume(mOpenCL->cmd_queue, outputBuffer, outputVolumeSize, outputData->GetScalarPointer());

	// Cleaning up
	messageManager()->sendInfo(QString("Done, freeing GPU memory"));
	this->freeFrameBlocks(inputBlocks, numBlocks);
	delete[] inputBlocks;

	inputBlocks = NULL;

	return true;
}

void TordAlgorithm::fillPlaneMatrices(float *planeMatrices, ProcessedUSInputDataPtr input)
{
	std::vector<TimedPosition> vecPosition = input->getFrames();

	// Sanity check on the number of frames
	if (input->getDimensions()[2] != vecPosition.end() - vecPosition.begin())
	{
		messageManager()->sendError(QString("Number of frames %1 != %2 dimension 2 of US input").arg(input->getDimensions()[2]).arg(vecPosition.end() - vecPosition.begin()));
		return;
	}

	int i = 0;
	for (std::vector<TimedPosition>::iterator it = vecPosition.begin(); it != vecPosition.end(); ++it)
	{
		Transform3D pos = it->mPos;

		// Now store the result in the output
		for (int j = 0; j < 16; j++)
		{
			planeMatrices[i++] = pos(j / 4, j % 4);
		}
	}
}

void TordAlgorithm::freeFrameBlocks(frameBlock_t *framePointers, int numBlocks)
{
	for (int i = 0; i < numBlocks; i++)
	{
		delete[] framePointers[i].data;
	}
}

void TordAlgorithm::setKernelArguments(
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
#ifdef CL_VERSION_1_2
		float radius,
		cl::Image2DArray clBscans,
		bool use_image_array
#else
		float radius
#endif
	)
{
	int arg = 0;
	kernel.setArg(arg++, volume_xsize);
	kernel.setArg(arg++, volume_ysize);
	kernel.setArg(arg++, volume_zsize);
	kernel.setArg(arg++, volume_xspacing);
	kernel.setArg(arg++, volume_yspacing);
	kernel.setArg(arg++, volume_zspacing);
	kernel.setArg(arg++, in_xsize);
	kernel.setArg(arg++, in_ysize);
	kernel.setArg(arg++, in_xspacing);
	kernel.setArg(arg++, in_yspacing);
#ifdef CL_VERSION_1_2
	if(use_image_array)
	{
		kernel.setArg(arg++, clBscans);
	}
	else {
#endif
	for (int i = 0; i < blocks.size(); i++)
	{
		kernel.setArg(arg++, blocks[i]);
	}
#ifdef CL_VERSION_1_2
	}
#endif
	kernel.setArg(arg++, out_volume);
	kernel.setArg(arg++, plane_matrices);
	kernel.setArg(arg++, mask);
	kernel.setArg<cl::LocalSpaceArg>(arg++, cl::__local(plane_eqs_size));
	kernel.setArg<cl::LocalSpaceArg>(arg++, cl::__local(close_planes_size));
	kernel.setArg(arg++, radius);
}

size_t TordAlgorithm::calculateSpaceNeededForClosePlanes(cl::Kernel kernel, cl::Device device, size_t local_work_size, size_t nPlanes_numberOfInputImages, int nClosePlanes)
{
	// Find out how much local memory the device has
	size_t dev_local_mem_size;
	dev_local_mem_size = device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();

	// Find the maximum work group size
	size_t max_work_size;
	kernel.getWorkGroupInfo(device, CL_KERNEL_WORK_GROUP_SIZE, &max_work_size);

	// Now find the largest multiple of the preferred work group size that will fit into local mem
	size_t constant_local_mem = sizeof(cl_float) * 4 * nPlanes_numberOfInputImages;

	size_t varying_local_mem = (sizeof(cl_float) + sizeof(cl_short) + sizeof(cl_uchar) + sizeof(cl_uchar)) * (nClosePlanes + 1);  //see _close_plane struct in kernels.cl
	messageManager()->sendInfo(QString("Device has %1 bytes of local memory").arg(dev_local_mem_size));
	dev_local_mem_size -= constant_local_mem + 128; //Hmmm? 128?

	// How many work items can the local mem support?
	size_t maxItems = dev_local_mem_size / varying_local_mem;
	// And what is the biggest multiple of local_work_size that fits into that?
	int multiple = maxItems / local_work_size;

	if(multiple == 0)
	{
		// If the maximum amount of work items is smaller than the preferred multiple, we end up here.
		// This means that the local memory use is so big we can't even fit into the preferred multiple, and
		// have use a sub-optimal local work size.
		local_work_size = std::min(max_work_size, maxItems);
	}
	else
	{
		// Otherwise, we make it fit into the local work size.
		local_work_size = std::min(max_work_size, multiple * local_work_size);
	}

	size_t close_planes_size = varying_local_mem*local_work_size;

	return close_planes_size;
}

bool TordAlgorithm::isUsingTooMuchMemory(size_t outputVolumeSize, size_t inputBlocksLength, cl_ulong globalMemUse)
{
	bool usingTooMuchMemory = false;

	cl_ulong maxAllocSize = mOpenCL->device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();
	cl_ulong globalMemSize = mOpenCL->device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
	if (maxAllocSize < outputVolumeSize)
	{
		messageManager()->sendError(QString("Output volume size too large! %1 > %2\n").arg(outputVolumeSize).arg(maxAllocSize));
		usingTooMuchMemory = true;
	}

	if (maxAllocSize < inputBlocksLength)
	{
		messageManager()->sendError(QString("Input blocks too large! %1 > %2\n").arg(inputBlocksLength).arg(maxAllocSize));
		usingTooMuchMemory = true;
	}

	if (globalMemSize < globalMemUse)
	{
		messageManager()->sendError(QString("Using too much global memory! %1 > %2").arg(globalMemUse).arg(globalMemSize));
		usingTooMuchMemory = true;
	}

	messageManager()->sendInfo(QString("Using %1 of %2 global memory").arg(globalMemUse).arg(globalMemSize));
	return usingTooMuchMemory;
}

} /* namespace cx */
