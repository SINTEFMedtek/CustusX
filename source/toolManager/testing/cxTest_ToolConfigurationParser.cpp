#include "cxTest_ToolConfigurationParser.h"

#include "cxDataLocations.h"
#include "cxToolConfigurationParser.h"
#include <QFile>

void TestToolConfigurationParser::setUp()
{
  mXmlFilePath = cx::DataLocations::getRootConfigPath()+QString("/tool/Lab/POLARIS_07-198-0838_SW_Pointer_02_AND_02-206-00913_SW_PasRef_01.xml");

  QString xmlFilePath = mXmlFilePath;
  mConfigurationParser = new cx::ToolConfigurationParser(xmlFilePath);
}

void TestToolConfigurationParser::tearDown()
{
  delete mConfigurationParser;
}

void TestToolConfigurationParser::testConstructor()
{
  CPPUNIT_ASSERT_MESSAGE("Xml file does not exist.", QFile::exists(mXmlFilePath));
  CPPUNIT_ASSERT_MESSAGE("Could not make a new configuration parser.", mConfigurationParser);
}

void TestToolConfigurationParser::testGetTracker()
{
  cx::IgstkTracker::InternalStructure tracker = mConfigurationParser->getTracker();
  //CPPUNIT_ASSERT_MESSAGE("Tracker is null.", tracker);
  CPPUNIT_ASSERT_MESSAGE("Tracker is not of type Polaris.", tracker.mType == ssc::tsPOLARIS);
}

void TestToolConfigurationParser::testGetConfiguredTools()
{
  std::vector<cx::Tool::InternalStructure> tools = mConfigurationParser->getConfiguredTools();
  CPPUNIT_ASSERT_MESSAGE("Not the right number of tools.", tools.size() == 2);

  std::vector<cx::Tool::InternalStructure>::iterator it;
  for(it = tools.begin(); it != tools.end(); ++it)
  {
    //cx::ToolPtr cxTool = boost::shared_static_cast<cx::Tool>((*it).second);
    CPPUNIT_ASSERT_MESSAGE("Found an tool of type.", it->mType);
  }
}
