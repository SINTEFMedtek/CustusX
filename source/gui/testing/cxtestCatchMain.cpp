#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "sscApplication.h"

int main (int argc, char* argv[])
{
	ssc::Application app( argc, argv );

	int result = Catch::Main( argc, argv );
	app.exec();

	return result;
}
