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
 *
 */


TEST_CASE( "TEST_CASE: Name and tags", "[tutorial]" ) {
  REQUIRE( 1 == 1 );
}

TEST_CASE( "TEST_CASE: Name, description and tags", "Description. [tutorial]" ) {
  REQUIRE( 1 == 1 );
}

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
