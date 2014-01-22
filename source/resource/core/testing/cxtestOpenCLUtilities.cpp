#ifdef SSC_USE_OpenCL

#include "catch.hpp"
#include "cxOpenCLUtilities.h"
#include "sscMessageManager.h"

namespace cxtest
{

//TODO make OpenCL testing fixture, waiting for common opencl library
bool runTestKernel(cx::OpenCL::ocl* opencl)
{
	cx::messageManager()->initialize();
	const char* source = "__kernel void test_kernel(void){int i = 0; i+=i;}";
	bool build = true;
	cl::Program::Sources sources(1, std::pair<const char*, ::size_t>(source, strlen(source)));
	VECTOR_CLASS<cl::Device> devices;
	devices.push_back(opencl->device);

	bool success = true;
	try
	{
		cl::Program program(opencl->context, sources);
		program.build(devices, "", NULL, NULL);
		cl::Kernel kernel = cx::OpenCL::createKernel(program, opencl->device, "test_kernel");
		size_t numberOfWorkingItems;
		kernel.getWorkGroupInfo(opencl->device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, &numberOfWorkingItems);
		int numberOfWorkGroups = 2;
		size_t totalNumberOfItems = numberOfWorkGroups*numberOfWorkingItems; //is the mental picture correct???
		opencl->cmd_queue.enqueueNDRangeKernel(kernel, 0, totalNumberOfItems, numberOfWorkingItems, NULL, NULL);
		opencl->cmd_queue.finish();
	}
	catch(cl::Error error)
	{
		std::cerr << "Could not execute the kernel. Reason: " << error.what() << std::endl;
		success = false;
	}
	return success;
	cx::messageManager()->shutdown();
}

TEST_CASE("OpenCLUtilities: Can initialize OpenCL using CPU", "[unit][OpenCL][OpenCLUtilities][CPU]")
{
	cx::OpenCL::ocl* opencl = cx::OpenCL::init(CL_DEVICE_TYPE_CPU);
	CHECK(opencl);
	cx::OpenCL::release(opencl);
}

TEST_CASE("OpenCLUtilities: Can initialize OpenCL using GPU", "[unit][OpenCL][OpenCLUtilities][GPU]")
{
	cx::OpenCL::ocl* opencl = cx::OpenCL::init(CL_DEVICE_TYPE_GPU);
	CHECK(opencl);
	cx::OpenCL::release(opencl);
}

TEST_CASE("OpenCLUtilities: Can create a small global OpenCL buffer using CPU device", "[unit][OpenCL][OpenCLUtilities][CPU]")
{
	cx::OpenCL::ocl* opencl = cx::OpenCL::init(CL_DEVICE_TYPE_CPU);
	REQUIRE(opencl);

	size_t size = sizeof(cl_char);
	cl::Buffer buffer = cx::OpenCL::createBuffer(opencl->context, CL_MEM_READ_WRITE, size, NULL, "global test buffer");

	cx::OpenCL::release(opencl);
}

TEST_CASE("OpenCLUtilities: Can create a small kernel, build a program and run it on a CPU", "[unit][OpenCL][OpenCLUtilities][CPU]")
{
	cx::OpenCL::ocl* opencl = cx::OpenCL::init(CL_DEVICE_TYPE_CPU);
	REQUIRE(opencl);

	CHECK(runTestKernel(opencl));

	cx::OpenCL::release(opencl);
}

TEST_CASE("OpenCLUtilities: Can create a small kernel, build a program and run it on a GPU", "[unit][OpenCL][OpenCLUtilities][CPU]")
{
	cx::OpenCL::ocl* opencl = cx::OpenCL::init(CL_DEVICE_TYPE_GPU);
	REQUIRE(opencl);

	CHECK(runTestKernel(opencl));

	cx::OpenCL::release(opencl);
}

}
#endif //SSC_USE_OpenCL
