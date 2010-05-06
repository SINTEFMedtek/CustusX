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
typedef boost::shared_ptr<class PositionData> PositionDataPtr;
/** Represents one 2D US frame along with timestamp and position
 */
class PositionData
{
public:
  double mTime;
  Transform3D mPos;
};
  
typedef boost::shared_ptr<class Reconstructer> ReconstructerPtr;
class Reconstructer : public QObject
{
  Q_OBJECT
public:
  Reconstructer();
  ImagePtr reconstruct(QString mhdFileName, QString calFileName);
  ImagePtr getOutput();
private:
  ImagePtr mUsRaw;///<All imported US data framed packed into one image
  QString changeExtension(QString name, QString ext);
  std::vector<UsFrame> mFrames;
  std::vector<PositionData> mPositions;
  ImagePtr mOutput;///< Output image from reconstruction
  ImagePtr mMask;///< Clipping mask for the input data
  ReconstructAlgorithmPtr mAlgorithm;
  void readFiles(QString mhdFileName);
  void readUsDataFile(QString mhdFileName);
  void readPositionFile(QString posFile);
  ImagePtr generateMask();
  vtkImageDataPtr generateVtkImageData(Vector3D dim, Vector3D spacing, const char initValue); 
  
  Transform3D interpolate(const Transform3D& a, const Transform3D& b, double t);
  Transform3D readTransformFromFile(QString fileName);
  void applyCalibration(const Transform3D& calibration);
  void calibrateTimeStamps();
  void calibrateTimeStamps(double timeOffset, double scale);
  void interpolatePositions();
  void calibrate(QString calFile);
  Transform3D find_prMd();
  ImagePtr generateOutputVolume();
};

}//namespace
#endif //SSCRECONSTRUCTER_H_