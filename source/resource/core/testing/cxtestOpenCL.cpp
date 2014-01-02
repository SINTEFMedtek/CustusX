#ifdef SSC_USE_OpenCL

#include "catch.hpp"
#include "cxOpenCLPrinter.h"

namespace cxtest
{

TEST_CASE("OpenCL: print info about platform and devices", "[unit][OpenCL]")
{
	cx::OpenCLPrinter::printPlatformAndDeviceInfo();
	CHECK(true);
}

}
#endif //SSC_USE_OpenCL
