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
#include "sscBoundingBox3D.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace ssc
{
  
typedef boost::shared_ptr<class Reconstructer> ReconstructerPtr;
class Reconstructer : public QObject
{
  Q_OBJECT
public:
  Reconstructer();

  void readFiles(QString mhdFileName, QString calFileName);
  void reconstruct(); // assumes readFiles has already been called

  ImagePtr reconstruct(QString mhdFileName, QString calFileName); // do everything
  ImagePtr getOutput();

  long getMaxOutputVolumeSize() const;
  void setMaxOutputVolumeSize(long val);
  ssc::DoubleBoundingBox3D getExtent() const; ///< extent of volume on output space

private:
  ImagePtr mUsRaw;///<All imported US data framed packed into one image
  std::vector<TimedPosition> mFrames;
  std::vector<TimedPosition> mPositions;
  ssc::DoubleBoundingBox3D mExtent; ///< extent of volume on output space
  ImagePtr mOutput;///< Output image from reconstruction
  ImagePtr mMask;///< Clipping mask for the input data
  ReconstructAlgorithmPtr mAlgorithm;
  unsigned long mMaxVolumeSize;///< Max volume size in bytes for output volume

  void readUsDataFile(QString mhdFileName);
  void readTimeStampsFile(QString fileName, std::vector<TimedPosition>* timedPos);
  void readPositionFile(QString posFile, bool alsoReadTimestamps);
  ImagePtr generateMask();
  ImagePtr readMaskFile(QString mhdFileName);
  vtkImageDataPtr generateVtkImageData(Vector3D dim, Vector3D spacing, const unsigned char initValue); 
  void findExtentAndOutputTransform();

  QString changeExtension(QString name, QString ext);
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
