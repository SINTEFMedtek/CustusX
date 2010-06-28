/*
 *  sscReconstructer.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */
#include "sscReconstructer.h"

#include <algorithm>
#include <QtCore>
#include <vtkImageData.h>
#include "matrixInterpolation.h"
#include "sscBoundingBox3D.h"
#include "sscDataManagerImpl.h"
#include "sscTypeConversions.h"
#include "sscXmlOptionItem.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscToolManager.h"
#include "sscMessageManager.h"
#include "sscThunderVNNReconstructAlgorithm.h"
#include "sscPNNReconstructAlgorithm.h"
#include "utils/sscReconstructHelper.h"

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ssc
{

Reconstructer::Reconstructer(QString appDataPath, QString shaderPath) :
  mOutputRelativePath(""),
  mOutputBasePath(""),
  mShaderPath(shaderPath),
  mLastAppliedMaskReduce("")
{
  QDomDocument doc("usReconstruction");
  doc.appendChild(doc.createElement("usReconstruct"));
  doc.documentElement().appendChild(doc.createElement("algorithms"));

  mSettings = XmlOptionFile(appDataPath+"/usReconstruct.xml", doc);

  mOrientationAdapter = StringDataAdapterXml::initialize("Orientation", "",
      "Algorithm to use for output volume orientation",
      "MiddleFrame",
      QString("PatientReference MiddleFrame").split(" "),
      mSettings.getElement());
  connect(mOrientationAdapter.get(), SIGNAL(valueWasSet()),   this,                      SLOT(setSettings()));
  connect(this,                      SIGNAL(paramsChanged()), mOrientationAdapter.get(), SIGNAL(changed()));

  
  
	mMaskReduce = StringDataAdapterXml::initialize("Reduce mask (% in 1D)", "",
                                                 "Speedup by reducing mask size",
                                                  "3", QString("0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15").split(" "),
                                                  mSettings.getElement());
  
  connect(mMaskReduce.get(), SIGNAL(valueWasSet()),   this, SLOT(setSettings()));
  
  mAlgorithmAdapter = StringDataAdapterXml::initialize("Algorithm", "",
      "Choose algorithm to use for reconstruction",
      "ThunderVNN",
      QString("ThunderVNN PNN").split(" "),
      mSettings.getElement());
  connect(mAlgorithmAdapter.get(), SIGNAL(valueWasSet()),   this,                    SLOT(setSettings()));
  connect(this,                    SIGNAL(paramsChanged()), mAlgorithmAdapter.get(), SIGNAL(changed()));

  createAlgorithm();
}

Reconstructer::~Reconstructer()
{
  mSettings.save();
}

void Reconstructer::createAlgorithm()
{
//  QString name = mSettings.getStringOption("Algorithm").getValue();
  QString name = mAlgorithmAdapter->getValue();

  if (mAlgorithm && mAlgorithm->getName()==name)
    return;

  // create new algo
  if (name=="ThunderVNN")
    mAlgorithm = ReconstructAlgorithmPtr(new ThunderVNNReconstructAlgorithm(mShaderPath));
  else if (name=="PNN")
    mAlgorithm = ReconstructAlgorithmPtr(new PNNReconstructAlgorithm());
  else
    mAlgorithm.reset();

  // generate settings for new algo
  if (mAlgorithm)
  {
    QDomElement algo = mSettings.getElement("algorithms", mAlgorithm->getName());
    mAlgoOptions = mAlgorithm->getSettings(algo);
    ssc::messageManager()->sendInfo("Using reconstruction algorithm " + string_cast(mAlgorithm->getName()));
  }
}

void Reconstructer::setSettings()
{
  this->createAlgorithm();

//  QString newOrient = mSettings.getStringOption("Orientation").getValue();
  QString newOrient = mOrientationAdapter->getValue();
  if (newOrient!=mLastAppliedOrientation || 
      mMaskReduce->getValue() != mLastAppliedMaskReduce)
  {
    mLastAppliedOrientation = newOrient;
    mLastAppliedMaskReduce = mMaskReduce->getValue();
    this->clearOutput();
    // reread everything.
    this->readFiles(mFilename, mCalFilesPath);
    ssc::messageManager()->sendInfo("set settings - " + string_cast(newOrient));
  }


  ssc::XmlOptionItem maxVol("MaxVolumeSize", mSettings.getElement());
  maxVol.writeValue(QString::number(mOutputVolumeParams.getMaxVolumeSize()));

  // notify that settings xml is changed
  emit paramsChanged();

  mSettings.save();
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

void Reconstructer::setOutputRelativePath(QString path)
{
  mOutputRelativePath = path;
}
  
void Reconstructer::setOutputBasePath(QString path)
{
  mOutputBasePath = path;
}
  
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
  
  //Split mhdFileName into file name and file path
  QStringList list = mhdFileName.split("/");
  QString fileName = list[list.size()-1];
  list[list.size()-1] = "";
  QString filePath = list.join("/");//+"/";
  
  // Remove file ending from file name
  list = fileName.split(".");
  if(list.size() > 1)
  {
    list[list.size()-1] = "";
    fileName = list.join("");
  }
  
  //Use file name as uid
  mUsRaw = MetaImageReader().load(string_cast(fileName), 
                                  string_cast(mhdFileName));
  mUsRaw->setFilePath(string_cast(filePath));
  
  //Read XML info from mdh file
  //Stored in ConfigurationID tag
  QFile file(mhdFileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    ssc::messageManager()->sendWarning("Error in Reconstructer::readUsDataFile(): Can't open file: "
                                       + string_cast(mhdFileName));
  }
  bool foundConfig = false;
  QStringList configList;
  bool foundCalFile = false;
  while (!file.atEnd())
  {
    //QByteArray array = file.readLine();
    //QString line = QString(array);
    QString line = file.readLine();
    if(line.startsWith("ConfigurationID", Qt::CaseInsensitive))
    {
      QStringList tempList = line.split("=", QString::SkipEmptyParts);
      configList = tempList[1].trimmed().split(":", QString::SkipEmptyParts);
      configList[3] = configList[3].trimmed();
      foundConfig = true;
    }
    else if(line.startsWith("ProbeCalibration", Qt::CaseInsensitive))
    {
      QStringList list = line.split("=", QString::SkipEmptyParts);
      mCalFileName = list[1].trimmed();
      foundCalFile = true;
    }
  }
  if(!foundConfig)
  {
    ssc::messageManager()->sendWarning(std::string("Error in Reconstructer::readUsDataFile(): ")
                                       + "Can't find ConfigurationID in file: "
                                       + string_cast(mhdFileName));
  }
  else
  {
    //Assumes ProbeCalibConfigs.xml file and calfiles have the same path
    ssc::messageManager()->sendInfo("Use mCalFilesPath: " + string_cast(mCalFilesPath));
    QString xmlPath = mCalFilesPath+"ProbeCalibConfigs.xml";
    ProbeXmlConfigParser* xmlConfigParser = new ProbeXmlConfigParser(xmlPath);
    mConfiguration = xmlConfigParser->getConfiguration(configList[0], 
                                                       configList[1], 
                                                       configList[2], 
                                                       configList[3]);
  }
  
  if(!foundCalFile)
  {
    ssc::messageManager()->sendWarning(std::string("Error in Reconstructer::readUsDataFile(): ")
                                       + "Can't find ProbeCalibration in file: "
                                       + string_cast(mhdFileName));
  }
  
  //Allcate place for position and time stamps for all frames
  mFrames.resize(mUsRaw->getBaseVtkImageData()->GetDimensions()[2]);
  
  //std::cout << "Reconstructer::readUsDataFile() - succes. Number of frames: " 
  //  << mFrames.size() << std::endl;
  return;
}

void Reconstructer::readTimeStampsFile(QString fileName, 
                                       std::vector<TimedPosition>* timedPos)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly))
  {
    ssc::messageManager()->sendWarning("Can't open file: " 
                                       + string_cast(fileName));
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
      ssc::messageManager()->sendWarning("Can't read double in file: " 
                                         + string_cast(fileName));
      return;
    }
    timedPos->at(i).mTime = time;
    i++;
  }
  
  if(i!=timedPos->size())
  {
    ssc::messageManager()->sendWarning(std::string("Reconstructer::readTimeStampsFile() ")
                                       + "timedPos->size(): " 
                                       + string_cast(timedPos->size())
                                       + ", read number of time stamps: "
                                       + string_cast(i));
  }
  else
  {
    //std::cout << "Reconstructer::readTimeStampsFile() - succes. ";
    //std::cout << "Number of time stamps: ";
    //std::cout << timedPos->size() << std::endl;
  }
  return;
}
  
void Reconstructer::readPositionFile(QString posFile, bool alsoReadTimestamps)
{
  QFile file(posFile);
  if(!file.open(QIODevice::ReadOnly))
  {
    ssc::messageManager()->sendWarning("Can't open file: "
                                       + string_cast(posFile));
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
        ssc::messageManager()->sendWarning("Can't read double in file: " 
                                           + string_cast(posFile));
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
      ssc::messageManager()->sendWarning("Can't read position number: "
                                         + string_cast(mPositions.size()) 
                                         + " from file: " 
                                         + string_cast(posFile)
                                         + "values: "
                                         + string_cast(position.mPos[0][0]));
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
  //std::cout << "Reconstructer::readPositionFile() - succes. ";
  //std::cout << "Number of positions: ";
  //std::cout << mPositions.size() << std::endl;
  return;
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
  
  vtkImageDataPtr data = ssc::generateVtkImageData(dim, spacing, 255);
    
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
  //TODO: Use data from a time calibration instead of this function
  ssc::messageManager()->sendInfo("Generate time calibration based on input time stamps.");
  double framesSpan = mFrames.back().mTime - mFrames.front().mTime;
  double positionsSpan = mPositions.back().mTime - mPositions.front().mTime;
  double scale = framesSpan / positionsSpan;
  //ssc::messageManager()->sendDebug("framesTimeSpan: " 
  //                                 + string_cast(framesSpan)
  //                                 + ", positionsTimeSpan: " 
  //                                 + string_cast(positionsSpan));
  
  double offset = mFrames.front().mTime - scale * mPositions.front().mTime;
  
  //ssc::messageManager()->sendDebug(string_cast("Reconstructor::calibrateTimeStamps()")
  //                                 + "NB!!! generated offset: " 
  //                                 + string_cast(offset)
  //                                 + " scale: "
  //                                 + string_cast(scale));
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
  for(unsigned i_frame = 0; i_frame < mFrames.size(); i_frame++)
  {
    unsigned i_pos = i_frame*scale;// =floor()
    if (i_pos < 0)
      i_pos = 0;
    else if(i_pos >= mPositions.size()-1)
      i_pos = mPositions.size()-2;

    double t_delta_tracking = mPositions[i_pos+1].mTime - mPositions[i_pos].mTime;
    double t = 0;
    if (!similar(t_delta_tracking, 0))
      t = (mFrames[i_frame].mTime - mPositions[i_pos].mTime) / t_delta_tracking;
    mFrames[i_frame].mPos = mPositions[i_pos].mPos;
//    mFrames[i_frame].mPos = interpolate(mPositions[i_pos].mPos,
//                                        mPositions[i_pos+1].mPos, t);

    //std::cout << mFrames[i_frame].mPos.inv().coord(ssc::Vector3D(0,0,0));
    //std::cout << std::endl;
  }
}

vnl_matrix_double convertSSC2VNL(const ssc::Transform3D& src)
{
//  std::cout << src[0][3] << " " << src[1][3] << " " << src[2][3] << " " << std::endl;

  vnl_matrix_double dst(4,4);
  for (int i=0; i<4; ++i)
    for (int j=0; j<4; ++j)
      dst[i][j] = src[i][j];
  return dst;
}

ssc::Transform3D convertVNL2SSC(const vnl_matrix_double& src)
{
  ssc::Transform3D dst;
  for (int i=0; i<4; ++i)
    for (int j=0; j<4; ++j)
      dst[i][j] = src[i][j];
  return dst;
}

//void Reconstructer::interpolatePositions()
//{
//  vnl_vector<double> DataPoints(mPositions.size());
//  std::vector<vnl_matrix_double> DataValues(mPositions.size());
//  vnl_vector<double> InterpolationPoints(mFrames.size());
//  std::string InterpolationMethod = "linear";
////  std::string InterpolationMethod = "closest point";
//
//  for (unsigned i=0; i<mPositions.size(); ++i)
//  {
//    DataValues[i] = convertSSC2VNL(mPositions[i].mPos.inv());
//    DataPoints[i] = mPositions[i].mTime;
//  }
//  for (unsigned i=0; i<mFrames.size(); ++i)
//  {
//    InterpolationPoints[i] = mFrames[i].mTime;
//  }
////  std::cout << "--------------------------------" << std::endl;
////  std::cout << "--------------------------------" << std::endl;
////  std::cout << "--------------------------------" << std::endl;
//
//  std::vector<vnl_matrix_double> result = matrixInterpolation(
//                         DataPoints,
//                         DataValues,
//                         InterpolationPoints,
//                         InterpolationMethod );
//
//  if (result.size()!=mFrames.size())
//  {
//    std::cout << "ERROR: failed to interpolate matrices" << std::endl;
//    return;
//  }
//
//  for (unsigned i=0; i<mFrames.size(); ++i)
//  {
//    mFrames[i].mPos = convertVNL2SSC(result[i]).inv();
//  }
//}

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
    ssc::messageManager()->sendWarning("Can't open file: " 
                                       + string_cast(fileName));
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
    ssc::messageManager()->sendWarning("Can't read calibration from file: "
                                       + string_cast(fileName)
                                       + "values: " 
                                       + string_cast(retval[0][0]));
    return retval;
  }
  return retval;
}

  
/**
 * Pre:  mPos is sMpr
 * Post: mPos is uMpr
 */
void Reconstructer::calibrate(QString calFilesPath)
{
  // Calibration from tool space to localizer = sMt
  Transform3D sMt = this->readTransformFromFile(calFilesPath+mCalFileName);
      
  //testcode: Transform from image coordinate syst with origin in lower left corner
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
  
  ssc::Transform3D tMu = T*R;
  
  ssc::Transform3D sMu = sMt*tMu;
  
  ssc::Vector3D ex_u(1,0,0);
  ssc::Vector3D ey_u(0,1,0);
  ssc::Vector3D ex_t = tMu.vector(ex_u);
  ssc::Vector3D ey_t = tMu.vector(ey_u);
  ssc::Vector3D origin_t = tMu.coord(origin_u);
  
  if (!similar(ex_t, ssc::Vector3D(0,-1,0)))
  {
    ssc::messageManager()->sendWarning("error ex_t: " + string_cast(ex_t));
  }
  if (!similar(ey_t, ssc::Vector3D(0,0,1)))
  {
    ssc::messageManager()->sendWarning("error ey_t: " + string_cast(ey_t));
  }
  if (!similar(origin_t, ssc::Vector3D(0,0,0)))
  {
    ssc::messageManager()->sendWarning("error origin_t: " 
                                       + string_cast(origin_t));
  }
  
  //mPos is prMs
  for (unsigned int i = 0; i < mPositions.size(); i++)
  {
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
    
    mPositions[i].mPos = prMs * sMu;
        
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
    ssc::messageManager()->sendError(string_cast("Reconstructer::generateInputRectangle()")
                                     + "requires mask");
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
  
  //Test: reduce the output volume by reducing the mask when determining 
  //      output volume size
  int reduceX = (xmax-xmin) * (mMaskReduce->getValue().toDouble() / 100);
  int reduceY = (ymax-ymin) * (mMaskReduce->getValue().toDouble() / 100);
  //messageManager()->sendDebug("reduceX: "+string_cast(reduceX));
  //messageManager()->sendDebug("reduceY: "+string_cast(reduceY));
  //messageManager()->sendDebug("test reduceX: "+string_cast(mMaskReduce->getValue().toDouble()));
  
  xmin += reduceX;
  xmax -= reduceX;
  ymin += reduceY;
  ymax -= reduceY;  
  
  retval[0] = ssc::Vector3D(xmin*spacing[0], ymin*spacing[1], 0);
  retval[1] = ssc::Vector3D(xmax*spacing[0], ymin*spacing[1], 0);
  retval[2] = ssc::Vector3D(xmin*spacing[0], ymax*spacing[1], 0);
  retval[3] = ssc::Vector3D(xmax*spacing[0], ymax*spacing[1], 0);
  
  //ssc::messageManager()->sendInfo("x and y, min and max: " 
  //                                + string_cast(xmin) + " "
  //                                + string_cast(xmax) + " " 
  //                                + string_cast(ymin) + " "
  //                                + string_cast(ymax));
  
  /*retval[0] = ssc::Vector3D(0,0,0);
  retval[1] = ssc::Vector3D(dims[0]*spacing[0],0,0);
  retval[2] = ssc::Vector3D(0,dims[1]*spacing[1],0);
  retval[3] = ssc::Vector3D(dims[0]*spacing[0],dims[1]*spacing[1],0);*/
  return retval;
}

/**Compute the orientation part of the transform prMd, denoted as prMdd.
 * /return the prMdd.
 *
 * Pre:  mFrames[i].mPos = prMu
 * Post: mFrames[i].mPos = d'Mu, where d' is an oriented but not translated data space.
 */
ssc::Transform3D Reconstructer::applyOutputOrientation()
{
//  QString newOrient = mSettings.getStringOption("Orientation").getValue();
  QString newOrient = mOrientationAdapter->getValue();
  ssc::Transform3D prMdd;

  if (newOrient=="PatientReference")
  {
    // identity
  }
  else if (newOrient=="MiddleFrame")
  {
    prMdd = mFrames[mFrames.size()/2].mPos;
  }
  else
  {
    ssc::messageManager()->sendError("no orientation algorithm selected in reconstruction");
  }

  // apply the selected orientation to the frames.
  ssc::Transform3D ddMpr = prMdd.inv();
  for (unsigned i = 0; i < mFrames.size(); i++)
  {
    // mPos = prMu
    mFrames[i].mPos = ddMpr * mFrames[i].mPos;
    // mPos = ddMu
  }

  return prMdd;
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
  // A first guess for d'Mu with correct orientation
  ssc::Transform3D prMdd = this->applyOutputOrientation();
  //mFrames[i].mPos = d'Mu, d' = only rotation
  
  // Find extent of all frames as a point cloud
  std::vector<ssc::Vector3D> inputRect = this->generateInputRectangle();
  std::vector<ssc::Vector3D> outputRect;
  for(unsigned slice = 0; slice < mFrames.size(); slice++)
  {
    Transform3D dMu = mFrames[slice].mPos;
    for (unsigned i = 0; i < inputRect.size(); i++)
    {
      outputRect.push_back(dMu.coord(inputRect[i]));
    }
  }
  
  /*std::cout << "1st dMus:  " << mFrames.front().mPos.inv().coord(ssc::Vector3D(0,0,0));  
  std::cout << std::endl;
  std::cout << "last dMus: " << mFrames.back().mPos.inv().coord(ssc::Vector3D(0,0,0)); 
  std::cout << std::endl;*/
    
  ssc::DoubleBoundingBox3D extent = ssc::DoubleBoundingBox3D::fromCloud(outputRect);
    
  // Translate dMu to output volume origo
  ssc::Transform3D T_origo = ssc::createTransformTranslate(extent.corner(0,0,0));
  ssc::Transform3D prMd = prMdd * T_origo; // transform from output space to patref, use when storing volume.
  for (unsigned i = 0; i < mFrames.size(); i++)
  {
    mFrames[i].mPos = T_origo.inv() * mFrames[i].mPos;
    //std::cout << mFrames[i].mPos.inv().coord(ssc::Vector3D(0,0,0));  
    //std::cout << std::endl;
  }

  // Calculate optimal output image spacing and dimensions based on US frame spacing
  double inputSpacing = std::min(mUsRaw->getBaseVtkImageData()->GetSpacing()[0],
                                 mUsRaw->getBaseVtkImageData()->GetSpacing()[1]);
  mOutputVolumeParams = OutputVolumeParams(extent, inputSpacing, ssc::Vector3D(mUsRaw->getBaseVtkImageData()->GetDimensions()));

  if (ssc::ToolManager::getInstance())
    mOutputVolumeParams.m_rMd = (*ssc::ToolManager::getInstance()->get_rMpr()) * prMd;
  else
    mOutputVolumeParams.m_rMd = prMd;

  //mOutputVolumeParams.constrainVolumeSize(256*256*256*2);
  ssc::XmlOptionItem maxVol("MaxVolumeSize", mSettings.getElement());
  mOutputVolumeParams.constrainVolumeSize(maxVol.readValue(QString::number(1024*1024*16)).toDouble());
}
  
/**
 * Pre:  All data read, mExtent is calculated
 * Post: Output volume is initialized
 */
ImagePtr Reconstructer::generateOutputVolume()
{
  ssc::Vector3D dim = mOutputVolumeParams.getDim();
  ssc::Vector3D spacing = ssc::Vector3D(1,1,1) * mOutputVolumeParams.getSpacing();
  
  //ssc::messageManager()->sendInfo("output dim: "
  //                                + string_cast(dim));
  //ssc::messageManager()->sendInfo("output spacing: "
  //                                + string_cast(spacing));  
  
  vtkImageDataPtr data = ssc::generateVtkImageData(dim, spacing, 0);
  
  // Add _rec to volume name and uid
  QString volumeName = qstring_cast(mUsRaw->getName()) + "_rec";
  QString volumeId = qstring_cast(mUsRaw->getUid()) + "_rec";
  
  std::vector<std::string> imageUids = DataManager::getInstance()->getImageUids();
 
  // Find an uid that is not used before
  int numMatches = std::count(imageUids.begin(), imageUids.end(), string_cast(volumeId));
  if(numMatches != 0)
  {
    int recNumber = 1;
    while(numMatches != 0)
    {
      QString newId = volumeName + QString::number(++recNumber);
      numMatches = std::count(imageUids.begin(), imageUids.end(), string_cast(newId));
    }
    
    volumeName += QString::number(recNumber);
    volumeId += QString::number(recNumber);
  }
  ImagePtr image = ImagePtr(new Image(string_cast(volumeId), 
                                      data, 
                                      string_cast(volumeName))) ;
  //If no output path is selecetd, use the same path as the input
  QString filePath;
  if(mOutputBasePath.isEmpty() && mOutputRelativePath.isEmpty())
    filePath = qstring_cast(mUsRaw->getFilePath());
  else
    filePath = mOutputRelativePath;
  filePath += "/" + volumeName + ".mhd";

  image->setFilePath(string_cast(filePath));
  image->set_rMd(mOutputVolumeParams.m_rMd);
  
  return image;
}


void Reconstructer::readFiles(QString fileName, QString calFilesPath)
{
  //std::cout << "calFilesPath: " << string_cast(calFilesPath) << std::endl;
  this->clearAll();
  mFilename = fileName;
  mCalFilesPath = calFilesPath;

  if (!QFileInfo(changeExtension(fileName, "mhd")).exists())
  {
    // There may not be any files here due to the automatic calling of the function
    //std::cout << "File not found: " << changeExtension(fileName, "mhd") <<", reconstruct load failed" << std::endl;
    return;
  }

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

  this->calibrate(calFilesPath);
  //mPos (in mPositions) is now prMu

  this->interpolatePositions();
  // mFrames: now mPos as prMu

  this->findExtentAndOutputTransform();
//  mOutput = this->generateOutputVolume();
  //mPos in mFrames is now dMu

  emit paramsChanged();
}

  
ImagePtr Reconstructer::reconstruct(QString mhdFileName, QString calFilesPath )
{
  ssc::messageManager()->sendInfo("Perform reconstruction on: "
                                  + string_cast(mhdFileName));
  //std::cout << "Use calibration file: " << calFileName << std::endl;

  this->readFiles(mhdFileName, calFilesPath);
  mOutput = this->generateOutputVolume();

  this->reconstruct();

  return mOutput;
}

void Reconstructer::reconstruct()
{
  if (mFrames.empty() || !mUsRaw)
  {
    ssc::messageManager()->sendError("Reconstruct failed: no data loaded");
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

  QDomElement algoSettings = mSettings.getElement("algorithms", mAlgorithm->getName());

  QDateTime pre = QDateTime::currentDateTime();
  mAlgorithm->reconstruct(mFrames, mUsRaw, mOutput, mMask, algoSettings);
  ssc::messageManager()->sendInfo("Reconstruct time: "
                                  + string_cast(pre.time().msecsTo(QDateTime::currentDateTime().time())));

  DataManager::getInstance()->loadImage(mOutput);
  //DataManager::getInstance()->loadImage(mUsRaw);
  
  //TODO: fix mOutputBasePath
  DataManager::getInstance()->saveImage(mOutput, string_cast(mOutputBasePath));
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
