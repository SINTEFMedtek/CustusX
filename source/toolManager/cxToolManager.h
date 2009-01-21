#ifndef CXTOOLMANAGER_H_
#define CXTOOLMANAGER_H_

#include "sscToolManager.h"

#include "itkCommand.h"

#include "cxTool.h"
#include "cxTracker.h"

/**
 * cxToolManager.h
 *
 * \brief
 *
 * \date Nov 6, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class QDomNode;
class QDomNodeList;
class QTimer;

namespace cx
{
class ToolManager: public ssc::ToolManager
{
  Q_OBJECT

public:
  typedef Tracker::Message TrackerMessage;
  typedef Tool::Message ToolMessage;
  typedef std::string stdString;

  static ToolManager* getInstance();

  virtual bool isConfigured() const;
  virtual bool isInitialized() const;
  virtual bool isTracking() const;

  virtual ssc::ToolManager::ToolMapPtr getConfiguredTools();
  virtual ssc::ToolManager::ToolMapPtr getTools();
  virtual ssc::ToolPtr getTool(const std::string& uid);

  virtual ssc::ToolPtr getDominantTool();
  virtual void setDominantTool(const std::string& uid); ///< can be set to either a connected or configured tool

  virtual std::map<std::string, std::string> getToolUidsAndNames() const; ///< both from configured and connected tools
  virtual std::vector<std::string> getToolNames() const; ///< both from configured and connected tools
  virtual std::vector<std::string> getToolUids() const; ///< both from configured and connected tools

  virtual ssc::Transform3DPtr get_rMpr() const;
  virtual void set_rMpr(const ssc::Transform3DPtr& val);
  virtual ssc::ToolPtr getReferenceTool() const;
  virtual void saveTransformsAndTimestamps(std::string filePathAndName = "");

  void setConfigurationFile(std::string configurationFile); ///< Sets the configuration file to use, must be located in the resourcefolder \param configurationFile path to the configuration file to use
  void setLoggingFolder(std::string loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved

public slots:
  virtual void configure();
  virtual void initialize();
  virtual void startTracking();
  virtual void stopTracking();

signals:
  void toolManagerReport(std::string message); ///< sends out messages the outside might want to log

protected slots:
  /**
   * Slot that receives reports from tools
   * \param message What happended to the tool
   * \param state   Whether the tool was trying to enter or leave a state
   * \param success Whether or not the request was a success
   * \param uid     The tools unique id
   */
  void receiveToolReport(ToolMessage message, bool state, bool success, stdString uid);
  /**
   * Slot that receives reports from trackers
   * \param message What happended to the tool
   * \param state   Whether the tool was trying to enter or leave a state
   * \param success Whether or not the request was a success
   * \param uid     The tools unique id
   */
  void receiveTrackerReport(TrackerMessage message, bool state, bool success, stdString uid);
  void checkTimeoutsAndRequestTransform(); ///< checks for igstk timeouts and requests transform to the patient reference if needed

protected:
  typedef ssc::ToolManager::ToolMap::const_iterator ToolMapConstIter;

  ToolManager(); ///< use getInstance instead
  ~ToolManager(); ///< destructor

  bool pathsExists(); ///< checks that the needed paths actually exits
  /**
   * reads the configuration file and extracts tracker and tool nodes
   * \param[out] trackerNode  a container for the tracker info
   * \param[out] toolNodeList a container for the tool info
   * \return whether or not the configuration file could be read successfully
   */
  bool readConfigurationFile(QDomNodeList& trackerNode, QList<QDomNodeList>& toolNodeList);
  /**
   * configures a tracker according to the config file
   * \param[in] trackerNodeList
   * \return a pointer to the created tracker
   */
  TrackerPtr configureTracker(QDomNodeList& trackerNodeList);
  /**
   * configures tools according to the config file
   * \param[in] toolNodeList
   * \return a pointer to a map containing all the created tools
   */
  ssc::ToolManager::ToolMapPtr configureTools(QList<QDomNodeList>& toolNodeList);
  void addConnectedTool(std::string uid); ///< moves a tool from configuredTools to connectedTools
  void connectSignalsAndSlots(); ///< connects signals and slots

  static ToolManager* mCxInstance;

  std::string mConfigurationFilePath; ///< path to the configuration file
  std::string mLoggingFolder;         ///< path to where logging should be saved
  QTimer* mTimer;                     ///< timer controlling the demand of transforms

  TrackerPtr                    mTracker;         ///< the tracker to use
  ssc::ToolManager::ToolMapPtr  mConfiguredTools; ///< all configured, but not connected, tools
  ssc::ToolManager::ToolMapPtr  mConnectedTools;  ///< all connected tools
  ssc::ToolPtr                  mDominantTool;    ///< the tool with highest priority
  ssc::ToolPtr                  mReferenceTool;   ///< the tool which is used as patient reference tool

  ssc::Transform3DPtr m_rMpr; ///< the transform from the patient reference to the reference

  bool mConfigured;   ///< whether or not the system is configured
  bool mInitialized;  ///< whether or not the system is initialized
  bool mTracking;   ///< whether or not the system is tracking

  const std::string mTrackerTag, mTrackerTypeTag, mToolfileTag, mToolTag,
                    mToolTypeTag, mToolIdTag, mToolNameTag,
                    mToolGeoFileTag, mToolSensorTag, mToolSensorTypeTag,
                    mToolSensorWirelessTag, mToolSensorDOFTag, mToolSensorPortnumberTag,
                    mToolSensorChannelnumberTag, mToolSensorRomFileTag,
                    mToolCalibrationTag, mToolCalibrationFileTag;
                    ///< names of necessary tags in the configuration file

  igstk::PulseGenerator::Pointer mPulseGenerator;
};
}//namespace cx
#endif /* CXTOOLMANAGER_H_ */
