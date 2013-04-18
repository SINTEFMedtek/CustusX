#define CATCH_CONFIG_MAIN  // This tell CATCH to provide a main() - only do this in one cpp file

#include "catch.hpp"

/**
 * This file describes how to use CATCH.
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
TEST_CASE( "TEST_CASE: Name and tags", "[tutorial]" ) {
	//both name and tags are visible
	REQUIRE( 1 == 1 );
}

TEST_CASE( "TEST_CASE: Name, description and tags", "Description. [tutorial]" ) {
	//both name and tags are visible, but NOT the description.
	REQUIRE( 1 == 1 );
}

//===========================================================================================
/**
 * This section shows what assertion macros exists.
 */
TEST_CASE( "ASSERTIONS: Natural expressions", "[tutorial]" ) {
	//Evaluates the expression and records the result.
	//If an exception is thrown it is caught, reported, and counted as a failure.
	//These are the macros you will use most of the time
	REQUIRE(1 == 1);
	CHECK(1 == 1);

	//Evaluates the expression and records the logical NOT of the result.
	//If an exception is thrown it is caught, reported, and counted as a failure.
	//(these forms exist as a workaround for the fact that ! prefixed expressions cannot be decomposed).
	REQUIRE_FALSE( 1 == 2);
	CHECK_FALSE( 1 == 2);
}

TEST_CASE( "ASSERTIONS: Exceptions", "[tutorial]" ) {
	//Expects that an exception (of any type) is be thrown during evaluation of the expression.
	REQUIRE_THROWS(throw std::exception());
	CHECK_THROWS(throw std::exception());

	//Expects that an exception of the specified type is thrown during evaluation of the expression.
	REQUIRE_THROWS_AS(throw std::exception(), std::exception);
	CHECK_THROWS_AS(throw std::exception(), std::exception);

	//Expects that no exception is thrown during evaluation of the expression.
	REQUIRE_NOTHROW( 1 == 1);
	CHECK_NOTHROW( 1 == 1);
}

TEST_CASE( "ASSERTIONS: Matcher expressions", "[tutorial]" ) {
//	REQUIRE_THAT( lhs, matcher call);
//	CHECK_THAT( lhs, matcher call );
}

//===========================================================================================
/**
 * This section shows what logginng macros exists.
 */
//TEST_CASE( "LOGGING: ", "[tutorial]" ) {
//
//}
//
//TEST_CASE( "LOGGING: ", "[tutorial]" ) {
//
//}
//
//TEST_CASE( "LOGGING: ", "[tutorial]" ) {
//
//}

//===========================================================================================
/**
 * This section shows how to write Behavior-driven development styled tests.
 * http://en.wikipedia.org/wiki/Behavior-driven_development
 */

//BDD style testing
SCENARIO("BDD scenario", "Behavior-driven development scenario. [tutorial][BDD]"){
	GIVEN("something is given"){
		WHEN("something happens"){
			//operations
			THEN("we can require something"){
				//require
				REQUIRE(1 == 1);
			}
			AND_WHEN("something else happens"){
				//operations
				THEN("we can require something else"){
					//require
				}
			}
		}
	}
}
