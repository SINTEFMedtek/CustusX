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
#include "sscProbeSector.h"
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
  friend class ThreadedReconstructer;

public:
  Reconstructer(XmlOptionFile settings, QString shaderPath);
  virtual ~Reconstructer();

  void selectData(QString filename, QString calFilesPath="");
  void reconstruct(); // assumes readFiles has already been called
  QString getSelectedData() const { return mFilename; }

  ImagePtr getOutput();
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
  struct FileData
  {
    USFrameDataPtr mUsRaw;///<All imported US data frames with pointers to each frame
    std::vector<TimedPosition> mFrames;
    std::vector<TimedPosition> mPositions;
    ImagePtr mMask;///< Clipping mask for the input data
    ssc::ProbeSector mProbeData;
  };
  FileData mFileData;
  FileData mOriginalFileData; ///< original version of loaded data. Use as basis when recalculating due to changed params.

  cx::UsReconstructionFileReaderPtr mFileReader;
  OutputVolumeParams mOutputVolumeParams;
  ReconstructAlgorithmPtr mAlgorithm;
  XmlOptionFile mSettings;
  QString mCalFileName; ///< Name of calibration file
  QString mCalFilesPath; ///< Path to calibration files
  QString mFilename; ///< filename used for current data read
  ImagePtr mOutput;///< Output image from reconstruction
  QString mOutputRelativePath;///< Relative path to the output image
  QString mOutputBasePath;///< Global path where the relative path starts, for the output image
  QString mShaderPath; ///< name of shader folder
  double mMaxTimeDiff; ///< The largest allowed time deviation for the positions used in the frame interpolations

  void readCoreFiles(QString fileName, QString calFilesPath);
  ImagePtr createMaskFromConfigParams();
  ImagePtr generateMask();
  ssc::Transform3D applyOutputOrientation();
  void findExtentAndOutputTransform();
  void transformPositionsTo_prMu();

  Transform3D interpolate(const Transform3D& a, const Transform3D& b, double t);
  void alignTimeSeries();
  void applyTimeCalibration();
  void calibrateTimeStamps(double timeOffset, double scale);
  void interpolatePositions();
//  void calibrate(QString calFilesPath);
  std::vector<ssc::Vector3D> generateInputRectangle();
  ImagePtr generateOutputVolume();
  void clearOutput();
  void createAlgorithm();
  void updateFromOriginalFileData();
  void clearAll();

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
