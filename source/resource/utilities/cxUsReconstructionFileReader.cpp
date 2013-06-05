/*
 * cxUsReconstructionFileReader.cpp
 *
 *  \date Feb 3, 2011
 *      \author christiana
 */
#include "cxUsReconstructionFileReader.h"

#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QDataStream>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscDataManagerImpl.h"
#include <vtkImageData.h>
#include "sscImage.h"
#include "sscUtilHelpers.h"
#include "cxCreateProbeDataFromConfiguration.h"
#include "sscVolumeHelpers.h"
#include "sscUSFrameData.h"

namespace cx
{

UsReconstructionFileReader::UsReconstructionFileReader()
{

}

ssc::USReconstructInputData UsReconstructionFileReader::readAllFiles(QString fileName, QString calFilesPath)
{
  if (calFilesPath.isEmpty())
  {
    QStringList list = fileName.split("/");
    list[list.size()-1] = "";
    calFilesPath = list.join("/")+"/";
  }

  ssc::USReconstructInputData retval;

  // ignore if a directory is read - store folder name only
  if (QFileInfo(fileName).suffix().isEmpty())
    return retval;

  retval.mFilename = fileName;

  if (!QFileInfo(ssc::changeExtension(fileName, "fts")).exists())
  {
    // There may not be any files here due to the automatic calling of the function
    ssc::messageManager()->sendWarning("File not found: "+ssc::changeExtension(fileName, "fts")+", reconstruct load failed");
    return retval;
  }

  //Read US images
  retval.mUsRaw = this->readUsDataFile(fileName);

  std::pair<QString, ssc::ProbeData>  probeDataFull = this->readProbeDataBackwardsCompatible(ssc::changeExtension(fileName, "mhd"), calFilesPath);
  ssc::ProbeData  probeData = probeDataFull.second;
  // override spacing with spacing from image file. This is because the raw spacing from probe calib might have been changed by changing the sound speed.
  bool spacingOK = ssc::similar(probeData.getImage().mSpacing[0], retval.mUsRaw->getSpacing()[0], 0.001)
  	  	  	  	&& ssc::similar(probeData.getImage().mSpacing[1], retval.mUsRaw->getSpacing()[1], 0.001);
  if (!spacingOK)
  {
      ssc::messageManager()->sendWarning(""
    	  "Mismatch in spacing values from calibration and recorded image.\n"
    	  "This might be valid if the sound speed was changed prior to recording.\n"
    	  "Probe definition: "+ qstring_cast(probeData.getImage().mSpacing) + ", Acquired Image: " + qstring_cast(retval.mUsRaw->getSpacing())
    	  );
  }
  probeData.getImage().mSpacing = ssc::Vector3D(retval.mUsRaw->getSpacing());
  retval.mProbeData.setData(probeData);
  retval.mProbeUid = probeDataFull.first;

  retval.mFrames = this->readFrameTimestamps(fileName);
  retval.mPositions = this->readPositions(fileName);

  //mPos is now prMs
  retval.mMask = this->generateMask(retval);
  if (!this->readMaskFile(fileName, retval.mMask))
  {
  	retval.mMask = this->createMaskFromConfigParams(retval);
  }

  if (!retval.mFrames.empty())
  {
	  double msecs = (retval.mFrames.rbegin()->mTime - retval.mFrames.begin()->mTime);
	  ssc::messageManager()->sendInfo(QString("Read %1 seconds of us data from %2.").arg(msecs/1000, 0, 'g', 3).arg(fileName));
  }

  return retval;
}

/**Read the probe data either from the .probedata.xml file,
 * or from ProbeCalibConfigs.xml file for backwards compatibility.
 *
 */
std::pair<QString, ssc::ProbeData>  UsReconstructionFileReader::readProbeDataBackwardsCompatible(QString mhdFileName, QString calFilesPath)
{
	std::pair<QString, ssc::ProbeData>  retval = this->readProbeDataFromFile(mhdFileName);

	if (retval.second.getType()==ssc::ProbeData::tNONE)
	{
		ssc::messageManager()->sendInfo(QString("Invalid probe in %1, falling back to old format").arg(retval.first));
		QString caliFilename;
		QStringList probeConfigPath;
		this->readCustomMhdTags(mhdFileName, &probeConfigPath, &caliFilename);
		ProbeXmlConfigParser::Configuration configuration = this->readProbeConfiguration(calFilesPath, probeConfigPath);
		retval.second = createProbeDataFromConfiguration(configuration);
	}

	return retval;
}

ssc::ImagePtr UsReconstructionFileReader::createMaskFromConfigParams(ssc::USReconstructInputData data)
{
  vtkImageDataPtr mask = data.mProbeData.getMask();
  ssc::ImagePtr image = ssc::ImagePtr(new ssc::Image("mask", mask, "mask")) ;

  Eigen::Array3i usDim(data.mUsRaw->getDimensions());
  usDim[2] = 1;
  ssc::Vector3D usSpacing(data.mUsRaw->getSpacing());

  // checking
  bool spacingOK = true;
  spacingOK = spacingOK && ssc::similar(usSpacing[0], mask->GetSpacing()[0], 0.001);
  spacingOK = spacingOK && ssc::similar(usSpacing[1], mask->GetSpacing()[1], 0.001);
  bool dimOK = ssc::similar(usDim, Eigen::Array3i(mask->GetDimensions()));
  if (!dimOK || !spacingOK)
  {
    ssc::messageManager()->sendError("Reconstruction: mismatch in mask and image dimensions/spacing: ");
    if (!dimOK)
      ssc::messageManager()->sendError("Dim: Image: "+ qstring_cast(usDim) + ", Mask: " + qstring_cast(Eigen::Array3i(mask->GetDimensions())));
    if (!spacingOK)
      ssc::messageManager()->sendError("Spacing: Image: "+ qstring_cast(usSpacing) + ", Mask: " + qstring_cast(ssc::Vector3D(mask->GetSpacing())));
  }
  return image;
}

ssc::ImagePtr UsReconstructionFileReader::generateMask(ssc::USReconstructInputData data)
{
  Eigen::Array3i dim(data.mUsRaw->getDimensions());
  dim[2] = 1;
  ssc::Vector3D spacing(data.mUsRaw->getSpacing());

  vtkImageDataPtr raw = ssc::generateVtkImageData(dim, spacing, 255);

  ssc::ImagePtr image = ssc::ImagePtr(new ssc::Image("mask", raw, "mask")) ;
  return image;
}

void UsReconstructionFileReader::readCustomMhdTags(QString mhdFileName, QStringList* probeConfigPath, QString* calFileName)
{
  //Read XML info from mhd file
  //Stored in ConfigurationID tag
  QFile file(mhdFileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    ssc::messageManager()->sendWarning("Error in Reconstructer::readUsDataFile(): Can't open file: "
                                       + mhdFileName);
  }
  bool foundConfig = false;
  QStringList& configList = *probeConfigPath;
  bool foundCalFile = false;
  while (!file.atEnd())
  {
    QString line = file.readLine();
    if(line.startsWith("ConfigurationID", Qt::CaseInsensitive))
    {
      QStringList tempList = line.split("=", QString::SkipEmptyParts);
      configList = tempList[1].trimmed().split(":", QString::SkipEmptyParts);
      if (configList.size()>=3)
      	configList[3] = configList[3].trimmed();
      foundConfig = true;
    }
    else if(line.startsWith("ProbeCalibration", Qt::CaseInsensitive))
    {
      QStringList list = line.split("=", QString::SkipEmptyParts);
      *calFileName = list[1].trimmed();
      foundCalFile = true;
//      std::cout << "Calibration file used: " << *calFileName << std::endl;
    }
  }
  if(!foundConfig)
  {
    ssc::messageManager()->sendWarning(QString("Error in Reconstructer::readUsDataFile(): ")
                                       + "Can't find ConfigurationID in file: "
                                       + mhdFileName);
  }
  if(!foundCalFile)
  {
    ssc::messageManager()->sendWarning(QString("Error in Reconstructer::readUsDataFile(): ")
                                       + "Can't find ProbeCalibration in file: "
                                       + mhdFileName);
  }
}

ProbeXmlConfigParser::Configuration UsReconstructionFileReader::readProbeConfiguration(QString calFilesPath, QStringList probeConfigPath)
{
  if (probeConfigPath.size()!=4)
    return ProbeXmlConfigParser::Configuration();
  //Assumes ProbeCalibConfigs.xml file and calfiles have the same path
  QString xmlPath = calFilesPath+"ProbeCalibConfigs.xml";
  ProbeXmlConfigParser* xmlConfigParser = new ProbeXmlConfigParserImpl(xmlPath);

  ProbeXmlConfigParser::Configuration configuration;
  configuration = xmlConfigParser->getConfiguration(
      probeConfigPath[0],
      probeConfigPath[1],
      probeConfigPath[2],
      probeConfigPath[3]);

  return configuration;
}

std::pair<QString, ssc::ProbeData> UsReconstructionFileReader::readProbeDataFromFile(QString mhdFileName)
{
	std::pair<QString, ssc::ProbeData>  retval;
	QString filename = ssc::changeExtension(mhdFileName, "probedata.xml");

	if (!QFileInfo(filename).exists())
	{
		ssc::messageManager()->sendWarning("File not found: " + filename + ", failed to load probe data.");
		return retval;
	}

	ssc::XmlOptionFile file = ssc::XmlOptionFile(filename, "navnet");
	retval.second.parseXml(file.getElement("configuration"));

	retval.first = file.getElement("tool").toElement().attribute("toolID");

	return retval;
}

ssc::USFrameDataPtr UsReconstructionFileReader::readUsDataFile(QString mhdFileName)
{
	return ssc::USFrameData::create(mhdFileName);
}

std::vector<ssc::TimedPosition> UsReconstructionFileReader::readFrameTimestamps(QString fileName)
{
  bool useOldFormat = !QFileInfo(ssc::changeExtension(fileName, "fts")).exists();
  std::vector<ssc::TimedPosition> retval;

  if (useOldFormat)
  {
    this->readTimeStampsFile(ssc::changeExtension(fileName, "tim"), &retval);
  }
  else
  {
    this->readTimeStampsFile(ssc::changeExtension(fileName, "fts"), &retval);
  }
  return retval;
}

std::vector<ssc::TimedPosition> UsReconstructionFileReader::readPositions(QString fileName)
{
  bool useOldFormat = !QFileInfo(ssc::changeExtension(fileName, "fts")).exists();
  std::vector<ssc::TimedPosition> retval;

  if (useOldFormat)
  {
    this->readPositionFile(ssc::changeExtension(fileName, "pos"), true, &retval);
  }
  else
  {
    this->readPositionFile(ssc::changeExtension(fileName, "tp"), false, &retval);
    this->readTimeStampsFile(ssc::changeExtension(fileName, "tts"), &retval);
  }
  return retval;
}

void UsReconstructionFileReader::readTimeStampsFile(QString fileName,
                                       std::vector<ssc::TimedPosition>* timedPos)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly))
  {
    ssc::messageManager()->sendWarning("Can't open file: " + fileName);
    return;
  }
  bool ok = true;

  unsigned int i = 0;
  while (!file.atEnd())
  {
    if (i>=timedPos->size())
    {
      timedPos->push_back(ssc::TimedPosition());
    }

    QByteArray array = file.readLine();
    double time = QString(array).toDouble(&ok);
    if (!ok)
    {
      ssc::messageManager()->sendWarning("Can't read double in file: " + fileName);
      return;
    }
    timedPos->at(i).mTime = time;
    i++;
  }

}

void UsReconstructionFileReader::readPositionFile(QString posFile, bool alsoReadTimestamps, std::vector<ssc::TimedPosition>* timedPos)
{
  QFile file(posFile);
  if(!file.open(QIODevice::ReadOnly))
  {
    ssc::messageManager()->sendWarning("Can't open file: "
                                       + posFile);
    return;
  }
  bool ok = true;

  unsigned i = 0;
  while (!file.atEnd())
  {
    if (i>=timedPos->size())
    {
      timedPos->push_back(ssc::TimedPosition());
    }

    ssc::TimedPosition position;
    position.mTime = 0;
    if (alsoReadTimestamps)
    {
      //old format - timestamps embedded in pos file);
      QByteArray array = file.readLine();
      position.mTime = QString(array).toDouble(&ok);
      if (!ok)
      {
        ssc::messageManager()->sendWarning("Can't read double in file: "
                                           + posFile);
        return;
      }
    }

    QString positionString = file.readLine();
    positionString += " " + file.readLine();
    positionString += " " + file.readLine();
    positionString += " 0 0 0 1";
    position.mPos = ssc::Transform3D::fromString(positionString, &ok);
    if (!ok)
    {
      ssc::messageManager()->sendWarning("Can't read position number: "
                                         + qstring_cast(i)
                                         + " from file: "
                                         + posFile
                                         + "values: "
                                         + qstring_cast(position.mPos(0,0)));
      return;
    }
    timedPos->at(i) = position;
    i++;
  }

  return;
}

bool UsReconstructionFileReader::readMaskFile(QString mhdFileName, ssc::ImagePtr mask)
{
  QString fileName = ssc::changeExtension(mhdFileName, "msk");

  if (!QFileInfo(fileName).exists())
    return false;

  vtkImageDataPtr data = mask->getBaseVtkImageData();


  QFile file(fileName);
  file.open(QIODevice::ReadOnly);
  QDataStream stream(&file);

  unsigned char* dataPtr = static_cast<unsigned char*>(data->GetScalarPointer());
  char *rawchars = reinterpret_cast<char*>(dataPtr);

  stream.readRawData(rawchars, file.size());

  return true;
}


} // namespace cx
