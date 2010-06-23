#include "cxTest_UsReconstruction.h"

#include "sscReconstructer.h"

//#include "cxToolConfigurationParser.h"

void TestUsReconstruction::setUp()
{
  // this stuff will be performed just before all tests in this class
}

void TestUsReconstruction::tearDown()
{
  // this stuff will be performed just after all tests in this class
}

void TestUsReconstruction::testConstructor()
{
  ssc::ReconstructerPtr reconstructer(new ssc::Reconstructer("",""));
}
