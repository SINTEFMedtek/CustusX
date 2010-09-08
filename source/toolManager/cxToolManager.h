#ifndef CXTOOLMANAGER_H_
#define CXTOOLMANAGER_H_

#include "sscToolManager.h"
#include "itkCommand.h"
#include "vtkSmartPointer.h"
#include "sscManualTool.h"
#include "sscLandmark.h"
#include "cxTool.h"
#include "cxTracker.h"

class QDomNode;
class QDomDocument;
class QDomNodeList;
class QTimer;
class QFileInfo;
typedef vtkSmartPointer<class vtkDoubleArray> vtkDoubleArrayPtr;

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
  typedef Tracker::Message TrackerMessage;
  typedef Tool::Message ToolMessage;
  typedef std::string stdString;

  static void initializeObject();
  static ToolManager* getInstance();

  virtual bool isConfigured() const; ///< checks if the system is configured
  virtual bool isInitialized() const; ///< checks if the hardware is initialized
  virtual bool isTracking() const; ///< checks if the system is tracking

  virtual ssc::ToolManager::ToolMapPtr getConfiguredTools(); ///< get all configured, but not initialized tools
  virtual ssc::ToolManager::ToolMapPtr getInitializedTools(); ///< get all initialized tools
  virtual ssc::ToolManager::ToolMapPtr getTools(); ///< get all configured and initialized tools
  virtual ssc::ToolPtr getTool(const std::string& uid); ///< get a specific tool

  virtual ssc::ToolPtr getDominantTool(); ///< get the dominant tool
  virtual void setDominantTool(const std::string& uid); ///< can be set to either a connected or configured tool

  virtual std::map<std::string, std::string> getToolUidsAndNames() const; ///< both from configured and connected tools
  virtual std::vector<std::string> getToolNames() const; ///< both from configured and connected tools
  virtual std::vector<std::string> getToolUids() const; ///< both from configured and connected tools

  virtual ssc::Transform3DPtr get_rMpr() const; ///< get the patient registration transform
  virtual void set_rMpr(const ssc::Transform3DPtr& val); ///<  set the transform from patient to reference space

  virtual ssc::RegistrationHistoryPtr get_rMpr_History();

  virtual ssc::ToolPtr getReferenceTool() const; ///< get the tool that is used as a reference, if any
  virtual void saveTransformsAndTimestamps(std::string filePathAndName = ""); ///<

  void setConfigurationFile(std::string configurationFile); ///< Sets the configuration file to use, must be located in the resourcefolder \param configurationFile path to the configuration file to use
  void setLoggingFolder(std::string loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved

  void addXml(QDomNode& parentNode); ///< write internal state to node
  void parseXml(QDomNode& dataNode);///< read internal state from node
  virtual void clear(); ///< clear everything loaded from xml

  ssc::ManualToolPtr getManualTool(); ///< a mouse-controllable virtual tool that is available even when not tracking.
  virtual ssc::LandmarkMap getLandmarks();
  virtual void setLandmark(ssc::Landmark landmark);
  virtual void removeLandmark(std::string uid);

  void setUSProbeSector(ssc::ProbeSector probeSector); ///< Set US probe sector on  the dominant tool (if it is a US probe)

  TrackerPtr getTracker();

public slots:
  virtual void configure(); ///< sets up the software like the xml file suggests
  virtual void initialize(); ///< connects to the hardware
  virtual void startTracking(); ///< starts tracking
  virtual void stopTracking(); ///< stops tracking
  virtual void saveToolsSlot(); ///< saves transforms and timestamps
  void dominantCheckSlot(); ///< checks if the visible tool is going to be set as dominant tool

protected slots:
  void receiveToolReport(ToolMessage message, bool state, bool success, stdString uid); ///< Slot that receives reports from tools
  void receiveTrackerReport(TrackerMessage message, bool state, bool success, stdString uid); ///< Slot that receives reports from trackers
  void checkTimeoutsAndRequestTransform(); ///< checks for igstk timeouts and requests transform to the patient reference if needed

protected:
  typedef ssc::ToolManager::ToolMap::const_iterator ToolMapConstIter;

  ToolManager(); ///< use getInstance instead
  ~ToolManager(); ///< destructor

  void addConnectedTool(std::string uid); ///< moves a tool from configuredTools to connectedTools
  void connectSignalsAndSlots(); ///< connects signals and slots
  void initializeManualTool();
  void configureReferences(); ///<

  std::string mConfigurationFilePath; ///< path to the configuration file
  std::string mLoggingFolder; ///< path to where logging should be saved
  QTimer* mTimer; ///< timer controlling the demand of transforms

  TrackerPtr mTracker; ///< the tracker to use
  ssc::ToolManager::ToolMapPtr mConfiguredTools; ///< all configured, but not connected, tools
  ssc::ToolManager::ToolMapPtr mInitializedTools; ///< all initialized tools
  ssc::ToolPtr mDominantTool; ///< the tool with highest priority
  ssc::ToolPtr mReferenceTool; ///< the tool which is used as patient reference tool
  ssc::ManualToolPtr mManualTool; ///< a mouse-controllable virtual tool that is available even when not tracking.

  ssc::RegistrationHistoryPtr m_rMpr_History; ///< transform from the patient reference to the reference, along with historical data.

  bool mConfigured; ///< whether or not the system is configured
  bool mInitialized; ///< whether or not the system is initialized
  bool mTracking; ///< whether or not the system is tracking

  igstk::PulseGenerator::Pointer mPulseGenerator;

  ssc::LandmarkMap mLandmarks; ///< in space patient reference.

private:
  ToolManager(ToolManager const&);
  ToolManager& operator=(ToolManager const&);

  void createSymlink();
  QFileInfo getSymlink() const;
  void cleanupSymlink();
};

bool toolTypeSort(const ssc::ToolPtr tool1, const ssc::ToolPtr tool2); ///< function for sorting tools by type

}//namespace cx
#endif /* CXTOOLMANAGER_H_ */
