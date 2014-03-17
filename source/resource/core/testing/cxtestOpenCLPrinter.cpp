#include "catch.hpp"
#include "cxOpenCLPrinter.h"

namespace cxtest
{

TEST_CASE("OpenCLPrinter: print info about platform and devices", "[integration][broken][OpenCL][OpenCLPrinter]")
{
	cx::OpenCLPrinter::printPlatformAndDeviceInfo();
	CHECK(true);
}

}
