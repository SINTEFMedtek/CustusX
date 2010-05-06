/*
 *  sscReconstructer.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */


#include "sscReconstructer.h"

#include <QtCore>
#include <vtkImageData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>
#include "sscDataManagerImpl.h"
#include "sscTypeConversions.h"

namespace ssc
{

QString Reconstructer::changeExtension(QString name, QString ext)
{
  QStringList splitName = name.split(".");
  splitName[splitName.size()-1] = ext;
  return splitName.join(".");
}
  
void Reconstructer::readFiles(QString mhdFileName)
{
  readUsDataFile(mhdFileName);
  
  QString posName = changeExtension(mhdFileName, "pos");
  readPositionFile(posName);
}

void Reconstructer::readUsDataFile(QString mhdFileName)
{
  mUsRaw = MetaImageReader().load(string_cast(mhdFileName), 
                                  string_cast(mhdFileName));
  
  
  QString timeName = changeExtension(mhdFileName, "tim");
  QFile timeFile(timeName);
  if(!timeFile.open(QIODevice::ReadOnly))
  {
    std::cout << "Can't open file: " << string_cast(timeName) << std::endl;
    return;
  }
  while (!timeFile.atEnd())
  {
    bool ok = true;
    UsFrame frame;
    QByteArray array = timeFile.readLine();
    frame.mTime = QString(array).toDouble(&ok);
    if (!ok)
    {
      std::cout << "Can't read double in file: " << string_cast(timeName) << std::endl;
      return;
    }
    mFrames.push_back(frame);
  }
  if(mUsRaw->getBaseVtkImageData()->GetDimensions()[2] != (int)mFrames.size())
  {
    mFrames.clear();
    mUsRaw.reset();
    std::cout << "Number of frames don't match number of positions" << std::endl;
    return;
  }
  std::cout << "Reconstructer::readUsDataFile() - succes. Number of frames: " 
    << mFrames.size() << std::endl;
  return;
}
void Reconstructer::readPositionFile(QString posFile)
{
  QFile file(posFile);
  if(!file.open(QIODevice::ReadOnly))
  {
    std::cout << "Can't open file: " << string_cast(posFile) << std::endl;
    return;
  }
  while (!file.atEnd())
  {
    bool ok = true;
    PositionData position;
    QByteArray array = file.readLine();
    position.mTime = QString(array).toDouble(&ok);
    if (!ok)
    {
      std::cout << "Can't read double in file: " << string_cast(posFile) 
        << std::endl;
      return;
    }
    
    QString positionString = file.readLine();
    positionString += " " + file.readLine();
    positionString += " " + file.readLine();
    positionString += " 0 0 0 1";
    position.mPos = Transform3D::fromString(positionString, &ok);
    if (!ok)
    { 
      std::cout << "Can't read position number: " << mPositions.size() 
        << " from file: " << string_cast(posFile) << std::endl;
      std::cout << "values: " << position.mPos[0][0] << std::endl;;
      return;
    }
    mPositions.push_back(position);
  }
  
  std::cout << "Reconstructer::readPositionFile() - succes. Number of positions: " 
  << mPositions.size() << std::endl;
  return;
}
  
/**
 * Apply time calibration function y = ax + b, where
 *  y = calibrated(new) position timestamp
 *  x = old position timestamp
 *  a = input scale
 *  b = input offset
 * \param offset Offset between images and positions. 
 * \param scale Scale between images and positions.
 */
void Reconstructer::calibrateTimeStamps(double offset, double scale)
{
  for (unsigned int i = 0; i < mPositions.size(); i++)
  {
    mPositions[i].mTime = scale * mPositions[i].mTime + offset;
    //std::cout << "postime: " << mPositions[i].mTime << std::endl;
  }
}

/**
 * Calculate timestamp calibration in an adhoc way, by assuming that 
 * images and positions start and stop at the exact same time
 */
void Reconstructer::calibrateTimeStamps()
{
  double scale = (mFrames.back().mTime - mFrames.front().mTime)
    / (mPositions.back().mTime - mPositions.front().mTime);
  
  double offset = mFrames.front().mTime - scale * mPositions.front().mTime;
  
  std::cout << "Reconstructor::calibrateTimeStamps() NB!!! generated offset: " 
    << offset << " scale: " << scale << std::endl;
  calibrateTimeStamps(offset, scale);
}

/**
 * Linear interpolation between a and b. t = 1 means use only b;
 */
Transform3D Reconstructer::interpolate(const Transform3D& a, 
                                       const Transform3D& b,
                                       double t)
{
  Transform3D c;
  for (int i = 0; i < 4; i++)
    for (int j= 0; j < 4; j++)
      c[i][j] = (1-t)*a[i][j] + t*b[i][j];
  return c;
}
  
/**
 * Find interpolated position values for each frame based on the input position
 * data.
 * Current implementation: 
 * Linear interpolation
 */
void Reconstructer::interpolatePositions()
{
  //TODO: Check if the affine transforms still are affine after the linear interpolation 
  
  double scale = mPositions.size() / (double)mFrames.size();
  for(unsigned int i_frame = 0; i_frame < mFrames.size(); i_frame++)
  {
    unsigned int i_pos = i_frame*scale;// =floor()
    if (i_pos < 0)
      i_pos = 0;
    else if(i_pos >= mPositions.size()-1)
      i_pos = mPositions.size()-2;
      
    double t = (mFrames[i_frame].mTime - mPositions[i_pos].mTime) 
      / (mPositions[i_pos+1].mTime - mPositions[i_pos].mTime);
    mFrames[i_frame].mPos = interpolate(mPositions[i_pos].mPos, 
                                        mPositions[i_pos+1].mPos, t);
  }
}
  
/**
 * Reads a whitespace separated 4x4 matrix from file
 * \param fileName Input file
 * \return  The matrix
 */
Transform3D Reconstructer::readTransformFromFile(QString fileName)
{
  Transform3D retval;
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly))
  {
    std::cout << "Can't open file: " << string_cast(fileName) << std::endl;
    return retval;
  }
  bool ok = true;
  QByteArray array = file.readLine();
  if (!ok)
  {
    std::cout << "Can't read double in file: " << string_cast(fileName) 
    << std::endl;
    return retval;
  }
  QString positionString = file.readLine();
  positionString += " " + file.readLine();
  positionString += " " + file.readLine();
  positionString += " " + file.readLine();
  retval = Transform3D::fromString(positionString, &ok);
  if (!ok)
  { 
    std::cout << "Can't read calibration from file: " << string_cast(fileName) << std::endl;
    std::cout << "values: " << retval[0][0] << std::endl;;
    return retval;
  }
  return retval;
}
  
void Reconstructer::applyCalibration(const Transform3D& calibration)
{
  for (unsigned int i = 0; i < mPositions.size(); i++)
    mPositions[i].mPos = calibration * mPositions[i].mPos;
}

void Reconstructer::calibrate(QString calFile)
{
  Transform3D usMt = readTransformFromFile(calFile);
  applyCalibration(usMt);
}
  
/**
 * Find the transform matrix between output data space d and reference space pr
 * \return the prMd matrix
 */
Transform3D Reconstructer::find_prMd()
{
  Transform3D retval;
  retval = mFrames[mFrames.size()/2].mPos;
  retval = retval.inv();
  
  //TODO: Move origo for prMd
  
  return retval;
}
  
/**
 * Pre:  mFrames[i].mPos = usMpr
 * Post: mFrames[i].mPos = usMd
 *       Output volume is initialized
 */
ImagePtr Reconstructer::generateOutputVolume()
{
  Transform3D prMd = this->find_prMd();
  for (unsigned int i = 0; i < mFrames.size(); i++)
    mFrames[i].mPos = mFrames[i].mPos * prMd;
  
  int x = 400;
  int y = 200;
  int z = 400;
  
  //TODO: Must generalize
  vtkImageDataPtr data = vtkImageDataPtr::New();
  data->SetSpacing(0.115, 0.115, 0.115);
  data->SetExtent(0, x-1, 0, y-1, 0, z-1);
  data->SetScalarTypeToUnsignedChar();
  data->SetNumberOfScalarComponents(1);
  
  int scalarSize = x*y*z;
  
	unsigned char *rawchars = (unsigned char*)malloc(scalarSize+1);
  std::fill(rawchars,rawchars+scalarSize, 200);
  
  vtkUnsignedCharArrayPtr array = vtkUnsignedCharArrayPtr::New();
  array->SetNumberOfComponents(1);
  //TODO: Whithout the +1 the volume is black 
  array->SetArray(rawchars, scalarSize+1, 0); // take ownership
  data->GetPointData()->SetScalars(array);
  
  /*data->AllocateScalars();
  unsigned char* dataPtr = static_cast<unsigned char*>(data->GetScalarPointer());
  unsigned long N = data->GetNumberOfPoints();
  N--;//Don't understand this
  for (unsigned long i = 0; i < N; i++)
    dataPtr[i] = 200;
    
  //dataPtr[N] = 255;//This creates a black volume
  
  std::cout << "Reconstructer::generateOutputVolume() data->GetNumberOfPoints(): " 
    << N << std::endl;
  */
  
  //TODO: Create ouput volume name
  ImagePtr image = ImagePtr(new Image("tull", data, "tull")) ;
  return image;
}
  
  
ImagePtr Reconstructer::reconstruct(QString mhdFileName, QString calFileName)
{
  this->readFiles(mhdFileName); 
  //mPos is now tMpr
  
  this->calibrateTimeStamps();
  
  this->calibrate(calFileName);
  //mPos (in mPositions) is now usMpr
  
  this->interpolatePositions();
  // mFrames now mPos as usMpr
  
  mOutput = this->generateOutputVolume();
  //mPos in mFrames is now usMd
  
  //mOutput = mAlgorithm->reconstruct(mFrames, mOutput);
  
  return mOutput;
}
 
  
ImagePtr Reconstructer::getOutput()
{
  return mOutput;
}
  
}