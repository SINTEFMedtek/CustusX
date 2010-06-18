#include "cxTest_ToolConfigurationParser.h"

#include "cxToolConfigurationParser.h"

void TestToolConfigurationParser::setUp()
{
  // this stuff will be performed just before all tests in this class
}

void TestToolConfigurationParser::tearDown()
{
  // this stuff will be performed just after all tests in this class
}

void TestToolConfigurationParser::testConstructor()
{
  std::string xmlFilePath = "../../../modules/xmlFileGenerator/ProbeCalibConfigs.xml";
  cx::ToolConfigurationParser configurationParser(xmlFilePath);
}
