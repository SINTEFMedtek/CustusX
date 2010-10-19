#include "cxTest_Tracker.h"

#include <QDir>

void TestTracker::setUp()
{
  mValidPolarisInternal.mType = cx::Tracker::TRACKER_POLARIS;
  mValidPolarisInternal.mLoggingFolderName = QDir::currentPath();

  mValidAuroraInternal.mType = cx::Tracker::TRACKER_AURORA;
  mValidAuroraInternal.mLoggingFolderName = QDir::currentPath();
}

void TestTracker::tearDown()
{
}

void TestTracker::testConstructor()
{
  cx::Tracker::InternalStructure defaultInternalStructure;
  cx::Tracker trackerWithDefaultInternalStructure(defaultInternalStructure);
  CPPUNIT_ASSERT_MESSAGE("Tracker with default internal is valid when it shouldn't be.", !trackerWithDefaultInternalStructure.isValid());

  cx::Tracker polarisTrackerWithValidInternal(mValidPolarisInternal);
  CPPUNIT_ASSERT_MESSAGE("Polaris tracker is not valid.", polarisTrackerWithValidInternal.isValid());

  cx::Tracker auroraTrackerWithValidInternal(mValidAuroraInternal);
  CPPUNIT_ASSERT_MESSAGE("Aurora tracker is not valid.", auroraTrackerWithValidInternal.isValid());
}
