/*
 *  sscReconstructer.h
 *  Created by Ole Vegard Solberg on 5/4/10.
 *
 */

#ifndef SSCRECONSTRUCTER_H_
#define SSCRECONSTRUCTER_H_

#include <QObject>
#include <QThread>
#include <math.h>
#include "sscReconstructAlgorithm.h"
#include "sscBoundingBox3D.h"
#include "sscReconstructedOutputVolumeParams.h"
//#include "probeXmlConfigParser.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"
#include "sscXmlOptionItem.h"
#include "sscProbeData.h"
//#include "sscStringWidgets.h"
#include "cxUsReconstructionFileReader.h"

namespace ssc
{
//class StringOptionItem;

typedef boost::shared_ptr<class Reconstructer> ReconstructerPtr;
/**
 * Used coordinate systems:
 * u  = raw input Ultrasound frames (in x, y. Origin lower left.)
 * t  = Tool space for probe as defined in ssc:Tool (z in ray direction, y to the left)
 * s  = probe localizer Sensor.
 * pr = Patient Reference localizer sensor.
 * d  = Output Data space
 *
 */
class Reconstructer : public QObject
{
  Q_OBJECT
public:
  friend class ThreadedReconstructer;

  Reconstructer(XmlOptionFile settings, QString shaderPath);
  virtual ~Reconstructer();

  void selectData(QString filename, QString calFilesPath="");
  void reconstruct(); // assumes readFiles has already been called
//  bool validReconstructData() const;
  void clearAll();
  QString getSelectedData() const { return mFilename; }

  ImagePtr getOutput();
  //ImagePtr getInput();
  XmlOptionFile getSettings() const { return mSettings; }
  StringDataAdapterXmlPtr mOrientationAdapter;
  StringDataAdapterXmlPtr mAlgorithmAdapter;
  std::vector<DataAdapterPtr> mAlgoOptions;
  StringDataAdapterXmlPtr mMaskReduce;//Reduce mask size in % in each direction
  BoolDataAdapterXmlPtr mAlignTimestamps; ///align track and frame timestamps to each other automatically
  DoubleDataAdapterXmlPtr mTimeCalibration; ///set a offset in the frame timestamps

  OutputVolumeParams getOutputVolumeParams() const;
  void setOutputVolumeParams(const OutputVolumeParams& par);
  void setOutputRelativePath(QString path);
  void setOutputBasePath(QString path);

public slots:
  void setSettings();

signals:
  void paramsChanged();
  void inputDataSelected(QString mhdFileName);
  void reconstructFinished();

private:
  //ImagePtr mUsRaw;///<All imported US data framed packed into one image
  USFrameDataPtr mUsRaw;///<All imported US data frames with pointers to each frame
  std::vector<TimedPosition> mFrames;
  std::vector<TimedPosition> mPositions;
  ImagePtr mMask;///< Clipping mask for the input data

  OutputVolumeParams mOutputVolumeParams;
  ReconstructAlgorithmPtr mAlgorithm;
  ssc::ProbeData mProbeData;
  XmlOptionFile mSettings;
  QString mLastAppliedOrientation; ///< the orientation algorithm used for the currently calculated extent.
  QString mCalFileName; ///< Name of calibration file
  QString mCalFilesPath; ///< Path to calibration files
  QString mFilename; ///< filename used for current data read
//  QString mCalFilename; /// filename used for current cal read
  ImagePtr mOutput;///< Output image from reconstruction
  QString mOutputRelativePath;///< Relative path to the output image
  QString mOutputBasePath;///< Global path where the relative path starts, for the output image
  QString mShaderPath; ///< name of shader folder
  QString mLastAppliedMaskReduce;///< The last used mask reduction
  
  double mMaxTimeDiff; ///< The largest allowed time deviation for the positions used in the frame interpolations

  cx::UsReconstructionFileReaderPtr mFileReader;
  void readFiles(QString mhdFileName, QString calFilesPath);
//  void reconstruct(QString mhdFileName, QString calFilesPath); // do everything
  ImagePtr createMaskFromConfigParams();
  ImagePtr generateMask();
  ssc::Transform3D applyOutputOrientation();
  void findExtentAndOutputTransform();

  Transform3D interpolate(const Transform3D& a, const Transform3D& b, double t);
  void alignTimeSeries();
  void applyTimeCalibration();
  void calibrateTimeStamps(double timeOffset, double scale);
  void interpolatePositions();
  void calibrate(QString calFilesPath);
  std::vector<ssc::Vector3D> generateInputRectangle();
  ImagePtr generateOutputVolume();
  //StringOptionItem getNamedSetting(const QString& uid);
  void clearOutput();
  //void saveSettings();
  void createAlgorithm();

  QString generateOutputUid();
  QString generateImageName(QString uid) const;

  void threadedPreReconstruct();
  void threadedReconstruct();
  void threadedPostReconstruct();

  bool validInputData() const;///< checks if internal states is valid (that it actually has frames to reconstruct)
};

/**Execution of a reconstruction in another thread.
 * The class replaces the Reconstructer::reconstruct() method.
 *
 */
class ThreadedReconstructer : public QThread
{
  Q_OBJECT

public:
  ThreadedReconstructer(ReconstructerPtr reconstructer);
  virtual void run();
private slots:
  void postReconstructionSlot();
private:
  ReconstructerPtr mReconstructer;
};
typedef boost::shared_ptr<class ThreadedReconstructer> ThreadedReconstructerPtr;

}//namespace
#endif //SSCRECONSTRUCTER_H_
