/*
 *  sscReconstructer.h
 *  Created by Ole Vegard Solberg on 5/4/10.
 *
 */

#ifndef SSCRECONSTRUCTER_H_
#define SSCRECONSTRUCTER_H_

#include <QObject>
#include <math.h>
#include "sscReconstructAlgorithm.h"
#include "sscThunderVNNReconstructAlgorithm.h"
#include "sscBoundingBox3D.h"
#include "sscReconstructedOutputVolumeParams.h"
#include "../../modules/ultrasoundAcquisition/source/gui/probeXmlConfigParser.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace ssc
{
  
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
  Reconstructer();

  void readFiles(QString mhdFileName, QString calFileName);
  void reconstruct(); // assumes readFiles has already been called

  ImagePtr reconstruct(QString mhdFileName, QString calFileName); // do everything
  ImagePtr getOutput();
  ImagePtr getInput();
  QDomElement getSettings() const;

  OutputVolumeParams getOutputVolumeParams() const;
  void setOutputVolumeParams(const OutputVolumeParams& par);
//  long getMaxOutputVolumeSize() const;
//  void setMaxOutputVolumeSize(long val);
//  ssc::DoubleBoundingBox3D getExtent() const; ///< extent of volume on output space

public slots:
  void setSettings();

signals:
  void paramsChanged();

private:
  ImagePtr mUsRaw;///<All imported US data framed packed into one image
  std::vector<TimedPosition> mFrames;
  std::vector<TimedPosition> mPositions;
//  ssc::DoubleBoundingBox3D mExtent; ///< extent of volume on output space
  OutputVolumeParams mOutputVolumeParams;
  ImagePtr mOutput;///< Output image from reconstruction
  ImagePtr mMask;///< Clipping mask for the input data
  ReconstructAlgorithmPtr mAlgorithm;
  ProbeXmlConfigParser::Configuration mConfiguration;
  QDomDocument mSettings;
  QString mLastAppliedOrientation; ///< the orientation algorithm used for the currently calculated extent.

  void readUsDataFile(QString mhdFileName);
  void readTimeStampsFile(QString fileName, std::vector<TimedPosition>* timedPos);
  void readPositionFile(QString posFile, bool alsoReadTimestamps);
  ImagePtr createMaskFromConfigParams();
  ImagePtr generateMask();
  ImagePtr readMaskFile(QString mhdFileName);
  vtkImageDataPtr generateVtkImageData(Vector3D dim, Vector3D spacing, const unsigned char initValue); 
  void applyOutputOrientation();
  void findExtentAndOutputTransform();

  QString changeExtension(QString name, QString ext);
  Transform3D interpolate(const Transform3D& a, const Transform3D& b, double t);
  Transform3D readTransformFromFile(QString fileName);
  void calibrateTimeStamps();
  void calibrateTimeStamps(double timeOffset, double scale);
  void interpolatePositions();
  void calibrate(QString calFile);
  std::vector<ssc::Vector3D> generateInputRectangle();
  ImagePtr generateOutputVolume();
  StringOptionItem getNamedSetting(const QString& uid);
  void clearAll();
  void clearOutput();
};

}//namespace
#endif //SSCRECONSTRUCTER_H_
