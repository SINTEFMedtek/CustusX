#include "cxTest_Tracker.h"

#include <QDir>

void TestTracker::setUp()
{
  mValidPolarisInternal.mType = ssc::tsPOLARIS;
  mValidPolarisInternal.mLoggingFolderName = QDir::currentPath();

  mValidAuroraInternal.mType = ssc::tsAURORA;
  mValidAuroraInternal.mLoggingFolderName = QDir::currentPath();
}

void TestTracker::tearDown()
{
}

void TestTracker::testConstructor()
{
  cx::IgstkTracker::InternalStructure defaultInternalStructure;
  cx::IgstkTracker trackerWithDefaultInternalStructure(defaultInternalStructure);
  CPPUNIT_ASSERT_MESSAGE("Tracker with default internal is valid when it shouldn't be.", !trackerWithDefaultInternalStructure.isValid());

  cx::IgstkTracker polarisTrackerWithValidInternal(mValidPolarisInternal);
  CPPUNIT_ASSERT_MESSAGE("Polaris tracker is not valid.", polarisTrackerWithValidInternal.isValid());

  cx::IgstkTracker auroraTrackerWithValidInternal(mValidAuroraInternal);
  CPPUNIT_ASSERT_MESSAGE("Aurora tracker is not valid.", auroraTrackerWithValidInternal.isValid());
}
