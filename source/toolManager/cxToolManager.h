#ifndef CXTOOLMANAGER_H_
#define CXTOOLMANAGER_H_

#include "sscToolManager.h"
#include "itkCommand.h"
#include "vtkSmartPointer.h"
#include "sscManualTool.h"
#include "sscLandmark.h"
#include "cxTool.h"
#include "cxTracker.h"
#include "sscDummyTool.h"
#include "vtkForwardDeclarations.h"
#include "cxManualToolAdapter.h"

class QDomNode;
class QDomDocument;
class QDomNodeList;
class QTimer;
class QFileInfo;

namespace cx
{
/**
 * \class ToolManager
 *
 * \brief Interface towards the navigation system.
 *
 * \date Nov 6, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class ToolManager: public ssc::ToolManager
{
Q_OBJECT

public:
  static void initializeObject();
  static ToolManager* getInstance();

  virtual bool isConfigured() const; ///< checks if the system is configured
  virtual bool isInitialized() const; ///< checks if the hardware is initialized
  virtual bool isTracking() const; ///< checks if the system is tracking

  virtual ssc::ToolManager::ToolMapPtr getConfiguredTools(); ///< get all configured, but not initialized tools
  virtual ssc::ToolManager::ToolMapPtr getInitializedTools(); ///< get all initialized tools
  virtual ssc::ToolManager::ToolMapPtr getTools(); ///< get all configured and initialized tools
  virtual ssc::ToolPtr getTool(const QString& uid); ///< get a specific tool

  virtual ssc::ToolPtr getDominantTool(); ///< get the dominant tool
  virtual void setDominantTool(const QString& uid); ///< can be set to either a connected or configured tool

  virtual std::map<QString, QString> getToolUidsAndNames() const; ///< both from configured and connected tools
  virtual std::vector<QString> getToolNames() const; ///< both from configured and connected tools
  virtual std::vector<QString> getToolUids() const; ///< both from configured and connected tools

  virtual ssc::Transform3DPtr get_rMpr() const; ///< get the patient registration transform
  virtual void set_rMpr(const ssc::Transform3DPtr& val); ///<  set the transform from patient to reference space

  virtual ssc::RegistrationHistoryPtr get_rMpr_History();

  virtual ssc::ToolPtr getReferenceTool() const; ///< get the tool that is used as a reference, if any

  virtual void savePositionHistory();
  virtual void loadPositionHistory();

  void setConfigurationFile(QString configurationFile); ///< Sets the configuration file to use, must be located in the resourcefolder \param configurationFile path to the configuration file to use
  void setLoggingFolder(QString loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved

  void addXml(QDomNode& parentNode); ///< write internal state to node
  void parseXml(QDomNode& dataNode);///< read internal state from node
  virtual void clear(); ///< clear everything loaded from xml

  ssc::ManualToolPtr getManualTool(); ///< a mouse-controllable virtual tool that is available even when not tracking.
  virtual ssc::LandmarkMap getLandmarks();
  virtual void setLandmark(ssc::Landmark landmark);
  virtual void removeLandmark(QString uid);
  virtual void removeLandmarks();

  virtual ssc::SessionToolHistoryMap getSessionHistory(double startTime, double stopTime);

  void runDummyTool(ssc::DummyToolPtr tool);
//  void setUSProbeSector(ssc::ProbeSector probeSector); ///< Set US probe sector on  the dominant tool (if it is a US probe)

  TrackerPtr getTracker();

public slots:
  void configure(); ///< sets up the software like the xml file suggests
  void initialize(); ///< connects to the hardware
  void uninitialize(); ///< disconnects from the hardware
  void startTracking(); ///< starts tracking
  void stopTracking(); ///< stops tracking
  void saveToolsSlot(); ///< saves transforms and timestamps
  void dominantCheckSlot(); ///< checks if the visible tool is going to be set as dominant tool

protected slots:
  void checkTimeoutsAndRequestTransform(); ///< checks for igstk timeouts and requests transform to the patient reference if needed
  void trackerOpenSlot(bool);
  void trackerInitializedSlot(bool);
  void trackerTrackingSlot(bool);
  void toolInitialized(bool);
  void updateReferenceTransformSlot();

protected:
  typedef ssc::ToolManager::ToolMap::iterator ToolMapIter;

  ToolManager(); ///< use getInstance instead
  virtual ~ToolManager(); ///< destructor

  void addInitializedTool(QString uid); ///< moves a tool from configuredTools to initializedTools
  void initializeManualTool();
  void configureReferences(); ///< specifies a tools as the reference

  QString mConfigurationFilePath; ///< path to the configuration file
  QString mLoggingFolder; ///< path to where logging should be saved
  QTimer* mTimer; ///< timer controlling the demand of transforms

  TrackerPtr mTracker; ///< the tracker to use
  ssc::ToolManager::ToolMapPtr mConfiguredTools; ///< all configured, but not connected, tools
  ssc::ToolManager::ToolMapPtr mInitializedTools; ///< all initialized tools
  ssc::ToolPtr mDominantTool; ///< the tool with highest priority
  ssc::ToolPtr mReferenceTool; ///< the tool which is used as patient reference tool
  ManualToolAdapterPtr mManualTool; ///< a mouse-controllable virtual tool that is available even when not tracking.

  ssc::RegistrationHistoryPtr m_rMpr_History; ///< transform from the patient reference to the reference, along with historical data.

  bool mConfigured; ///< whether or not the system is configured
  bool mInitialized; ///< whether or not the system is initialized
  bool mTracking; ///< whether or not the system is tracking

  igstk::PulseGenerator::Pointer mPulseGenerator;

  ssc::LandmarkMap mLandmarks; ///< in space patient reference.
  double mLastLoadPositionHistory;

private:
  ToolManager(ToolManager const&);
  ToolManager& operator=(ToolManager const&);

#ifndef _WINDOWS
  void createSymlink();
  QFileInfo getSymlink() const;
  void cleanupSymlink();
#endif //_WINDOWS
};

bool toolTypeSort(const ssc::ToolPtr tool1, const ssc::ToolPtr tool2); ///< function for sorting tools by type

}//namespace cx
#endif /* CXTOOLMANAGER_H_ */
