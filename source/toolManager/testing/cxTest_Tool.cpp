#include "cxTest_Tool.h"

#include "cxTracker.h"
#include "cxDataLocations.h"
#include <QDir>

void TestTool::setUp()
{
  QString toolToolPath(cx::DataLocations::getRootConfigPath()+QString("/tool/Tools/"));

  mValidPolarisInternal.mType = ssc::Tool::TOOL_POINTER;
  mValidPolarisInternal.mName = "Polaris pointer name";
  mValidPolarisInternal.mUid = "Polaris pointer uid";
  mValidPolarisInternal.mTrackerType = cx::Tracker::TRACKER_POLARIS;
  mValidPolarisInternal.mSROMFilename = (toolToolPath+QString("07-198-0838_SW_Pointer_02.rom")).toStdString();
  //mValidPolarisInternal.mPortNumber; //not needed for polaris, default value should be enough
  //mValidPolarisInternal.mChannelNumber; //not needed for polaris, default value should be enough
  mValidPolarisInternal.mWireless = true; //only support wireless polaris
  //mValidPolarisInternal.m5DOF; //not needed for polaris, default value should be enough
  mValidPolarisInternal.mCalibrationFilename = (toolToolPath+QString("07-198-0838_SW_Pointer_02.cal")).toStdString();
  mValidPolarisInternal.mGraphicsFileName = (toolToolPath+QString("07-198-0838_SW_Pointer_02.stl")).toStdString();
  mValidPolarisInternal.mTransformSaveFileName = QDir::currentPath().toStdString();
  mValidPolarisInternal.mLoggingFolderName = QDir::currentPath().toStdString();

  mValidAuroraInternal.mType = ssc::Tool::TOOL_POINTER;
  mValidAuroraInternal.mName = "Aurora pointer name";
  mValidAuroraInternal.mUid = "Aurora pointer uid";
  mValidAuroraInternal.mTrackerType = cx::Tracker::TRACKER_AURORA;
  //mValidAuroraInternal.mSROMFilename; //not needed for aurora, default value should be enough
  mValidAuroraInternal.mPortNumber = 0; //0 is the same as port 1 on the physical aurora hw
  mValidAuroraInternal.mChannelNumber = 0; //only needed for 5DOF
  //mValidAuroraInternal.mWireless; //not needed for aurora, default value should be enough
  mValidAuroraInternal.m5DOF = true;
  mValidAuroraInternal.mCalibrationFilename = (toolToolPath+QString("probeCalibrationTransform_hypofyse_comp.cal")).toStdString();
  //mValidAuroraInternal.mGraphicsFileName; //not needed, and we dont have a stl for a aurora tool atm
  mValidAuroraInternal.mTransformSaveFileName = QDir::currentPath().toStdString();
  mValidAuroraInternal.mLoggingFolderName = QDir::currentPath().toStdString();
}

void TestTool::tearDown()
{
}

void TestTool::testConstructor()
{
  this->setUp();

  cx::Tool::InternalStructure defaultInternal;
  cx::Tool toolWithDefaultInternal(defaultInternal);
  CPPUNIT_ASSERT_MESSAGE("Default internal tool is valid, it should not be.", !toolWithDefaultInternal.isValid());

  cx::Tool polarisToolWithValidInternal(mValidPolarisInternal);
  CPPUNIT_ASSERT_MESSAGE("Polaris tool is not valid with a valid internal.", polarisToolWithValidInternal.isValid());

  cx::Tool auroraToolWithValidInternal(mValidAuroraInternal);
  CPPUNIT_ASSERT_MESSAGE("Aurora tool is not valid with a valid internal.", auroraToolWithValidInternal.isValid());
}
