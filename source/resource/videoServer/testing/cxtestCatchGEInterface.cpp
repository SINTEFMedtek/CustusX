// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "catch.hpp"
#include "cxTestGEInterface.h"

namespace cxtest
{

TEST_CASE("GEInterface can stream 2D CPU scanconverted stream with auto size", "[integration]")
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "0"; //Test only CPU scan conversion

	TestGEInterface helper;
	helper.testStream(args);
}

TEST_CASE("GEInterface can stream 2D CPU scanconverted stream with defined size", "[integration]")
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "0"; //Test only CPU scan conversion
	args["imagesize"] = "500*500";

	TestGEInterface helper;
	helper.testStream(args);
}
TEST_CASE("GEInterface can stream 3D CPU scanconverted stream with auto size", "[integration]")
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "0"; //Test only CPU scan conversion
	args["test"] = "3D";
	args["imagesize"] = "auto";

	TestGEInterface helper;
	helper.testStream(args); //3D
}
TEST_CASE("GEInterface can stream 3D CPU scanconverted stream with defined size", "[integration]")
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "0"; //Test only CPU scan conversion
	args["test"] = "3D";
	args["imagesize"] = "100*100*100";

	TestGEInterface helper;
	helper.testStream(args); //set size
}

TEST_CASE("GEInterface can stream all 2D streams with GPU scanconversion where available", "[integration]")
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	//args["ip"] = "bhgrouter.hopto.org";
	//args["test"] = "no";
	args["useOpenCL"] = "1"; //Test GPU (OpenCL) scan conversion
	args["streams"] = "all";

	TestGEInterface helper;
	helper.testStream(args);
}

TEST_CASE("GEInterface can stream 2D scanconverted stream, with GPU scanconversion where available", "[integration]")
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "1"; //Test GPU (OpenCL) scan conversion
	args["streams"] = "scanconverted";

	TestGEInterface helper;
	helper.testStream(args);
}

TEST_CASE("GEInterface can stream 2D tissue stream, with GPU scanconversion where available", "[integration]")
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "1"; //Test GPU (OpenCL) scan conversion
	args["streams"] = "tissue";

	TestGEInterface helper;
	helper.testStream(args);
}
TEST_CASE("GEInterface can stream 2D frequency stream with GPU scanconversion where available", "[integration]")
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "1"; //Test GPU (OpenCL) scan conversion
	args["streams"] = "frequency";

	TestGEInterface helper;
	helper.testStream(args);
}
TEST_CASE("GEInterface can stream 2D bandwidth stream with GPU scanconversion where available", "[integration]")
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "1"; //Test GPU (OpenCL) scan conversion
	args["streams"] = "bandwidth";

	TestGEInterface helper;
	helper.testStream(args);
}
TEST_CASE("GEInterface can stream 2D velocity stream with GPU scanconversion where available", "[integration]")
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "1"; //Test GPU (OpenCL) scan conversion
	args["streams"] = "velocity";

	TestGEInterface helper;
	helper.testStream(args);
}
TEST_CASE("GEInterface can stream default 2D streams with GPU scanconversion where available", "[integration]")
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "1"; //Test GPU (OpenCL) scan conversion

	TestGEInterface helper;
	helper.testStream(args);
}

TEST_CASE("GEStreamer can stream 3D scanconverted stream with GPU scanconversion", "[integration]")
{
	TestGEInterface helper;
	helper.testGEStreamer();
}

} // namespace cxtest

