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

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace ssc
{

/** Helper struct for sending and controlling output volume properties.
 */
class OutputVolumeParams
{
public:
  // constants, set only based on input data
  ssc::DoubleBoundingBox3D mExtent;
  double mInputSpacing;
  ssc::Vector3D mInputDim;

  OutputVolumeParams() :
    mExtent(0,0,0,0,0,0),
    mInputSpacing(0),
    mInputDim(0,0,0),
    mMaxVolumeSize(0)
  {
  }
  /** Initialize the volue parameters with sensible defaults.
   */
  OutputVolumeParams(ssc::DoubleBoundingBox3D extent, double inputSpacing, ssc::Vector3D inputDim) :
    mExtent(extent),
    mInputSpacing(inputSpacing),
    mInputDim(inputDim),
    mMaxVolumeSize(1024*1024*32)
  {
    // Calculate optimal output image spacing and dimensions based on US frame spacing
    setSpacing(mInputSpacing);
  }

  unsigned long getVolumeSize() const
  {
    return mDim[0]*mDim[1]*mDim[2];;
  }

  /** Set a spacing, recalculate dimensions.
   */
  void setSpacing(double spacing)
  {
    mSpacing = spacing;
    mDim = mExtent.range() / mSpacing;
  }
  double getSpacing() const
  {
    return mSpacing;
  }
  /** Set one of the dimensions explicitly, recalculate other dims and spacing.
   */
  void setDim(int index, int val)
  {
    //mSpacing = mExtent.range()[index] / val;
    setSpacing(mExtent.range()[index] / val);
  }
  ssc::Vector3D getDim() const
  {
    return mDim;
  }
  /** Increase spacing in order to keep size below a max size
   */
  void constrainVolumeSize(double maxSize)
  {
    mMaxVolumeSize = maxSize;
    // Reduce output volume size if optimal volume size is too large
    unsigned long volumeSize = getVolumeSize();
    if (volumeSize > mMaxVolumeSize)
    {
      double scaleFactor = pow(volumeSize/double(mMaxVolumeSize),1/3.0);
      std::cout << "Downsampled volume - Used scaleFactor : " << scaleFactor << std::endl;
      mDim /= scaleFactor;
      mSpacing *= scaleFactor;
    }
  }
  unsigned long getMaxVolumeSize() const
  {
    return mMaxVolumeSize;
  }

private:
  // controllable data, set only using the setters
  unsigned long mMaxVolumeSize;
  double mSpacing;
  ssc::Vector3D mDim;
};
  
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
  OutputVolumeParams mOutputVolumeParams;
  ImagePtr mOutput;///< Output image from reconstruction
  ImagePtr mMask;///< Clipping mask for the input data
  ReconstructAlgorithmPtr mAlgorithm;

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
