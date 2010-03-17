#ifndef CXTOOLMANAGER_H_
#define CXTOOLMANAGER_H_

#include "sscToolManager.h"
#include "itkCommand.h"
#include "vtkSmartPointer.h"
#include "cxTool.h"
#include "cxTracker.h"

class QDomNode;
class QDomDocument;
class QDomNodeList;
class QTimer;
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

  static ToolManager* getInstance();

  virtual bool isConfigured() const; ///< checks if the system is configured
  virtual bool isInitialized() const; ///< checks if the hardware is initialized
  virtual bool isTracking() const; ///< checks if the system is tracking

  virtual ssc::ToolManager::ToolMapPtr getConfiguredTools(); ///< get all configured, but not connected tools
  virtual ssc::ToolManager::ToolMapPtr getTools(); ///< get all connected tools
  virtual ssc::ToolPtr getTool(const std::string& uid); ///< get a specific tool

  virtual ssc::ToolPtr getDominantTool(); ///< get the dominant tool
  virtual void setDominantTool(const std::string& uid); ///< can be set to either a connected or configured tool

  virtual std::map<std::string, std::string> getToolUidsAndNames() const; ///< both from configured and connected tools
  virtual std::vector<std::string> getToolNames() const; ///< both from configured and connected tools
  virtual std::vector<std::string> getToolUids() const; ///< both from configured and connected tools

  virtual ssc::Transform3DPtr get_rMpr() const; ///< get the patient ...
  virtual void set_rMpr(const ssc::Transform3DPtr& val); ///<  set the transform from patient to reference space

  virtual ssc::RegistrationHistoryPtr get_rMpr_History();
  //virtual void set_rMpr(const CalibrationTransform& val); ///<  set the patient-reference transform, along with calibration info.
  //virtual void set_rMpr_ActiveTime(const QDateTime& time); ///< roll the calibrationtime back to a specified point, use invalid DateTime for real time.
  //virtual TransformEvent get_rMprEvent() const;
  //virtual std::vector<TransformEvent> get_rMpr_History() const; ///< get all calibration events

  virtual ssc::ToolPtr getReferenceTool() const; ///< get the tool that is used as a reference, if any
  virtual void saveTransformsAndTimestamps(std::string filePathAndName = ""); ///<

  void setConfigurationFile(std::string configurationFile); ///< Sets the configuration file to use, must be located in the resourcefolder \param configurationFile path to the configuration file to use
  void setLoggingFolder(std::string loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved

  virtual vtkDoubleArrayPtr getToolSamples(); ///< \return all toolsamples defined .

  void addXml(QDomNode& parentNode); ///< write internal state to node
  void parseXml(QDomNode& dataNode);///< read internal state from node

public slots:
  virtual void configure(); ///< sets up the software like the xml file suggests
  virtual void initialize(); ///< connects to the hardware
  virtual void startTracking(); ///< starts tracking
  virtual void stopTracking(); ///< stops tracking
  virtual void saveToolsSlot(); ///< saves transforms and timestamps

signals:
  //void toolManagerReport(std::string message); ///< sends out messages the outside might want to log
  void toolSampleRemoved(double x, double y, double z, unsigned int index); ///< emitted when a tool(patient) coordinate is removed
  void toolSampleAdded(double x, double y, double z, unsigned int index); ///< emitted when a tool(patient) coordinate is added
  void rMprChanged(); ///< emitted when the transformation between patient reference and (data) reference is set

public slots:
  void addToolSampleSlot(double x, double y, double z, unsigned int index); ///< slot to remove tool(patient) samples
  void removeToolSampleSlot(double x, double y, double z, unsigned int index); ///< slot to add tool(patient) samples

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

  ssc::RegistrationHistoryPtr m_rMpr_History; ///< transform from the patient reference to the reference, along with historical data.

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

  /**
   * ToolSamples are in patient reference space.
   */
  vtkDoubleArrayPtr mToolSamples; ///< array consists of 4 components (<x,y,z,index>) for each tuple (landmark)

private:
  ToolManager(ToolManager const&);
  ToolManager& operator=(ToolManager const&);
};
}//namespace cx
#endif /* CXTOOLMANAGER_H_ */
