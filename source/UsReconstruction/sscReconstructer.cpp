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
#include "sscDataManager.h"
#include "sscXmlOptionItem.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscToolManager.h"
#include "sscMessageManager.h"
#include "sscThunderVNNReconstructAlgorithm.h"
#include "sscPNNReconstructAlgorithm.h"
#include "utils/sscReconstructHelper.h"
#include "sscTime.h"
#include "sscTypeConversions.h"
#include "sscRegistrationTransform.h"
#include "sscUtilHelpers.h"
#include "cxCreateProbeDataFromConfiguration.h"
#include "sscVolumeHelpers.h"
#include "cxUsReconstructionFileReader.h"

#include "cxToolManager.h"
#include "sscManualTool.h"

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ssc
{

Reconstructer::Reconstructer(XmlOptionFile settings, QString shaderPath) :
  mOutputRelativePath(""),
  mOutputBasePath(""),
  mShaderPath(shaderPath),
  mMaxTimeDiff(100)// TODO: Change default value for max allowed time difference between tracking and image time tags 
{
  mFileReader.reset(new cx::UsReconstructionFileReader());

  mSettings = settings;
  mSettings.getElement("algorithms");

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
  
  mAlignTimestamps = BoolDataAdapterXml::initialize("Align timestamps", "",
                                                 "Align the first of tracker and frame timestamps, ignoring lags.",
                                                  false, mSettings.getElement());
  connect(mAlignTimestamps.get(), SIGNAL(valueWasSet()),   this, SLOT(setSettings()));
  

  mTimeCalibration = DoubleDataAdapterXml::initialize("Time Calibration", "",
                                                 "Set an offset in the frame timestamps",
                                                  0.0, DoubleRange(-1000, 1000, 10), 0,
                                                  mSettings.getElement());
  connect(mTimeCalibration.get(), SIGNAL(valueWasSet()),   this, SLOT(setSettings()));

  mAlgorithmAdapter = StringDataAdapterXml::initialize("Algorithm", "",
      "Choose algorithm to use for reconstruction",
      "PNN",
      //QString("ThunderVNN PNN").split(" "),
      QString("PNN").split(" "),
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
  QString name = mAlgorithmAdapter->getValue();

  if (mAlgorithm && mAlgorithm->getName()==name)
    return;

  // create new algo
  if (name=="ThunderVNN")
  {
    //mAlgorithm = ReconstructAlgorithmPtr(new ThunderVNNReconstructAlgorithm(mShaderPath));
  }
  else if (name=="PNN")
    mAlgorithm = ReconstructAlgorithmPtr(new PNNReconstructAlgorithm());
  else
    mAlgorithm.reset();

  // generate settings for new algo
  if (mAlgorithm)
  {
    QDomElement algo = mSettings.getElement("algorithms", mAlgorithm->getName());
    mAlgoOptions = mAlgorithm->getSettings(algo);
    ssc::messageManager()->sendInfo("Using reconstruction algorithm " + mAlgorithm->getName());
  }
}

void Reconstructer::setSettings()
{
  this->createAlgorithm();

  this->updateFromOriginalFileData();
  if (!this->validInputData())
    return;

  ssc::XmlOptionItem maxVol("MaxVolumeSize", mSettings.getElement());
  maxVol.writeValue(QString::number(mOutputVolumeParams.getMaxVolumeSize()));

  mSettings.save();

  emit paramsChanged();
}

void Reconstructer::clearAll()
{
  mFileData = FileData();
  mOriginalFileData = FileData();
  mOutputVolumeParams = OutputVolumeParams();
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
  
bool within(int x, int min, int max)
{
  return (x>=min) && (x<=max);
}

ImagePtr Reconstructer::createMaskFromConfigParams()
{
  vtkImageDataPtr mask = mOriginalFileData.mProbeData.getMask();
  ImagePtr image = ImagePtr(new Image("mask", mask, "mask")) ;

  ssc::Vector3D usDim(mOriginalFileData.mUsRaw->getDimensions());
  usDim[2] = 1;
  ssc::Vector3D usSpacing(mOriginalFileData.mUsRaw->getSpacing());

  // checking
  bool spacingOK = similar(usSpacing, ssc::Vector3D(mask->GetSpacing()), 0.001);
  bool dimOK = similar(usDim, ssc::Vector3D(mask->GetDimensions()));
  if (!dimOK || !spacingOK)
  {
    ssc::messageManager()->sendError("Reconstruction: mismatch in mask and image dimensions/spacing: ");
    if (!dimOK)
      ssc::messageManager()->sendError("Dim: Image: "+ qstring_cast(usDim) + ", Mask: " + qstring_cast(ssc::Vector3D(mask->GetDimensions())));
    if (!spacingOK)
      ssc::messageManager()->sendError("Spacing: Image: "+ qstring_cast(usSpacing) + ", Mask: " + qstring_cast(ssc::Vector3D(mask->GetSpacing())));
  }
  return image;
}
  
ImagePtr Reconstructer::generateMask()
{  
  ssc::Vector3D dim(mOriginalFileData.mUsRaw->getDimensions());
  dim[2] = 1;
  ssc::Vector3D spacing(mOriginalFileData.mUsRaw->getSpacing());
  
  vtkImageDataPtr data = ssc::generateVtkImageData(dim, spacing, 255);
    
  ImagePtr image = ImagePtr(new Image("mask", data, "mask")) ;
  return image;
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
  for (unsigned int i = 0; i < mFileData.mPositions.size(); i++)
  {
    mFileData.mPositions[i].mTime = scale * mFileData.mPositions[i].mTime + offset;
  }
}

/**
 * Calculate timestamp calibration in an adhoc way, by assuming that 
 * images and positions start and stop at the exact same time.
 *
 * This is an option only - use with care!
 */
void Reconstructer::alignTimeSeries()
{
  ssc::messageManager()->sendInfo("Generate time calibration based on input time stamps.");
  double framesSpan = mFileData.mFrames.back().mTime - mFileData.mFrames.front().mTime;
  double positionsSpan = mFileData.mPositions.back().mTime - mFileData.mPositions.front().mTime;
  double scale = framesSpan / positionsSpan;
  
  double offset = mFileData.mFrames.front().mTime - scale * mFileData.mPositions.front().mTime;
  
  this->calibrateTimeStamps(offset, scale);
}

/** Calibrate the input tracker and frame timestamps.
 *
 *  Add a  constant time shift if set. (this comes in addition to
 *  a time calibration set in the probe calibration file).
 *
 *  If set, ignore the relative positioning between time series
 *  and rather set the first tracker and frame time equal.
 *
 */
void Reconstructer::applyTimeCalibration()
{
  double timeshift = mTimeCalibration->getValue();
  // The shift is on frames. The calibrate function applies to tracker positions,
  // hence the negative sign.
  std::cout << "TIMESHIFT " << timeshift << std::endl;
  timeshift = -timeshift;
  if (!ssc::similar(0.0, timeshift))
    ssc::messageManager()->sendInfo("Applying reconstruction-time calibration to tracking data: " + qstring_cast(timeshift) + "ms");
  this->calibrateTimeStamps(timeshift, 1.0);

  // ignore calibrations
  if (mAlignTimestamps->getValue())
  {
    this->alignTimeSeries();
  }
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
  
  for(unsigned i_frame = 0; i_frame < mFileData.mFrames.size();)
  {
    std::vector<TimedPosition>::iterator posIter;
    posIter= lower_bound(mFileData.mPositions.begin(), mFileData.mPositions.end(), mFileData.mFrames[i_frame]);
    
    unsigned i_pos = distance(mFileData.mPositions.begin(), posIter);
    if (i_pos != 0)
      i_pos--;
     
    if(i_pos >= mFileData.mPositions.size()-1)
      i_pos = mFileData.mPositions.size()-2;
    
    // Remove frames too far from the positions
    // Don't increment frame index since the index now points to the next element
    if ((fabs(mFileData.mFrames[i_frame].mTime - mFileData.mPositions[i_pos].mTime) > mMaxTimeDiff) ||
        (fabs(mFileData.mFrames[i_frame].mTime - mFileData.mPositions[i_pos+1].mTime) > mMaxTimeDiff))
    {
      mFileData.mFrames.erase(mFileData.mFrames.begin() + i_frame);
      mFileData.mUsRaw->removeFrame(i_frame);

      double diff1 = fabs(mFileData.mFrames[i_frame].mTime - mFileData.mPositions[i_pos].mTime);
      double diff2 = fabs(mFileData.mFrames[i_frame].mTime - mFileData.mPositions[i_pos+1].mTime);
      ssc::messageManager()->sendWarning("Time difference is too large. Removed input frame: " + qstring_cast(i_frame) + ", difference is: "+ qstring_cast(diff1) + " or "+ qstring_cast(diff2));
    }
    else
    {      
      double t_delta_tracking = mFileData.mPositions[i_pos+1].mTime - mFileData.mPositions[i_pos].mTime;
      double t = 0;
      if (!similar(t_delta_tracking, 0))
        t = (mFileData.mFrames[i_frame].mTime - mFileData.mPositions[i_pos].mTime) / t_delta_tracking;
      //    mFrames[i_frame].mPos = mPositions[i_pos].mPos;
      mFileData.mFrames[i_frame].mPos = interpolate(mFileData.mPositions[i_pos].mPos,
          mFileData.mPositions[i_pos+1].mPos, t);
      i_frame++;// Only increment if we didn't delete the frame
    }
  }
}

vnl_matrix_double convertSSC2VNL(const ssc::Transform3D& src)
{
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

/**
 * Pre:  mPos is sMpr
 * Post: mPos is uMpr
 */
void Reconstructer::calibrate(QString calFilesPath)
{
  // Calibration from tool space to localizer = sMt
  Transform3D sMt = mFileReader->readTransformFromFile(calFilesPath+mCalFileName);
  
  // Transform from image coordinate syst with origin in upper left corner
  // to t (tool) space. TODO check is u is ul corner or ll corner.
  ssc::Transform3D tMu = mFileData.mProbeData.get_tMu() * mFileData.mProbeData.get_uMv();

  ssc::Transform3D sMu = sMt*tMu;
  
  //mPos is prMs
  for (unsigned i = 0; i < mFileData.mPositions.size(); i++)
  {
    ssc::Transform3D prMt = mFileData.mPositions[i].mPos;
    ssc::Transform3D prMs = prMt * sMt.inv();
    mFileData.mPositions[i].mPos = prMs * sMu;
  }
  //mPos is prMu
}


/**
 * Generate a rectangle (2D) defining ROI in input image space
 */
std::vector<ssc::Vector3D> Reconstructer::generateInputRectangle()
{
  std::vector<ssc::Vector3D> retval(4);
  if(!mFileData.mMask)
  {
    ssc::messageManager()->sendError("Reconstructer::generateInputRectangle() + requires mask");
    return retval;
  }
  int* dims = mFileData.mUsRaw->getDimensions();
  ssc::Vector3D spacing = mFileData.mUsRaw->getSpacing();
  
  int xmin = dims[0];
  int xmax = 0;
  int ymin = dims[1];
  int ymax = 0;
  unsigned char* ptr = static_cast<unsigned char*>(mFileData.mMask->getBaseVtkImageData()->GetScalarPointer());
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
  
  xmin += reduceX;
  xmax -= reduceX;
  ymin += reduceY;
  ymax -= reduceY;  
  
  retval[0] = ssc::Vector3D(xmin*spacing[0], ymin*spacing[1], 0);
  retval[1] = ssc::Vector3D(xmax*spacing[0], ymin*spacing[1], 0);
  retval[2] = ssc::Vector3D(xmin*spacing[0], ymax*spacing[1], 0);
  retval[3] = ssc::Vector3D(xmax*spacing[0], ymax*spacing[1], 0);
  
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
    prMdd = mFileData.mFrames[mFileData.mFrames.size()/2].mPos;
  }
  else
  {
    ssc::messageManager()->sendError("no orientation algorithm selected in reconstruction");
  }

  // apply the selected orientation to the frames.
  ssc::Transform3D ddMpr = prMdd.inv();
  for (unsigned i = 0; i < mFileData.mFrames.size(); i++)
  {
    // mPos = prMu
    mFileData.mFrames[i].mPos = ddMpr * mFileData.mFrames[i].mPos;
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
  for(unsigned slice = 0; slice < mFileData.mFrames.size(); slice++)
  {
    Transform3D dMu = mFileData.mFrames[slice].mPos;
    for (unsigned i = 0; i < inputRect.size(); i++)
    {
      outputRect.push_back(dMu.coord(inputRect[i]));
    }
  }

  ssc::DoubleBoundingBox3D extent = ssc::DoubleBoundingBox3D::fromCloud(outputRect);
    
  // Translate dMu to output volume origo
  ssc::Transform3D T_origo = ssc::createTransformTranslate(extent.corner(0,0,0));
  ssc::Transform3D prMd = prMdd * T_origo; // transform from output space to patref, use when storing volume.
  for (unsigned i = 0; i < mFileData.mFrames.size(); i++)
  {
    mFileData.mFrames[i].mPos = T_origo.inv() * mFileData.mFrames[i].mPos;
  }

  // Calculate optimal output image spacing and dimensions based on US frame spacing
  double inputSpacing = std::min(mFileData.mUsRaw->getSpacing()[0],
      mFileData.mUsRaw->getSpacing()[1]);
  mOutputVolumeParams = OutputVolumeParams(extent, inputSpacing, ssc::Vector3D(mFileData.mUsRaw->getDimensions()));

  if (ssc::ToolManager::getInstance())
    mOutputVolumeParams.m_rMd = (*ssc::ToolManager::getInstance()->get_rMpr()) * prMd;
  else
    mOutputVolumeParams.m_rMd = prMd;

  //mOutputVolumeParams.constrainVolumeSize(256*256*256*2);
  ssc::XmlOptionItem maxVol("MaxVolumeSize", mSettings.getElement());
  mOutputVolumeParams.constrainVolumeSize(maxVol.readValue(QString::number(1024*1024*16)).toDouble());
}
  

/** Generate an output uid based on the assumption that input uid
 * is on the format "US-Acq_01_20001224T170000".
 * Change to "US_01_20001224T170000",
 * or add a "rec" postfix if a different name format is detected.
 */
QString Reconstructer::generateOutputUid()
{
  QString base = mFileData.mUsRaw->getUid();
  QString name = mFilename.split("/").back();
  name = name.split(".").front();

  QStringList split = name.split("_");
  QStringList prefix = split.front().split("-");
  if (prefix.size()==2)
  {
    split[0] = prefix[0];
  }
  else
  {
    split[0] += "rec";
  }

  return split.join("_");
}


/**Generate a pretty name for for volume based on the filename.
 * Assume filename has format US-Acq_01_20001224T170000 or similar.
 * Format: US <counter> <hh:mm>, for example US 3 15:34
 */
QString Reconstructer::generateImageName(QString uid) const
{
  QString name = uid.split("/").back();
  name = name.split(".").front();
  QString prefix = name.split("_").front(); // retrieve US-Acq part
  prefix = prefix.split("-").front(); // retrieve US part.
  if (prefix.isEmpty())
    prefix = "US";

  // retrieve  index counter from _99_
  QString counter = "";
  QRegExp countReg("_[0-9]{1,2}_");
  if (countReg.indexIn(name)>0)
  {
    counter = countReg.cap(0).remove("_");
  }

  // retrieve timestamp as HH:MM
  QRegExp tsReg("[0-9]{8}T[0-9]{6}");
  if (tsReg.indexIn(name)>0)
  {
    QDateTime datetime = QDateTime::fromString(tsReg.cap(0), timestampSecondsFormat());
    QString timestamp = datetime.toString("hh:mm");
    return prefix + " " + counter + " " + timestamp;
  }

  return name;
}

bool Reconstructer::validInputData() const
{
  if (mOriginalFileData.mFrames.empty() || !mOriginalFileData.mUsRaw || mOriginalFileData.mPositions.empty())
    return false;
  return true;
}


/**
 * Pre:  All data read, mExtent is calculated
 * Post: Output volume is initialized
 */
ImagePtr Reconstructer::generateOutputVolume()
{
  ssc::Vector3D dim = mOutputVolumeParams.getDim();
  ssc::Vector3D spacing = ssc::Vector3D(1,1,1) * mOutputVolumeParams.getSpacing();
  
  vtkImageDataPtr data = ssc::generateVtkImageData(dim, spacing, 0);
  
  //If no output path is selecetd, use the same path as the input
  QString filePath;
  if(mOutputBasePath.isEmpty() && mOutputRelativePath.isEmpty())
    filePath = qstring_cast(mFileData.mUsRaw->getFilePath());
  else
    filePath = mOutputRelativePath;

  QString uid = this->generateOutputUid();
  QString name = this->generateImageName(uid);

  ImagePtr image = dataManager()->createImage(data, uid + "_%1", name + " %1", filePath);
  image->get_rMd_History()->setRegistration(mOutputVolumeParams.m_rMd);

  return image;
}

void Reconstructer::selectData(QString filename, QString calFilesPath)
{
  if(filename.isEmpty())
  {
    ssc::messageManager()->sendWarning("no file selected");
    return;
  }

  if (calFilesPath.isEmpty())
  {
    QStringList list = filename.split("/");
    list[list.size()-1] = "";
    calFilesPath = list.join("/")+"/";
  }

  this->clearAll();
  this->readCoreFiles(filename, calFilesPath);
  this->updateFromOriginalFileData();

  emit inputDataSelected(filename);
}

/**Read from file into mOriginalFileData.
 * These data are not changed before clearAll() or this method is called again.
 */
void Reconstructer::readCoreFiles(QString fileName, QString calFilesPath)
{
  mFilename = fileName;
  mCalFilesPath = calFilesPath;

  // ignore if a directory is read - store folder name only
  if (QFileInfo(fileName).isDir())
    return;

  QString mhdFileName = changeExtension(fileName, "mhd");

  if (!QFileInfo(changeExtension(fileName, "mhd")).exists())
  {
    // There may not be any files here due to the automatic calling of the function
    ssc::messageManager()->sendWarning("File not found: "+changeExtension(fileName, "mhd")+", reconstruct load failed");
    return;
  }

  //Read US images
  mOriginalFileData.mUsRaw = mFileReader->readUsDataFile(mhdFileName);

  QStringList probeConfigPath;
  mFileReader->readCustomMhdTags(mhdFileName, &probeConfigPath, &mCalFileName);
  ProbeXmlConfigParser::Configuration configuration = mFileReader->readProbeConfiguration(mCalFilesPath, probeConfigPath);
  ProbeData probeData = createProbeDataFromConfiguration(configuration);
  // override spacing with spacing from image file. This is because the raw spacing from probe calib might have been changed by changing the sound speed.
  probeData.mImage.mSpacing = Vector3D(mOriginalFileData.mUsRaw->getSpacing());
  mOriginalFileData.mProbeData.setData(probeData);

  mOriginalFileData.mFrames = mFileReader->readFrameTimestamps(fileName);
  mOriginalFileData.mPositions = mFileReader->readPositions(fileName);

  //mPos is now prMs
  mOriginalFileData.mMask = this->generateMask();
  if (!mFileReader->readMaskFile(fileName, mOriginalFileData.mMask))
  {
    mOriginalFileData.mMask = this->createMaskFromConfigParams();
  }

}

/**Use the mOriginalFileData structure to rebuild all internal data.
 * Useful when settings have changed or data is loaded.
 */
void Reconstructer::updateFromOriginalFileData()
{
  this->clearOutput();

  if (!this->validInputData())
    return;

  mFileData = mOriginalFileData;
  mFileData.mUsRaw.reset(new ssc::USFrameData(mOriginalFileData.mUsRaw->getBase()));

  // Only use this if the time stamps have different formatsh
  // The function assumes that both lists of time stamps start at the same time,
  // and that is not completely corrcet.
  //this->calibrateTimeStamps();
  // Use the time calibration from the aquisition module
  //this->calibrateTimeStamps(0.0, 1.0);
  this->applyTimeCalibration();

  this->calibrate(mCalFilesPath);

  //mPos (in mPositions) is now prMu

  this->interpolatePositions();
  // mFrames: now mPos as prMu
  if(!this->validInputData())
  {
    ssc::messageManager()->sendError("Invalid reconstruct input.");
    return;
  }

  this->findExtentAndOutputTransform();
//  mOutput = this->generateOutputVolume();
  //mPos in mFrames is now dMu

  emit paramsChanged();
}

void Reconstructer::reconstruct()
{
  if (!this->validInputData())
  {
    ssc::messageManager()->sendError("Reconstruct failed: no data loaded");
    return;
  }
  ssc::messageManager()->sendInfo("Perform reconstruction on: " + mFilename);

  this->threadedPreReconstruct();
  this->threadedReconstruct();
  this->threadedPostReconstruct();
}

/**The reconstruct part that must be fun pre-rec in the main thread.
 *
 */
void Reconstructer::threadedPreReconstruct()
{
  if (!this->validInputData())
    return;
  mOutput = this->generateOutputVolume();
}

/**The reconstruct part that can be run in a separate thread.
 *
 */
void Reconstructer::threadedReconstruct()
{
  if (!this->validInputData())
    return;

  QDateTime startTime = QDateTime::currentDateTime();

  QDomElement algoSettings = mSettings.getElement("algorithms", mAlgorithm->getName());
  mAlgorithm->reconstruct(mFileData.mFrames, mFileData.mUsRaw, mOutput, mFileData.mMask, algoSettings);

  QTime tempTime = QTime(0, 0);
  tempTime = tempTime.addMSecs(startTime.time().msecsTo(QDateTime::currentDateTime().time()));
  ssc::messageManager()->sendInfo("Reconstruct time: " + tempTime.toString("hh:mm:ss:zzz"));
}

/**The reconstruct part that must be fun post-rec in the main thread.
 *
 */
void Reconstructer::threadedPostReconstruct()
{
  if (!this->validInputData())
    return;

  ssc::messageManager()->sendSuccess("Reconstruction done, "+mOutput->getName());

  DataManager::getInstance()->loadData(mOutput);
  DataManager::getInstance()->saveImage(mOutput, mOutputBasePath);
}

ImagePtr Reconstructer::getOutput()
{
  return mOutput;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


ThreadedReconstructer::ThreadedReconstructer(ReconstructerPtr reconstructer)
{
  mReconstructer = reconstructer;
  mReconstructer->threadedPreReconstruct();
  connect(this, SIGNAL(finished()), this, SLOT(postReconstructionSlot())); // ensure this slot is run before all other listeners.
}
void ThreadedReconstructer::run()
{
  mReconstructer->threadedReconstruct();
}
void ThreadedReconstructer::postReconstructionSlot()
{
  mReconstructer->threadedPostReconstruct();
}


  
}
