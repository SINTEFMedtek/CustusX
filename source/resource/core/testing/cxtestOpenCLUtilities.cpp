#ifdef SSC_USE_OpenCL

#include "catch.hpp"
#include "cxOpenCLUtilities.h"
#include "sscMessageManager.h"
#include "cxtestOpenCLFixture.h"

namespace cxtest
{

TEST_CASE("OpenCLUtilities: Can initialize OpenCL using GPU", "[unit][OpenCL][OpenCLUtilities][GPU]")
{
	OpenCLFixture fixture;
	cx::OpenCL::ocl* opencl = cx::OpenCL::init(CL_DEVICE_TYPE_GPU);
	CHECK(opencl);
	cx::OpenCL::release(opencl);
}

TEST_CASE("OpenCLUtilities: Can create a small global OpenCL buffer using GPU context", "[unit][OpenCL][OpenCLUtilities][GPU]")
{
	OpenCLFixture fixture;
	cx::OpenCL::ocl* opencl = cx::OpenCL::init(CL_DEVICE_TYPE_GPU);
	REQUIRE(opencl);

	size_t size = sizeof(cl_char);
	cl::Buffer buffer = cx::OpenCL::createBuffer(opencl->context, CL_MEM_READ_WRITE, size, NULL, "global test buffer");

	cx::OpenCL::release(opencl);
}

TEST_CASE("OpenCLUtilities: Can create a small kernel, build a program and run it on a GPU", "[unit][OpenCL][OpenCLUtilities][GPU]")
{
	OpenCLFixture fixture;
	CHECK(fixture.runTestKernel(CL_DEVICE_TYPE_GPU));
}

}
#endif //SSC_USE_OpenCL
