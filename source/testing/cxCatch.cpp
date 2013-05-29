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

///////////////////////////////////////////////////////////////////////////////
// CATCH TUTORIAL FOR CUSTUSX
///////////////////////////////////////////////////////////////////////////////

/**
 * This file describes how to use CATCH.
 *
 * ALL Catch tests should be tagged with ONE of the following tags:
 * (See https://en.wikipedia.org/wiki/Software_testing#Testing_levels for more information.)
 * - [unit]				Unit test - "...tests that verify the functionality of a specific section of code (...) usually at the class level"
 * - [integration]		Integration test - "... individual software modules are combined and tested as a group..."
 * - [system]			System test	- "...tests a completely integrated system..."
 * - [acceptance]		Acceptance test - "... a test conducted to determine if the requirements of a specification or contract are met..."
 *
 * The tag [hide] is a special tag, when tagging tests with this tag
 * they will be skipped by default.
 *
 * Run catch:
 * $ ./Catch
 *
 * See options:
 * $ ./Catch -?
 *
 * See help text for specific option:
 * $ ./Catch -t -?
 *
 * Read more about commandline options:
 * https://github.com/philsquared/Catch/wiki/Command-line
 *
 * Currently updated for:
 *  ----------------------------------------------------------
 *  CATCH v0.9 build 33 (integration branch)
 *  Generated: 2013-04-11 16:33:19.541792
 *  ----------------------------------------------------------
 */

//included for testing purposes
#include <exception>

//===========================================================================================
/**
 * This section shows how to write the basic test case.
 */
TEST_CASE( "TEST_CASE: Name and tags", "[hide][tutorial]" ) {
	//both name and tags are visible
	int a = 1, b = a;
	REQUIRE( a == b );
}

TEST_CASE( "TEST_CASE: Name, description and tags", "Description. [hide][tutorial]" ) {
	//both name and tags are visible, but NOT the description.
	int a = 1, b = a;
	REQUIRE( a == b );
}

//===========================================================================================
/**
 * This section shows what assertion macros exists.
 *
 * The REQUIRE family of macros tests an expression and aborts the test case if it fails.
 * The CHECK family are equivalent but execution continues in the same test case even if the assertion fails.
 * This is useful if you have a series of essentially orthoginal assertions and it is useful to see all the results
 * rather than stopping at the first failure.
 */
TEST_CASE( "ASSERTIONS: Natural expressions", "[hide][tutorial]" ) {
	//Evaluates the expression and records the result.
	//If an exception is thrown it is caught, reported, and counted as a failure.
	//These are the macros you will use most of the time
	int a = 1, b = a, c = 2;
	REQUIRE(a == b);
	CHECK(a == b);

	//Evaluates the expression and records the logical NOT of the result.
	//If an exception is thrown it is caught, reported, and counted as a failure.
	//(these forms exist as a workaround for the fact that ! prefixed expressions cannot be decomposed).
	REQUIRE_FALSE( a == c);
	CHECK_FALSE( a == c);
}

TEST_CASE( "ASSERTIONS: Exceptions", "[hide][tutorial]" ) {
	//Expects that an exception (of any type) is be thrown during evaluation of the expression.
	REQUIRE_THROWS(throw std::exception());
	CHECK_THROWS(throw std::exception());

	//Expects that an exception of the specified type is thrown during evaluation of the expression.
	REQUIRE_THROWS_AS(throw std::exception(), std::exception);
	CHECK_THROWS_AS(throw std::exception(), std::exception);

	//Expects that no exception is thrown during evaluation of the expression.
	int a = 1, b = a;
	REQUIRE_NOTHROW( a == b);
	CHECK_NOTHROW( a == b);
}

TEST_CASE( "ASSERTIONS: Matcher expressions: Equals", "[hide][tutorial]" ) {
	CHECK_THAT( "1", Equals("1"));
	REQUIRE_THAT( "1", Equals("1"));
}

TEST_CASE( "ASSERTIONS: Matcher expressions: Contains", "[hide][tutorial]" ) {
	CHECK_THAT( "onion", Contains("io"));
	REQUIRE_THAT( "onion", Contains("io"));
}

TEST_CASE( "ASSERTIONS: Matcher expressions: StartsWith", "[hide][tutorial]" ) {
	CHECK_THAT( "the start", StartsWith("the"));
	REQUIRE_THAT( "the start", StartsWith("the"));
}

TEST_CASE( "ASSERTIONS: Matcher expressions: EndsWith", "[hide][tutorial]" ) {
	CHECK_THAT( "the end", EndsWith("end"));
	REQUIRE_THAT( "the end", EndsWith("end"));
}

//===========================================================================================
/**
 * This section shows extra fucntionality
 */

TEST_CASE ("Float vs double precision", "[hide][tutorial]") {

	//Demonstrating the Approx class
	Approx approx(0.1);
	approx.epsilon(0.01); // precision can be specified

	float f_number = 0.1;
	double d_number = 0.1;
	CHECK_FALSE( f_number == d_number);
	CHECK( f_number == Approx(d_number));

}

//===========================================================================================
/**
 * This section shows what logging macros exists.
 * Messages can be logged during a test case.
 */

TEST_CASE( "LOGGING: INFO", "[hide][tutorial]" ) {
	std::string info = " logged.";
	INFO("The info is " << info);
}

TEST_CASE( "LOGGING: WARN", "[hide][tutorial]" ) {
	std::string warning = " not important at all.";
	WARN("The warning is " << warning);
}

TEST_CASE( "LOGGING: FAIL", "[hide][tutorial]" ) {
	//this will be recorded as a failure
	std::string failure = " not a failure at all, it is supposed to fail.";
	FAIL("The failure is " << failure);
}

TEST_CASE( "LOGGING: SCOPED_INFO", "[hide][tutorial]" ) {
	std::string scoped_info = " will only be logged if a test fails in the current scope.";
	SCOPED_INFO("The scoped info is " << scoped_info);
}

TEST_CASE( "LOGGING: CAPTURE", "[hide][tutorial]" ) {
	int theAnswere = 42;
	CAPTURE( theAnswere );
}

//===========================================================================================
/**
 * This section shows how to write Behavior-driven development styled tests.
 * http://en.wikipedia.org/wiki/Behavior-driven_development
 */

//BDD style testing
SCENARIO("BDD scenario", "Behavior-driven development scenario. [hide][tutorial][BDD]"){
	GIVEN("we create a new std::vector"){
		std::vector<int> vector;
		int i = 1;
		WHEN("we add a int to the vector"){
			vector.push_back(1);
			THEN("we can require that the vectors lenght is 1"){
				REQUIRE( vector.size() == 1);
			}
			AND_WHEN("we add another int to the vector"){
				vector.push_back(i);
				THEN("we can require that the vectors lenght is 2"){
					REQUIRE( vector.size() == 2);
				}
			}
			THEN("we can require that the vectors lenght is still 1"){
				REQUIRE( vector.size() == 1);
			}
		}
		THEN("we can require that the vector is empty"){
			REQUIRE(vector.empty());
		}
	}
}
