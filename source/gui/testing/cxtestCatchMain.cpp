#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "sscApplication.h"
#include <QTimer>

int main (int argc, char* argv[])
{
	ssc::Application app( argc, argv );

	int result = Catch::Main( argc, argv );

	return result;
}
