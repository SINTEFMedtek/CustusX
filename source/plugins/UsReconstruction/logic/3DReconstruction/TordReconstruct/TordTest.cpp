#include "TordTest.h"

#include <iostream>
#include <vtkImageData.h>
#include <QDomElement>
#include <recConfig.h>
#include <sscUSFrameData.h>
#include "sscMessageManager.h"

#include "cxOpenCLPrinter.h"
#include "sscVolumeHelpers.h"

namespace cx
{


TordTest::TordTest()
{
	mMethods.push_back("VNN");
	mMethods.push_back("VNN2");
	mMethods.push_back("DW");
	mMethods.push_back("Anisotropic");
	mPlaneMethods.push_back("Heuristic");
	mPlaneMethods.push_back("Closest");
}

TordTest::~TordTest()
{

}

std::vector<DataAdapterPtr> TordTest::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;

	retval.push_back(this->getMethodOption(root));
	retval.push_back(this->getRadiusOption(root));
	retval.push_back(this->getPlaneMethodOption(root));
	retval.push_back(this->getMaxPlanesOption(root));
	retval.push_back(this->getNStartsOption(root));
	retval.push_back(this->getNewnessWeightOption(root));
	retval.push_back(this->getBrightnessWeightOption(root));
	return retval;
}

StringDataAdapterXmlPtr TordTest::getMethodOption(QDomElement root)
{
	QStringList methods;
	for (std::vector<QString>::iterator it = mMethods.begin(); it != mMethods.end(); it++)
	{
		QString method = *it;
		methods << method;
	}
	return StringDataAdapterXml::initialize("Method", "", "Which algorithm to use for reconstruction", methods[0],
			methods, root);
}

DoubleDataAdapterXmlPtr TordTest::getNewnessWeightOption(QDomElement root)
{
	return DoubleDataAdapterXml::initialize("Newness weight", "", "Newness weight", 0, DoubleRange(0.0, 10, 0.1), 1,
			root);
}

DoubleDataAdapterXmlPtr TordTest::getBrightnessWeightOption(QDomElement root)
{
	return DoubleDataAdapterXml::initialize("Brightness weight", "", "Brightness weight", 0, DoubleRange(0.0, 10, 0.1),
			1, root);
}

StringDataAdapterXmlPtr TordTest::getPlaneMethodOption(QDomElement root)
{
	QStringList methods;
	for (std::vector<QString>::iterator it = mPlaneMethods.begin(); it != mPlaneMethods.end(); it++)
	{
		QString method = *it;
		methods << method;
	}
	return StringDataAdapterXml::initialize("Plane method", "", "Which method to use for finding close planes",
			methods[0], methods, root);
}

DoubleDataAdapterXmlPtr TordTest::getRadiusOption(QDomElement root)
{
	return DoubleDataAdapterXml::initialize("Radius (mm)", "", "Radius of kernel. mm.", 1, DoubleRange(0.1, 10, 0.1), 1,
			root);
}

DoubleDataAdapterXmlPtr TordTest::getMaxPlanesOption(QDomElement root)
{
	return DoubleDataAdapterXml::initialize("nPlanes", "", "Number of planes to include in closest planes", 8,
			DoubleRange(1, 200, 1), 0, root);
}

DoubleDataAdapterXmlPtr TordTest::getNStartsOption(QDomElement root)
{
	return DoubleDataAdapterXml::initialize("nStarts", "", "Number of starts for multistart searchs", 3,
			DoubleRange(1, 8, 1), 0, root);
}

int TordTest::getMethodID(QDomElement root)
{
	return find(mMethods.begin(), mMethods.end(), this->getMethodOption(root)->getValue()) - mMethods.begin();
}

int TordTest::getPlaneMethodID(QDomElement root)
{
	return find(mPlaneMethods.begin(), mPlaneMethods.end(), this->getPlaneMethodOption(root)->getValue())
			- mPlaneMethods.begin();
}




bool TordTest::initCL(QString kernelPath, int nMaxPlanes, int nPlanes, int method, int planeMethod, int nStarts, float brightnessWeight, float newnessWeight)
{
	// INIT
	mOCL = OpenCL::init(CL_DEVICE_TYPE_GPU);

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

cl::Program TordTest::buildCLProgram(const char* program_src, int nMaxPlanes, int nPlanes, int method, int planeMethod, int nStarts, float newnessWeight, float brightnessWeight, QString kernelPath, size_t sourceLen)
{
	cl::Program retval;
	cl_int err = 0;
	VECTOR_CLASS<cl::Device> devices;
	try
	{
		cl::Program::Sources sources;
		sources.push_back(std::pair<const char*, ::size_t>(program_src, sourceLen));
		retval = cl::Program(mOCL->context, sources, &err);
		check_error(err);

		QString define = "-D MAX_PLANES=%1 -D N_PLANES=%2 -D METHOD=%3 -D PLANE_METHOD=%4 -D MAX_MULTISTART_STARTS=%5 -D NEWNESS_FACTOR=%6 -D BRIGHTNESS_FACTOR=%7 -cl-opt-disable";
		define = define.arg(nMaxPlanes).arg(nPlanes).arg(method).arg(planeMethod).arg(nStarts).arg(newnessWeight).arg(brightnessWeight);

		devices = mOCL->context.getInfo<CL_CONTEXT_DEVICES>();
		err = retval.build(devices, define.toStdString().c_str(), NULL, NULL);
	} catch (cl::Error error)
	{
		messageManager()->sendError("Could not build a OpenCL program. Reason: "+QString(error.what()));
		check_error(error.err());
	}
	return retval;
}

bool TordTest::initializeFrameBlocks(frameBlock_t* framePointers, int numBlocks, ProcessedUSInputDataPtr inputFrames)
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

bool TordTest::doGPUReconstruct(ProcessedUSInputDataPtr input, vtkImageDataPtr outputData, float radius, int nClosePlanes)
{
	int numBlocks = 10; // FIXME? needs to be the same as the number of input bscans to the voxel_method kernel

	// Split input US into blocks
	// Splits and copies data from the processed input in the way the kernel will processes it, which is per frameBlock
	frameBlock_t* inputBlocks = new frameBlock_t[numBlocks];
	size_t nPlanes_numberOfInputImages = input->getDimensions()[2];
	this->initializeFrameBlocks(inputBlocks, numBlocks, input);

	// Allocate CL memory for each frame block
	VECTOR_CLASS<cl::Buffer> clBlocks;
	messageManager()->sendInfo("Allocating OpenCL input block buffers");
	for (int i = 0; i < numBlocks; i++)
	{
		//TODO why does the context suddenly contain a "dummy" device?
		cl::Buffer buffer = OpenCL::createBuffer(mOCL->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, inputBlocks[i].length, inputBlocks[i].data, "block buffer "+QString::number(i).toStdString());
		clBlocks.push_back(buffer);
	}
	// Allocate output memory
	int *outputDims = outputData->GetDimensions();
	messageManager()->sendDebug("Output dimensions: "+QString::number(outputDims[0])+", "+QString::number(outputDims[1])+", "+QString::number(outputDims[2]));

	size_t outputVolumeSize = outputDims[0] * outputDims[1] * outputDims[2] * sizeof(unsigned char);
	messageManager()->sendDebug("Actual memory size: "+QString::number(outputData->GetActualMemorySize() * 1024));
	messageManager()->sendDebug("outputVolumeSize: "+QString::number(outputVolumeSize));

	messageManager()->sendInfo(QString("Allocating CL output buffer, size %1").arg(outputVolumeSize));

	cl_ulong maxAllocSize = mOCL->device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();
	cl_ulong globalMemSize = mOCL->device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();

	// Check memory sizes
	if (maxAllocSize < outputVolumeSize)
	{
		messageManager()->sendError(QString("Output volume size too large! %1 > %2\n").arg(outputVolumeSize).arg(maxAllocSize));
		return false;
	}

	if (maxAllocSize < inputBlocks[0].length)
	{
		messageManager()->sendError(QString("Input blocks too large! %1 > %2\n").arg(inputBlocks[0].length).arg(maxAllocSize));
		return false;
	}

	cl_ulong globalMemUse = 10 * inputBlocks[0].length + outputVolumeSize + sizeof(float) * 16 * nPlanes_numberOfInputImages + sizeof(cl_uchar) * input->getDimensions()[0] * input->getDimensions()[1];
	if (globalMemSize < globalMemUse)
	{
		messageManager()->sendError(QString("Using too much global memory! %1 > %2").arg(globalMemUse).arg(globalMemSize));
		return false;
	}

	messageManager()->sendInfo(QString("Using %1 of %2 global memory").arg(globalMemUse).arg(globalMemSize));


	cl::Buffer outputBuffer = OpenCL::createBuffer(mOCL->context, CL_MEM_WRITE_ONLY, outputVolumeSize, NULL, "output volume buffer");

	// Fill the plane matrices
	float *planeMatrices = new float[16 * nPlanes_numberOfInputImages]; //4x4 (matrix) = 16
	this->fillPlaneMatrices(planeMatrices, input);

	cl::Buffer clPlaneMatrices = OpenCL::createBuffer(mOCL->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, nPlanes_numberOfInputImages * sizeof(float) * 16, planeMatrices, "plane matrices buffer");

	// US Probe mask
	cl::Buffer clMask = OpenCL::createBuffer(mOCL->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(cl_uchar) * input->getMask()->GetDimensions()[0] * input->getMask()->GetDimensions()[1],
			input->getMask()->GetScalarPointer(), "mask buffer");

	// Set kernel arguments (voxel_methods)
	int arg = 0;
	mKernel.setArg(arg++, outputDims[0]); // volume_xsize
	mKernel.setArg(arg++, outputDims[1]); // volume_ysize
	mKernel.setArg(arg++, outputDims[2]); // volume_zsize

	double *out_spacing = outputData->GetSpacing();
	float spacings[3];
	spacings[0] = out_spacing[0];
	spacings[1] = out_spacing[1];
	spacings[2] = out_spacing[2];

	mKernel.setArg(arg++, spacings[0]); // volume_xspacing
	mKernel.setArg(arg++, spacings[1]); // volume_yspacing
	mKernel.setArg(arg++, spacings[2]); // volume_zspacing

	mKernel.setArg(arg++, input->getDimensions()[0]); // in_xsize
	mKernel.setArg(arg++, input->getDimensions()[1]); // in_ysize

	spacings[0] = input->getSpacing()[0];
	spacings[1] = input->getSpacing()[1];

	mKernel.setArg(arg++, spacings[0]); // in_xspacing
	mKernel.setArg(arg++, spacings[1]); // in_yspacing

	// The input blocks
	for (int i = 0; i < clBlocks.size(); i++)
	{
		mKernel.setArg(arg++, clBlocks[i]);
	}
	mKernel.setArg(arg++, outputBuffer); // out_volume

	mKernel.setArg(arg++, clPlaneMatrices); // plane_matrices

	mKernel.setArg(arg++, clMask); // US Probe mask

	//TODO why 4? because float4 is used??
	messageManager()->sendDebug("Allocating "+QString::number(sizeof(cl_float)*4*nPlanes_numberOfInputImages)+" bytes of local memory.");
	messageManager()->sendDebug("CL_DEVICE_LOCAL_MEM_SIZE "+QString::number(mOCL->device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>()));
	mKernel.setArg<cl::LocalSpaceArg>(arg++, cl::__local(sizeof(cl_float)*4*nPlanes_numberOfInputImages)); // plane_eqs (local CL memory, will be calculated by the kernel)

	// Find out how much local memory the device has
	size_t dev_local_mem_size;
	dev_local_mem_size = mOCL->device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();

	size_t local_work_size;
	// Find the optimal local work size
	mKernel.getWorkGroupInfo(mOCL->device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, &local_work_size);
	messageManager()->sendDebug("Kernels preferred work group size multiple is "+QString::number(local_work_size));
	// Find the maximum work group size
	size_t max_work_size;
	mKernel.getWorkGroupInfo(mOCL->device, CL_KERNEL_WORK_GROUP_SIZE, &max_work_size);
	messageManager()->sendDebug("Kernels max work group size is "+QString::number(max_work_size));

	// Now find the largest multiple of the preferred work group size that will fit into local mem
	size_t constant_local_mem = sizeof(cl_float) * 4 * nPlanes_numberOfInputImages;

	//FROM KERNELS.CL
	//	typedef struct _close_plane
//	{
//		float dist;
//		short plane_id;
//		unsigned char intensity;
//		unsigned char padding; // Align with 4
//	} close_plane_t;
	size_t varying_local_mem = (sizeof(cl_float) + sizeof(cl_short) + sizeof(cl_uchar) + sizeof(cl_uchar)) * (nClosePlanes + 1);  //see _close_plane struct
	messageManager()->sendInfo(QString("Device has %1 bytes of local memory").arg(dev_local_mem_size));
	dev_local_mem_size -= constant_local_mem + 128; //Hmmm? 128?

	// How many work items can the local mem support?
	int maxItems = dev_local_mem_size / varying_local_mem;
	// And what is the biggest multiple of local_work_size that fits into that?
	int multiple = maxItems / local_work_size;
	//TEST
	local_work_size = std::min(max_work_size, multiple * local_work_size);
	messageManager()->sendDebug("TEST: local_work_size "+QString::number(local_work_size));

	size_t close_planes_size = varying_local_mem*local_work_size;
	messageManager()->sendDebug("Allocating (close_planes_size) "+QString::number(close_planes_size)+" bytes of local memory.");
	mKernel.setArg<cl::LocalSpaceArg>(arg++, cl::__local(close_planes_size)); // close planes (local CL memory, to be used by the kernel)

	mKernel.setArg(arg++, radius); // radius

	messageManager()->sendInfo(QString("Using %1 as local workgroup size").arg(local_work_size));

	//TODO does not work according to google!?
	// Print local memory usage for debugging purposes
	cl_ulong local_mem_size;
	mKernel.getWorkGroupInfo(mOCL->device, CL_KERNEL_LOCAL_MEM_SIZE, &local_mem_size);
	messageManager()->sendInfo(QString("Kernel is using %1 bytes of local memory").arg(local_mem_size));

	// We will divide the work into cubes of CUBE_DIM^3 voxels. The global work size is the total number of voxels divided by that.
	int cube_dim = 4;
	int cube_dim_pow3 = cube_dim * cube_dim * cube_dim;
	// Global work items:
	size_t global_work_size = (((outputDims[0] + cube_dim) * (outputDims[1] + cube_dim) * (outputDims[2] + cube_dim)) / cube_dim_pow3); // = number of cubes = number of kernels to run

	// Round global_work_size up to nearest multiple of local_work_size
	if (global_work_size % local_work_size)
		global_work_size = ((global_work_size / local_work_size) + 1) * local_work_size; // ceil(...)

	messageManager()->sendInfo(QString("Executing kernel"));

	try
	{
		mOCL->cmd_queue.enqueueNDRangeKernel(mKernel, 0, global_work_size, local_work_size, NULL, NULL);
		mOCL->cmd_queue.finish();
	} catch (cl::Error error)
	{
		messageManager()->sendError("Could not execute kernels. Reason: "+QString(error.what()));
		check_error(error.err());
	}

	// Read back data
	try
	{
		mOCL->cmd_queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, outputVolumeSize, outputData->GetScalarPointer(), 0, 0);
	} catch (cl::Error error)
	{
		messageManager()->sendError("Could not read output volume buffer from OpenCL. Reason: "+QString(error.what()));
		check_error(error.err());
	}

	// Cleaning up
	messageManager()->sendInfo(QString("Done, freeing GPU memory"));
	this->freeFrameBlocks(inputBlocks, numBlocks);
	delete[] inputBlocks;
	inputBlocks = NULL;

	return true;
}

void TordTest::fillPlaneMatrices(float *planeMatrices, ProcessedUSInputDataPtr input)
{
	std::vector<TimedPosition> vecPosition = input->getFrames();

	// Sanity check on the number of frames
	if (input->getDimensions()[2] != vecPosition.end() - vecPosition.begin())
	{
		messageManager()->sendError(QString("Number of frames %1 != %2 dimension 2 of US input").arg(input->getDimensions()[2]).arg(vecPosition.end() - vecPosition.begin()));
		return;
	}

	int i = 0;
	for (std::vector<TimedPosition>::iterator it = vecPosition.begin(); it != vecPosition.end(); it++)
	{
		Transform3D pos = it->mPos;

		// Now store the result in the output
		for (int j = 0; j < 16; j++)
		{
			planeMatrices[i++] = pos(j / 4, j % 4);
		}
	}
}

void TordTest::freeFrameBlocks(frameBlock_t *framePointers, int numBlocks)
{
	for (int i = 0; i < numBlocks; i++)
	{
		delete[] framePointers[i].data;
	}
}

bool TordTest::reconstruct(ProcessedUSInputDataPtr input, vtkImageDataPtr outputData, QDomElement settings)
{

	int nClosePlanes = getMaxPlanesOption(settings)->getValue();

	int method = getMethodID(settings);
	float radius = getRadiusOption(settings)->getValue();
	int planeMethod = getPlaneMethodID(settings);
	int nStarts = getNStartsOption(settings)->getValue();
	float newnessWeight = getNewnessWeightOption(settings)->getValue();
	float brightnessWeight = getBrightnessWeightOption(settings)->getValue();

	messageManager()->sendInfo(
			QString("Method: %1, radius: %2, planeMethod: %3, nClosePlanes: %4, nPlanes: %5, nStarts: %6 ").arg(method).arg(
					radius).arg(planeMethod).arg(nClosePlanes).arg(input->getDimensions()[2]).arg(nStarts));

	if (!initCL(QString(TORD_KERNEL_PATH) + "/kernels.cl", nClosePlanes, input->getDimensions()[2], method,
			planeMethod, nStarts, newnessWeight, brightnessWeight))
		return false;

	bool ret = doGPUReconstruct(input, outputData, radius, nClosePlanes);

	if (mOCL != NULL)
	{
		OpenCL::release(mOCL);
		mOCL = NULL;
	}

	return ret;
}

}

