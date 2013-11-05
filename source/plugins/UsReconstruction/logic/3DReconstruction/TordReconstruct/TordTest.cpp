#include "TordTest.h"
#include "sscMessageManager.h"
#include <vtkImageData.h>
#include <QDomElement>
#include <iostream>
#include <Thunder/utils.h>
#include <recConfig.h>
#include <sscUSFrameData.h>

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

std::vector<DataAdapterPtr>
TordTest::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;

	retval.push_back(this->getMethodOption(root));
	retval.push_back(this->getRadiusOption(root));
	retval.push_back(this->getPlaneMethodOption(root));
	retval.push_back(this->getMaxPlanesOption(root));
	return retval;
}

StringDataAdapterXmlPtr
TordTest::getMethodOption(QDomElement root)
{
	QStringList methods;
	for(std::vector<QString>::iterator it = mMethods.begin();
	    it != mMethods.end();
	    it++)
	{
		QString method = *it;
		methods << method;
	}
	return StringDataAdapterXml::initialize("Method",
	                                        "",
	                                        "Which algorithm to use for reconstruction",
	                                        methods[0],
	                                        methods,
	                                        root);
}

StringDataAdapterXmlPtr
TordTest::getPlaneMethodOption(QDomElement root)
{
	QStringList methods;
	for(std::vector<QString>::iterator it = mPlaneMethods.begin();
	    it != mPlaneMethods.end();
	    it++)
	{
		QString method = *it;
		methods << method;
	}
	return StringDataAdapterXml::initialize("Plane method",
	                                        "",
	                                        "Which method to use for finding close planes",
	                                        methods[0],
	                                        methods,
	                                        root);
}

DoubleDataAdapterXmlPtr
TordTest::getRadiusOption(QDomElement root)
{
	return DoubleDataAdapterXml::initialize("Radius (mm)", "",
	                                        "Radius of kernel. mm.",
	                                        1,
	                                        DoubleRange(0.1, 10, 0.1),
	                                        1,
	                                        root);
}

DoubleDataAdapterXmlPtr
TordTest::getMaxPlanesOption(QDomElement root)
{
	return DoubleDataAdapterXml::initialize("nPlanes", "",
	                                     "Number of planes to include in closest planes",
	                                     8,
	                                     DoubleRange(1, 200, 1),
	                                     0,
	                                     root);
}

int
TordTest::getMethodID(QDomElement root)
{
	return find(mMethods.begin(), mMethods.end(),
	            this->getMethodOption(root)->getValue()
		) - mMethods.begin();
}

int
TordTest::getPlaneMethodID(QDomElement root)
{
	return find(mPlaneMethods.begin(), mPlaneMethods.end(),
	            this->getPlaneMethodOption(root)->getValue()
		) - mPlaneMethods.begin();
}

bool
TordTest::initCL(QString kernelPath,
                 int nMaxPlanes,
                 int nPlanes,
                 int method,
                 int planeMethod)
{
	// Reusing initialization code from Thunder
	moClContext = ocl_init("GPU");

	size_t sourceLen;

	messageManager()->sendInfo(QString("Kernel path: %1").arg(kernelPath));
	char* sSource = file2string(kernelPath.toLocal8Bit().data(),
	                             &sourceLen);

	cl_program clprogram = this->buildCLProgram(sSource,
	                                            nMaxPlanes,
	                                            nPlanes,
	                                            method,
	                                            planeMethod,
	                                            kernelPath);

	if(clprogram == NULL) return false;

	mClKernel = ocl_kernel_build(clprogram,
	                             moClContext->device, "voxel_methods");
	return true;
	
}

cl_program
TordTest::buildCLProgram(const char* program_src,
                         int nMaxPlanes,
                         int nPlanes,
                         int method,
                         int planeMethod,
                         QString kernelPath)
{
	cl_program retval;
	cl_int err;
	retval = clCreateProgramWithSource(moClContext->context,
	                                   1,
	                                   (const char **) &program_src,
	                                   0,
	                                   &err);

	ocl_check_error(err);

	QString define = "-D MAX_PLANES=%1 -D N_PLANES=%2 -D METHOD=%3 -D PLANE_METHOD=%4";
	define = define.arg(nMaxPlanes).arg(nPlanes).arg(method).arg(planeMethod);

	err = clBuildProgram(retval, 0, NULL, define.toStdString().c_str(), 0, 0);
	
	if (err != CL_SUCCESS)
	{
		size_t len;
		char buffer[512 * 512];
		memset(buffer, 0, 512 * 512);
		printf("OpenCL ERROR: Failed to build program on device %p. Error code: %d\n",
		       moClContext->device, err);
		printf("Build log for program %s:\n", kernelPath.toStdString().c_str());

		clGetProgramBuildInfo(retval, // the program object being queried
			moClContext->device, // the device for which the OpenCL code was built
			CL_PROGRAM_BUILD_LOG, // specifies that we want the build log
			sizeof(char) * 512 * 512, // the size of the buffer
			buffer, // on return, holds the build log
			&len); // on return, the actual size in bytes of the data returned

		printf("%lu %s\n", len, buffer);
		for (uint i = 0; i < len; i++)
			printf("%c", buffer[i]);
		printf("\n");
		return NULL;
	}
	return retval;

}

bool
TordTest::initializeFrameBlocks(frameBlock_t* framePointers,
                                int numBlocks,
                                ProcessedUSInputDataPtr inputFrames)
{
	// Compute the size of each frame in bytes
	Eigen::Array3i dims = inputFrames->getDimensions();
	size_t frameSize = dims[0]*dims[1];
	size_t numFrames = dims[2];
	messageManager()->sendInfo(QString("Input dims: %1 %2 %3")
	                           .arg(dims[0])
	                           .arg(dims[1])
	                           .arg(dims[2]));

		// Find out how many frames needs to be in each block
	size_t framesPerBlock = numFrames / numBlocks;
	messageManager()->sendInfo(QString("Frames: %1 Blocks: %2 Frames per block: %3")
	                           .arg(numFrames)
	                           .arg(numBlocks)
	                           .arg(framesPerBlock));
	
	// Some blocks will need to contain one extra frame
	// (numFrames and numBlocks is probably not evenly divisible)
	size_t numBigBlocks = numFrames % numBlocks;
	messageManager()->sendInfo(QString("Allocating %1 big blocks")
	                           .arg(numBigBlocks));

	// Allocate the big blocks
	for(unsigned int block = 0; block < numBigBlocks; block++)
	{
		framePointers[block].length = (1+framesPerBlock)*frameSize;
		framePointers[block].data = new unsigned char[framePointers[block].length];
	}
	messageManager()->sendInfo(QString("Allocating %1 small blocks")
	                           .arg(numBlocks-numBigBlocks));

	// Then the small ones
	for(int block = numBigBlocks; block < numBlocks; block++)
	{
		framePointers[block].length = (framesPerBlock)*frameSize;
		framePointers[block].data = new unsigned char[framePointers[block].length];
	}
	
	// Now fill them
	unsigned int frame = 0;
	for(int block = 0; block < numBlocks; block++)
	{
		for(unsigned int frameInThisBlock = 0;
		    frameInThisBlock < framePointers[block].length/frameSize;
		    frameInThisBlock++) {

			memcpy(&(framePointers[block].data[frameInThisBlock*frameSize]),
			       inputFrames->getFrame(frame),
			       frameSize);

			frame++;
		}
	}	
	return true;
}

bool
TordTest::doGPUReconstruct(ProcessedUSInputDataPtr input,
                           vtkImageDataPtr outputData,
                           float radius)
{
	int numBlocks = 10; // FIXME?
	// Split input US into blocks
	frameBlock_t* inputBlocks = new frameBlock_t[numBlocks];
	
	this->initializeFrameBlocks(inputBlocks, numBlocks, input);

	// Allocate CL memory for each frame block
	cl_mem *clBlocks = new cl_mem[numBlocks];
	messageManager()->sendInfo("Allocating CL input block buffers");

	for(int i = 0; i < numBlocks; i++)
	{
		clBlocks[i] = ocl_create_buffer(moClContext->context,
		                                CL_MEM_READ_ONLY,
		                                inputBlocks[i].length,
		                                inputBlocks[i].data);
		                                
	}

	// Free the local frameblock buffers
	this->freeFrameBlocks(inputBlocks, numBlocks);
	delete [] inputBlocks;
	inputBlocks = NULL;

	// Allocate output memory
	int *outputDims = outputData->GetDimensions();
	
	size_t outputVolumeSize =
		outputDims[0]*outputDims[1]*outputDims[2]*sizeof(unsigned char);

	messageManager()->sendInfo(QString("Allocating CL output buffer, size %1")
	                           .arg(outputVolumeSize));
	cl_mem clOutputVolume = ocl_create_buffer(moClContext->context,
	                                          CL_MEM_WRITE_ONLY,
	                                          outputVolumeSize,
	                                          NULL);

	// Fill the plane matrices
	size_t nPlanes = input->getDimensions()[2];

	float *planeMatrices = new float[16*nPlanes];

	this->fillPlaneMatrices(planeMatrices, input);

	cl_mem clPlaneMatrices = ocl_create_buffer(moClContext->context,
	                                           CL_MEM_READ_ONLY,
	                                           nPlanes*sizeof(float)*16,
	                                           planeMatrices);

	// US Probe mask

	cl_mem clMask = ocl_create_buffer(moClContext->context,
	                                  CL_MEM_READ_ONLY,
	                                  sizeof(cl_uchar)*
	                                  input->getDimensions()[0]*input->getDimensions()[1],
	                                  input->getMask()->GetScalarPointer());
	

	
	// Set kernel args
	int arg = 0;
	// volume_xsize
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_int), &outputDims[0]));
	// volume_ysize
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_int), &outputDims[1]));
	// volume_zsize
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_int), &outputDims[2]));
	
	double *out_spacing = outputData->GetSpacing();
	float spacings[3];
	spacings[0] = out_spacing[0];
	spacings[1] = out_spacing[1];
	spacings[2] = out_spacing[2];
	
	// volume_xspacing
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &spacings[0]));
	// volume_yspacing
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &spacings[1]));
	// volume_zspacing
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &spacings[2]));

	// in_xsize
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_int), &input->getDimensions()[0]));
	// in_ysize
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_int), &input->getDimensions()[1]));
	
	spacings[0] = input->getSpacing()[0];
	spacings[1] = input->getSpacing()[1];

	// in_xspacing
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &spacings[0]));
	// in_yspacing
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &spacings[1]));
	
	// The input blocks
	for(int i = 0; i < numBlocks; i++)
	{
		ocl_check_error(clSetKernelArg(mClKernel, arg++,sizeof(cl_mem),&clBlocks[i]));
	}
	
	// out_volume
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_mem), &clOutputVolume));
	// plane_matrices
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_mem), &clPlaneMatrices));


	// US Probe mask
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_mem), &clMask));	

	// plane_eqs (local CL memory, will be calculated by the kernel)
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float)*4*nPlanes, NULL));

	// radius
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &radius));


	// Global work items:
	size_t local_work_size = 128;
	
	size_t global_work_size = (outputDims[0]*outputDims[2]);

	// Round global_work_size up to nearest multiple of local_work_size
	if(global_work_size % local_work_size)
		global_work_size = ((global_work_size/local_work_size) + 1)*local_work_size;

	
	messageManager()->sendInfo(QString("Executing kernel"));
	ocl_check_error(clEnqueueNDRangeKernel(moClContext->cmd_queue,
	                                       mClKernel,
	                                       1,
	                                       NULL,
	                                       &global_work_size,
	                                       &local_work_size,
	                                       0,
	                                       NULL,
	                                       NULL));
	

	// Read back data
	try {
		ocl_check_error(clEnqueueReadBuffer(moClContext->cmd_queue,
		                                    clOutputVolume,
		                                    CL_TRUE,
		                                    0,
		                                    outputVolumeSize,
		                                    outputData->GetScalarPointer(),
		                                    0, 0, 0));
	}
	catch (std::string& except)
	{
		messageManager()->sendInfo(QString((std::string("Got exception: ") + except).c_str()));
	}

	messageManager()->sendInfo(QString("Done, freeing GPU memory"));

	// Free the allocated cl memory objects
	for(int i = 0; i < numBlocks; i++)
	{
		ocl_check_error(clReleaseMemObject(clBlocks[i]));
	}
	ocl_check_error(clReleaseMemObject(clOutputVolume));
	ocl_check_error(clReleaseMemObject(clPlaneMatrices));
	ocl_check_error(clReleaseMemObject(clMask));
	
	return true;
}

void
TordTest::fillPlaneMatrices(float *planeMatrices,
                           ProcessedUSInputDataPtr input)
{
	std::vector<TimedPosition> vecPosition = input->getFrames();
	
	// Sanity check on the number of frames
	if(input->getDimensions()[2] != vecPosition.end() - vecPosition.begin())
	{
		messageManager()->sendError(QString("Number of frames %1 != %2 dimension 2 of US input")
		                            .arg(input->getDimensions()[2])
		                            .arg(vecPosition.end() - vecPosition.begin()));
		return;
	}

	int i = 0;
	for(std::vector<TimedPosition>::iterator it = vecPosition.begin();
	    it != vecPosition.end();
	    it++)
	{
		Transform3D pos = it->mPos;
	
		// Now store the result in the output
		for(int j = 0; j < 16; j++)
		{
			planeMatrices[i++] = pos(j/4, j%4);
		}
	}
}

void
TordTest::freeFrameBlocks(frameBlock_t *framePointers,
                          int numBlocks)
{
	for(int i = 0; i < numBlocks; i++)
	{
		delete [] framePointers[i].data;
	}
}

bool
TordTest::reconstruct(ProcessedUSInputDataPtr input,
                      vtkImageDataPtr outputData,
                      QDomElement settings)
{

	int nClosePlanes = getMaxPlanesOption(settings)->getValue();

	int method = getMethodID(settings);
	float radius = getRadiusOption(settings)->getValue();
	int planeMethod = getPlaneMethodID(settings);
	messageManager()->sendInfo(
		QString("Method: %1, radius: %2, planeMethod: %3, nClosePlanes: %4, nPlanes: %5 ")
		.arg(method)
		.arg(radius)
		.arg(planeMethod)
		.arg(nClosePlanes)
		.arg(input->getDimensions()[2]));

	if(!initCL(QString(TORD_KERNEL_PATH) + "/kernels.ocl",
	       nClosePlanes,
	       input->getDimensions()[2],
	       method,
	       planeMethod
		   )) return false;

	bool ret = doGPUReconstruct(input, outputData, radius );

	if(moClContext != NULL)
	{
		ocl_release(moClContext);
		moClContext = NULL;
	}
	
	return ret;
}

}


