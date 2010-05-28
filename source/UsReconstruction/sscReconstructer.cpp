/*
 *  sscReconstructer.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */


#include "sscReconstructer.h"

#include <QtCore>
#include <vtkImageData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>

#include "sscBoundingBox3D.h"
#include "sscDataManagerImpl.h"
#include "sscTypeConversions.h"

typedef vtkSmartPointer<class vtkUnsignedCharArray> vtkUnsignedCharArrayPtr;

namespace ssc
{

Reconstructer::Reconstructer() :
  mAlgorithm(new ThunderVNNReconstructAlgorithm)
{
  QString defPath = "/Users/christiana/workspace/sessions/";
  QString filename = "usReconstruct.xml";

  QDomDocument doc("us reconstruction");
  QFile file(defPath+filename);
  if (!file.open(QIODevice::ReadOnly))
  {
    std::cout << "file not found: " << defPath+filename << std::endl;
    return;
  }
  QString error;
  int line,col;
  if (!doc.setContent(&file, &error,&line,&col))
  {
    std::cout << "error setting xml content [" << line << "," << col << "]"<< error << std::endl;
      file.close();
      return;
  }
  file.close();

  std::cout << doc.toString(2) << std::endl;
  mSettings = doc;
}

QDomElement Reconstructer::getSettings() const
{
  return mSettings.documentElement();
}

void Reconstructer::setSettings()
{
  QString newOrient = this->getNamedSetting("Orientation").getValue();
  if (newOrient!=mLastAppliedOrientation)
  {
    clearAll();
    // reread everything.
    mLastAppliedOrientation = newOrient;
  }

  emit paramsChanged();
  // notify that settings xml is changed

  std::cout << "set settings - " << newOrient << std::endl;
  //std::cout << mSettings.toString(2) << std::endl;
}

StringOptionItem Reconstructer::getNamedSetting(const QString& uid)
{
  return StringOptionItem::fromName(uid, this->getSettings());
}

void Reconstructer::clearAll()
{
  mUsRaw.reset();
  mFrames.clear();
  mPositions.clear();
  mOutputVolumeParams = OutputVolumeParams();
  mMask.reset();///< Clipping mask for the input data
  //ReconstructAlgorithmPtr mAlgorithm;
  //ProbeXmlConfigParser::Configuration mConfiguration;
  //QDomDocument mSettings;
  //QString mLastAppliedOrientation; ///< the orientation algorithm used for the currently calculated extent.
  this->clearOutput();
}

void Reconstructer::clearOutput()
{
  mOutput.reset();
}


OutputVolumeParams Reconstructer::getOutputVolumeParams() const
{
  return mOutputVolumeParams;
}

void Reconstructer::setOutputVolumeParams(const OutputVolumeParams& par)
{
  mOutputVolumeParams = par;
  emit paramsChanged();
}

//long Reconstructer::getMaxOutputVolumeSize() const
//{
//  return mOutputVolumeParams.getMaxVolumeSize();
//}
//
//void Reconstructer::setMaxOutputVolumeSize(long val)
//{
//  mOutputVolumeParams.constrainVolumeSize(val);
//}
//
//ssc::DoubleBoundingBox3D Reconstructer::getExtent() const
//{
//  return mExtent;
//}
  
QString Reconstructer::changeExtension(QString name, QString ext)
{
  QStringList splitName = name.split(".");
  splitName[splitName.size()-1] = ext;
  return splitName.join(".");
}
  
bool within(int x, int min, int max)
{
  return (x>=min) && (x<=max);
}
  
  
void Reconstructer::readUsDataFile(QString mhdFileName)
{
  //Read US images
  mUsRaw = MetaImageReader().load(string_cast(mhdFileName), 
                                  string_cast(mhdFileName));
  
  //Read XML info from mdh file
  //Stored in ConfigurationID tag
  QFile file(mhdFileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    std::cout << "Error in Reconstructer::readUsDataFile(): Can't open file: ";
    std::cout << string_cast(mhdFileName) << std::endl;
  }
  bool found = false;
  while (!file.atEnd())
  {
    //QByteArray array = file.readLine();
    //QString line = QString(array);
    QString line = file.readLine();
    if(line.startsWith("ConfigurationID", Qt::CaseInsensitive))
    {
      QStringList tempList = line.split(" ", QString::SkipEmptyParts);
      QStringList list = tempList[2].split(":", QString::SkipEmptyParts);
      QString xmlPath = "ProbeCalibConfigs.xml"; //TODO: Use correct path
      ProbeXmlConfigParser* xmlConfigParser = new ProbeXmlConfigParser(xmlPath);
      QString config = list[3].trimmed();
      mConfiguration = xmlConfigParser->getConfiguration(list[0], list[1], list[2], config);
      found = true;
    }
  }
  if(!found)
  {
    std::cout << "Error in Reconstructer::readUsDataFile(): ";
    std::cout << "Can't find ConfigurationID in file: ";
    std::cout << string_cast(mhdFileName) << std::endl;
  }
  
  
  //testcode
  /*int* dims = mUsRaw->getBaseVtkImageData()->GetDimensions();
  double* spacing = mUsRaw->getBaseVtkImageData()->GetSpacing();
  unsigned char* ptr = static_cast<unsigned char*>(mUsRaw->getBaseVtkImageData()->GetScalarPointer());
  for (int x = 0; x < 50; x++)
    for(int y = 0; y < 50; y++)
      for(int z = 0; z < dims[2]; z++) 
    {
      //unsigned char val = ptr[x + y*dims[0] + z*dims[0]*dims[1]];
      ptr[x + y*dims[0] + z*dims[0]*dims[1]] = 255;      
    }
  
  int b = 20;
  for (int x = 0; x < dims[0]; x++)
    for(int y = 0; y < dims[1]; y++)
      for(int z = 0; z < dims[2]; z++) 
      {
        if (!within(x, b, dims[0]-b) || !within(y, b, dims[1]-b))
 //       if( (x<b) || (x>(dims[0]-b)))
 //          if ((y<b) || (y>(dims[1]-b)))
          ptr[x + y*dims[0] + z*dims[0]*dims[1]] = 255;      
      }
  */
  
  //Allcate place for position and time stamps for all frames
  mFrames.resize(mUsRaw->getBaseVtkImageData()->GetDimensions()[2]);
  
  std::cout << "Reconstructer::readUsDataFile() - succes. Number of frames: " 
    << mFrames.size() << std::endl;
  return;
}

void Reconstructer::readTimeStampsFile(QString fileName, 
                                       std::vector<TimedPosition>* timedPos)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly))
  {
    std::cout << "Can't open file: " << string_cast(fileName) << std::endl;
    return;
  }
  bool ok = true;
  
  unsigned int i = 0;
  while (!file.atEnd() && i<timedPos->size())
  {
    QByteArray array = file.readLine();
    double time = QString(array).toDouble(&ok);
    if (!ok)
    {
      std::cout << "Can't read double in file: " << string_cast(fileName) 
      << std::endl;
      return;
    }
    timedPos->at(i).mTime = time;
    i++;
  }
  
  if(i!=timedPos->size())
  {
    std::cout << "Reconstructer::readTimeStampsFile() - warning. ";
    std::cout << "timedPos->size(): " << timedPos->size();
    std::cout << ", read number of time stamps: ";
    std::cout << i << std::endl;
  }
  else
  {
    std::cout << "Reconstructer::readTimeStampsFile() - succes. ";
    std::cout << "Number of time stamps: ";
    std::cout << timedPos->size() << std::endl;
  }
  return;
}
  
void Reconstructer::readPositionFile(QString posFile, bool alsoReadTimestamps)
{
  QFile file(posFile);
  if(!file.open(QIODevice::ReadOnly))
  {
    std::cout << "Can't open file: " << string_cast(posFile) << std::endl;
    return;
  }
  bool ok = true;

  int i = 0;
  while (!file.atEnd())
  {
    TimedPosition position;
    if (alsoReadTimestamps)
    {
      //old format - timestamps embedded in pos file);
      QByteArray array = file.readLine();
      position.mTime = QString(array).toDouble(&ok);
      if (!ok)
      {
        std::cout << "Can't read double in file: " << string_cast(posFile)
          << std::endl;
        return;
      }
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
    //std::cout << positionString << std::endl;
    //std::cout << position.mPos << std::endl;
    //std::cout << position.mPos.inv().coord(ssc::Vector3D(0,0,0));  
    //std::cout << std::endl;
    i++;
  }
  
  //old format
  /*if(i!=numPos)
  {
    std::cout << "Reconstructer::readPositionFile() - warning. ";
    std::cout << "numPos: " << numPos << ", read number of pos: ";
    std::cout << mPositions.size() << std::endl;
  }*/
  std::cout << "Reconstructer::readPositionFile() - succes. ";
  std::cout << "Number of positions: ";
  std::cout << mPositions.size() << std::endl;
  return;
}
  
vtkImageDataPtr Reconstructer::generateVtkImageData(Vector3D dim, 
                                                  Vector3D spacing,
                                                  const unsigned char initValue)
{
  vtkImageDataPtr data = vtkImageDataPtr::New();
  data->SetSpacing(spacing[0], spacing[1], spacing[2]);
  data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
  data->SetScalarTypeToUnsignedChar();
  data->SetNumberOfScalarComponents(1);
  
  int scalarSize = dim[0]*dim[1]*dim[2];
  
	unsigned char *rawchars = (unsigned char*)malloc(scalarSize+1);
  std::fill(rawchars,rawchars+scalarSize, initValue);
  
  vtkUnsignedCharArrayPtr array = vtkUnsignedCharArrayPtr::New();
  array->SetNumberOfComponents(1);
  //TODO: Whithout the +1 the volume is black 
  array->SetArray(rawchars, scalarSize+1, 0); // take ownership
  data->GetPointData()->SetScalars(array);
  
  rawchars[0] = 255;// A trick to get a full LUT in ssc::Image (automatic LUT generation)
  
  /*data->AllocateScalars();
  unsigned char* dataPtr = static_cast<unsigned char*>(data->GetScalarPointer());
  unsigned long N = data->GetNumberOfPoints();
  N--;//Don't understand this
  for (unsigned long i = 0; i < N; i++)
    dataPtr[i] = 100;
  
  //dataPtr[N] = 255;//This creates a black volume
  
  dataPtr[0] = 255;
  
  std::cout << "Reconstructer::generateOutputVolume() data->GetNumberOfPoints(): " 
  << N << std::endl;*/
  
  
  return data;
}
  
ImagePtr Reconstructer::createMaskFromConfigParams()
{
  //TODO: Use corners instead of edges to allow for CLA and phased probes
  ssc::ImagePtr retval = this->generateMask();
  vtkImageDataPtr data = retval->getBaseVtkImageData();
  int* dim(mUsRaw->getBaseVtkImageData()->GetDimensions());
  unsigned char* dataPtr = static_cast<unsigned char*>(data->GetScalarPointer());
  for(int x = 0; x < dim[0]; x++)
    for(int y = 0; y < dim[1]; y++)
    {
      if(   (x < mConfiguration.mLeftEdge)
         || (x > mConfiguration.mRightEdge)
         || (y < mConfiguration.mTopEdge)
         || (y > mConfiguration.mBottomEdge) )
        dataPtr[x + y*dim[0]] = 0;
    }
  return retval;
}
  
ImagePtr Reconstructer::generateMask()
{  
  ssc::Vector3D dim(mUsRaw->getBaseVtkImageData()->GetDimensions());
  dim[2] = 1;
  ssc::Vector3D spacing(mUsRaw->getBaseVtkImageData()->GetSpacing());
  
  vtkImageDataPtr data = generateVtkImageData(dim, spacing, 255);
    
  ImagePtr image = ImagePtr(new Image("mask", data, "mask")) ;
  return image;
}
  
ImagePtr Reconstructer::readMaskFile(QString mhdFileName)
{  
  ssc::ImagePtr retval = this->generateMask();
  vtkImageDataPtr data = retval->getBaseVtkImageData();
    
  QString fileName = changeExtension(mhdFileName, "msk");
    
  QFile file(fileName);
  file.open(QIODevice::ReadOnly);
  QDataStream stream(&file);    
        
  unsigned char* dataPtr = static_cast<unsigned char*>(data->GetScalarPointer());
  char *rawchars = reinterpret_cast<char*>(dataPtr);
  
  stream.readRawData(rawchars, file.size());
  
  return retval;
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
  double framesSpan = mFrames.back().mTime - mFrames.front().mTime;
  double positionsSpan = mPositions.back().mTime - mPositions.front().mTime;
  double scale = framesSpan / positionsSpan;
  std::cout << "framesTimeSpan: " << framesSpan << ", positionsTimeSpan: " << positionsSpan << std::endl;
  
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
      
    double t_delta_tracking = mPositions[i_pos+1].mTime - mPositions[i_pos].mTime;
    double t = 0;
    if (!similar(t_delta_tracking, 0))
      t = (mFrames[i_frame].mTime - mPositions[i_pos].mTime) / t_delta_tracking;
    mFrames[i_frame].mPos = interpolate(mPositions[i_pos].mPos, 
                                        mPositions[i_pos+1].mPos, t);
    
    //std::cout << mFrames[i_frame].mPos.inv().coord(ssc::Vector3D(0,0,0));  
    //std::cout << std::endl;
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

  
/**
 * Pre:  mPos is sMpr
 * Post: mPos is uMpr
 */
void Reconstructer::calibrate(QString calFile)
{
  // Calibration from tool space to localizer = sMt
  Transform3D sMt = this->readTransformFromFile(calFile);
  
  //Geir Arne means calibration = tMs
  //Transform3D tMs = this->readTransformFromFile(calFile).inv();
  
  //testcode: Transform from image coordinate syst with origin in lower loft corner
  //Transform3D Rx = ssc::createTransformRotateX(-M_PI/2.0);
  //Transform3D Rz = ssc::createTransformRotateZ(-M_PI/2.0);
  //ssc::Transform3D R = (Rz*Rx);
  
  
  // Transform from image coordinate syst with origin in upper left corner
  // to t (tool) space
  Transform3D Rx = ssc::createTransformRotateX(M_PI/2.0);
  Transform3D Ry = ssc::createTransformRotateY(-M_PI/2.0);
  // Calibration from frame space u to tool space t
  ssc::Transform3D R = (Rx*Ry);
  
  int x_u = mConfiguration.mOriginCol * mConfiguration.mPixelWidth;
  int y_u = mConfiguration.mOriginRow * mConfiguration.mPixelHeight;
  ssc::Vector3D origin_u(x_u, y_u, 0);
  ssc::Vector3D origin_rotated = R.coord(origin_u);
  
  ssc::Transform3D T = ssc::createTransformTranslate(-origin_rotated);
  //ssc::Transform3D T = ssc::createTransformTranslate(origin_rotated);//test
  
  ssc::Transform3D tMu = T*R;
  //ssc::Transform3D tMu = T;//test
  
  //ssc::Transform3D sMu = tMs.inv()*tMu;
  ssc::Transform3D sMu = sMt*tMu;
  
  //testcode
  ssc::Vector3D ex_u(1,0,0);
  ssc::Vector3D ey_u(0,1,0);
  ssc::Vector3D ex_t = tMu.vector(ex_u);
  ssc::Vector3D ey_t = tMu.vector(ey_u);
  ssc::Vector3D origin_t = tMu.coord(origin_u);
  
  if (!similar(ex_t, ssc::Vector3D(0,-1,0)))
  {
    std::cout << "error ex_t: " << ex_t << std::endl;
  }
  //if (!similar(ey_t, ssc::Vector3D(0,0,-1)))
  if (!similar(ey_t, ssc::Vector3D(0,0,1)))
  {
    std::cout << "error ey_t: " << ey_t << std::endl;
  }
  if (!similar(origin_t, ssc::Vector3D(0,0,0)))
  {
    std::cout << "error origin_t: " << origin_t << std::endl;
  }
  
  //applyCalibration(sMu);
  
  //mPos is prMs
  for (unsigned int i = 0; i < mPositions.size(); i++)
  {
    //testcode
    //ssc::Transform3D sMpr = mPositions[i].mPos;
    ssc::Transform3D prMs = mPositions[i].mPos;
    //Calibration * Input position
    //ssc::Transform3D tMpr = tMs * sMpr;
    //ssc::Transform3D tMpr = tMs * prMs.inv();
    //ssc::Transform3D tMpr = sMt.inv() * sMpr;
    
    //ssc::Transform3D prMt = prMs * tMs.inv();
    //ssc::Transform3D prMt = prMs * sMt; 
    
    //std::cout << prMt.vector(ssc::Vector3D(0,1,0));
    //std::cout << prMt.coord(ssc::Vector3D(0,0,0));
    //std::cout << std::endl;
    
    mPositions[i].mPos = prMs * sMu; // * mPositions[i].mPos;
    
    ssc::Transform3D prMu = prMs * sMu;
    //std::cout << prMu.vector(ssc::Vector3D(1,0,0));
    //std::cout << prMu.coord(ssc::Vector3D(0,0,0));
    //std::cout << std::endl;
  }
  //mPos is prMu
  
}
  
/**
 * Generate a rectangle (2D) defining ROI in input image space
 */
std::vector<ssc::Vector3D> Reconstructer::generateInputRectangle()
{
  std::vector<ssc::Vector3D> retval(4);
  if(!mMask)
  {
    std::cout << "ERROR Reconstructer::generateInputRectangle() requires mask" << std::endl;
    return retval;
  }
  int* dims = mUsRaw->getBaseVtkImageData()->GetDimensions();
  double* spacing = mUsRaw->getBaseVtkImageData()->GetSpacing();
  
  int xmin = dims[0];
  int xmax = 0;
  int ymin = dims[1];
  int ymax = 0;
  unsigned char* ptr = static_cast<unsigned char*>(mMask->getBaseVtkImageData()->GetScalarPointer());
  for (int x = 0; x < dims[0]; x++)
    for(int y = 0; y < dims[1]; y++)
    {
      unsigned char val = ptr[x + y*dims[0]];
      if (val != 0)
      {
        xmin = std::min(xmin, x);
        ymin = std::min(ymin, y);
        xmax = std::max(xmax, x);
        ymax = std::max(ymax, y);
      }
    }
  
  retval[0] = ssc::Vector3D(xmin*spacing[0], ymin*spacing[1], 0);
  retval[1] = ssc::Vector3D(xmax*spacing[0], ymin*spacing[1], 0);
  retval[2] = ssc::Vector3D(xmin*spacing[0], ymax*spacing[1], 0);
  retval[3] = ssc::Vector3D(xmax*spacing[0], ymax*spacing[1], 0);
  
  std::cout << "x and y, min and max: " << xmin << " " << xmax << " " << ymin << " " << ymax << std::endl;
  
  /*retval[0] = ssc::Vector3D(0,0,0);
  retval[1] = ssc::Vector3D(dims[0]*spacing[0],0,0);
  retval[2] = ssc::Vector3D(0,dims[1]*spacing[1],0);
  retval[3] = ssc::Vector3D(dims[0]*spacing[0],dims[1]*spacing[1],0);*/
  return retval;
}

/**
 * Pre:  mFrames[i].mPos = prMu
 * Post: mFrames[i].mPos = d'Mu, where d' is an oriented but not translated data space.
 */
void Reconstructer::applyOutputOrientation()
{
  QString newOrient = this->getNamedSetting("Orientation").getValue();
  Transform3D prMd;

  if (newOrient=="PatientReference")
  {
    // identity
  }
  else if (newOrient=="MiddleFrame")
  {
    prMd = mFrames[mFrames.size()/2].mPos;
    prMd = prMd.inv();
  }
  else
  {
    std::cout << "ERROR: " << "no orientation algorithm selected in reconstruction" << std::endl;
  }

  // apply the selected orientation to the frames.
  for (unsigned int i = 0; i < mFrames.size(); i++)
  {
    mFrames[i].mPos = mFrames[i].mPos * prMd;
  }
}



/**
 * Compute the transform from input to output space using the
 * orientation of the mid-frame and the point cloud from the mask.
 * mExtent is computed as a by-product
 *
 * Pre:  mFrames[i].mPos = prMu
 * Post: mFrames[i].mPos = dMu
 *       mExtent is defined
 */
void Reconstructer::findExtentAndOutputTransform()
{
  this->applyOutputOrientation();

//  // A first guess for usMd with correct orientation
//  Transform3D prMd;
//  prMd = mFrames[mFrames.size()/2].mPos;
//  prMd = prMd.inv();
  
  /*for (unsigned int i = 0; i < mFrames.size(); i++)
  {
    mFrames[i].mPos = mFrames[i].mPos * prMd;
  }*/
  
  //mFrames[i].mPos = d'Mu, d' = only rotation
  
  // Find extent of all frames as a point cloud
  std::vector<ssc::Vector3D> inputRect = this->generateInputRectangle();
  std::vector<ssc::Vector3D> outputRect;
  //std::cout << "inputRect" << std::endl;
  for(unsigned int slice = 0; slice < mFrames.size(); slice++)
  {
    Transform3D dMu = mFrames[slice].mPos;
    for (unsigned int i = 0; i < inputRect.size(); i++)
    {
      outputRect.push_back(dMu.coord(inputRect[i]));
      //std::cout << " " << inputRect[i];
    }
    //std::cout << " " << dMus;
    //std::cout << " " << mFrames[slice].mPos;
    //std::cout << std::endl;
  }
  
  /*std::cout << "1st dMus:  " << mFrames.front().mPos.inv().coord(ssc::Vector3D(0,0,0));  
  std::cout << std::endl;
  std::cout << "last dMus: " << mFrames.back().mPos.inv().coord(ssc::Vector3D(0,0,0)); 
  std::cout << std::endl;*/
    
  ssc::DoubleBoundingBox3D extent = ssc::DoubleBoundingBox3D::fromCloud(outputRect);
  //mExtent = boundingBox;
    
  // Translate dMu to output volume origo
  ssc::Transform3D T_origo = ssc::createTransformTranslate(extent.corner(0,0,0));
  for (unsigned int i = 0; i < mFrames.size(); i++)
  {
    mFrames[i].mPos = T_origo.inv() * mFrames[i].mPos;
    //std::cout << mFrames[i].mPos.inv().coord(ssc::Vector3D(0,0,0));  
    //std::cout << std::endl;
  }

  // Calculate optimal output image spacing and dimensions based on US frame spacing
  double inputSpacing = std::min(mUsRaw->getBaseVtkImageData()->GetSpacing()[0],
                                 mUsRaw->getBaseVtkImageData()->GetSpacing()[1]);
  mOutputVolumeParams = OutputVolumeParams(extent, inputSpacing, ssc::Vector3D(mUsRaw->getBaseVtkImageData()->GetDimensions()));
  //mOutputVolumeParams.constrainVolumeSize(256*256*256*2);
  mOutputVolumeParams.constrainVolumeSize(1024*1024*16);
}
  
/**
 * Pre:  All data read, mExtent is calculated
 * Post: Output volume is initialized
 */
ImagePtr Reconstructer::generateOutputVolume()
{
  ssc::Vector3D dim = mOutputVolumeParams.getDim();
  ssc::Vector3D spacing = ssc::Vector3D(1,1,1) * mOutputVolumeParams.getSpacing();
  
  std::cout << "output dim: " << dim << std::endl;
  std::cout << "output spacing: " << spacing << std::endl;  
  
  vtkImageDataPtr data = this->generateVtkImageData(dim, spacing, 0);
  
  // Generate volume name and uid
  QString volumeName = qstring_cast(mUsRaw->getName());
  volumeName += "_reconstruct";
  QString volumeId = qstring_cast(mUsRaw->getUid());
  volumeId += "_reconstruct";
  ImagePtr image = ImagePtr(new Image(string_cast(volumeId), 
                                      data, 
                                      string_cast(volumeName))) ;
  return image;
}


void Reconstructer::readFiles(QString fileName, QString calFileName)
{
  this->clearAll();

  readUsDataFile(changeExtension(fileName, "mhd"));
    
  bool useOldFormat = !QFileInfo(changeExtension(fileName, "fts")).exists();

  if (useOldFormat)
  {
    readTimeStampsFile(changeExtension(fileName, "tim"), &mFrames);
    readPositionFile(changeExtension(fileName, "pos"), true);
  }
  else
  {
    readTimeStampsFile(changeExtension(fileName, "fts"), &mFrames);
    readPositionFile(changeExtension(fileName, "tp"), false);
    readTimeStampsFile(changeExtension(fileName, "tts"), &mPositions);
  }

  //mPos is now prMs

  if (QFileInfo(changeExtension(fileName, "msk")).exists())
  {
    mMask = this->readMaskFile(changeExtension(fileName, "msk"));
  }
  else
  {
    //mMask = this->generateMask();
    mMask = this->createMaskFromConfigParams();
  }

  this->calibrateTimeStamps();

  this->calibrate(calFileName);
  //mPos (in mPositions) is now prMu

  this->interpolatePositions();
  // mFrames: now mPos as prMu

  this->findExtentAndOutputTransform();
//  mOutput = this->generateOutputVolume();
  //mPos in mFrames is now dMu

  emit paramsChanged();
}

  
ImagePtr Reconstructer::reconstruct(QString mhdFileName, QString calFileName)
{
  std::cout << "Perform reconstruction on: " << mhdFileName << std::endl;
  std::cout << "Use calibration file: " << calFileName << std::endl;

  this->readFiles(mhdFileName, calFileName);
  mOutput = this->generateOutputVolume();

  this->reconstruct();

  return mOutput;
}

void Reconstructer::reconstruct()
{
  if (mFrames.empty() || !mUsRaw)
  {
    std::cout << "Reconstruct failed: no data loaded" << std::endl;
    return;
  }

  // do anyway: the dim might have changed
//  if (!mOutput)
//  {
    mOutput = this->generateOutputVolume();
//  }
  
  // reconstruction expects the inverted matrix direction: give it that.
  //std::vector<TimedPosition> mInvFrames = mFrames;
  
  //test
  //for (unsigned int i=0; i < mFrames.size(); i++)
  //  mInvFrames[i].mPos = mFrames[i].mPos.inv();
  
  //test
  //DataManager::getInstance()->loadImage(mUsRaw);
  
  /*double* spacing = mUsRaw->getBaseVtkImageData()->GetSpacing();
  ssc::Vector3D tp = ssc::Vector3D(370*spacing[0], 150*spacing[1], 0);
  for (int i = 0; i < mFrames.size(); i++)
  {
    std::cout << i << ": " << mFrames[i].mPos.coord(tp) << std::endl;
  }*/
  
  QDateTime pre = QDateTime::currentDateTime();
  mAlgorithm->reconstruct(mFrames, mUsRaw, mOutput, mMask);
  std::cout << "Reconstruct time: " << pre.time().msecsTo(QDateTime::currentDateTime().time()) << std::endl;

  DataManager::getInstance()->loadImage(mOutput);
  //DataManager::getInstance()->loadImage(mUsRaw);
}

 
  
ImagePtr Reconstructer::getOutput()
{
  return mOutput;
}
ImagePtr Reconstructer::getInput()
{
  return mUsRaw;
}
  
}
