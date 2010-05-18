/*
 *  sscReconstructer.h
 *  Created by Ole Vegard Solberg on 5/4/10.
 *
 */

#ifndef SSCRECONSTRUCTER_H_
#define SSCRECONSTRUCTER_H_

#include <QObject>
#include "sscReconstructAlgorithm.h"
#include "sscThunderVNNReconstructAlgorithm.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace ssc
{
  
typedef boost::shared_ptr<class Reconstructer> ReconstructerPtr;
class Reconstructer : public QObject
{
  Q_OBJECT
public:
  Reconstructer();
  ImagePtr reconstruct(QString mhdFileName, QString calFileName);
  ImagePtr getOutput();

  long getMaxOutputVolumeSize() const;
  void setMaxOutputVolumeSize(long val);

private:
  ImagePtr mUsRaw;///<All imported US data framed packed into one image
  QString changeExtension(QString name, QString ext);
  std::vector<TimedPosition> mFrames;
  std::vector<TimedPosition> mPositions;
  ImagePtr mOutput;///< Output image from reconstruction
  ImagePtr mMask;///< Clipping mask for the input data
  ReconstructAlgorithmPtr mAlgorithm;
  unsigned long mMaxVolumeSize;///< Max volume size in bytes for output volume
  void readFiles(QString mhdFileName);
  void readUsDataFile(QString mhdFileName);
  void readTimeStampsFile(QString fileName, std::vector<TimedPosition>* timedPos);
  void readPositionFile(QString posFile, bool alsoReadTimestamps);
  ImagePtr generateMask();
  ImagePtr readMaskFile(QString mhdFileName);
  vtkImageDataPtr generateVtkImageData(Vector3D dim, Vector3D spacing, const unsigned char initValue); 
  
  Transform3D interpolate(const Transform3D& a, const Transform3D& b, double t);
  Transform3D readTransformFromFile(QString fileName);
  void applyCalibration(const Transform3D& calibration);
  void calibrateTimeStamps();
  void calibrateTimeStamps(double timeOffset, double scale);
  void interpolatePositions();
  void calibrate(QString calFile);
  std::vector<ssc::Vector3D> generateInputRectangle();
  ImagePtr generateOutputVolume();
};

}//namespace
#endif //SSCRECONSTRUCTER_H_
