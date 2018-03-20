/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxUsReconstructionFileReader.h"

#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QDataStream>
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include <vtkImageData.h>
#include "cxImage.h"
#include "cxUtilHelpers.h"
#include "cxCreateProbeDefinitionFromConfiguration.h"
#include "cxVolumeHelpers.h"
#include "cxUSFrameData.h"

namespace cx
{

UsReconstructionFileReader::UsReconstructionFileReader()
{

}

USReconstructInputData UsReconstructionFileReader::readAllFiles(QString fileName, QString calFilesPath)
{
  if (calFilesPath.isEmpty())
  {
    QStringList list = fileName.split("/");
    list[list.size()-1] = "";
    calFilesPath = list.join("/")+"/";
  }

  USReconstructInputData retval;

  // ignore if a directory is read - store folder name only
  if (QFileInfo(fileName).suffix().isEmpty())
    return retval;

  retval.mFilename = fileName;

  if (!QFileInfo(changeExtension(fileName, "fts")).exists())
  {
    // There may not be any files here due to the automatic calling of the function
    reportWarning("File not found: "+changeExtension(fileName, "fts")+", reconstruct load failed");
    return retval;
  }

  //Read US images
  retval.mUsRaw = this->readUsDataFile(fileName);

  std::pair<QString, ProbeDefinition>  probeDefinitionFull = this->readProbeDefinitionBackwardsCompatible(changeExtension(fileName, "mhd"), calFilesPath);
  ProbeDefinition  probeDefinition = probeDefinitionFull.second;
  // override spacing with spacing from image file. This is because the raw spacing from probe calib might have been changed by changing the sound speed.
    bool spacingOK = similar(probeDefinition.getSpacing()[0], retval.mUsRaw->getSpacing()[0], 0.001)
                                && similar(probeDefinition.getSpacing()[1], retval.mUsRaw->getSpacing()[1], 0.001);
  if (!spacingOK)
  {
      reportWarning(""
    	  "Mismatch in spacing values from calibration and recorded image.\n"
    	  "This might be valid if the sound speed was changed prior to recording.\n"
                "Probe definition: "+ qstring_cast(probeDefinition.getSpacing()) + ", Acquired Image: " + qstring_cast(retval.mUsRaw->getSpacing())
    	  );
  }
    probeDefinition.setSpacing(Vector3D(retval.mUsRaw->getSpacing()));
  retval.mProbeDefinition.setData(probeDefinition);
  retval.mProbeUid = probeDefinitionFull.first;

  retval.mFrames = this->readFrameTimestamps(fileName);
  retval.mPositions = this->readPositions(fileName);

	if (!this->valid(retval))
	{
		return USReconstructInputData();
	}

	//mPos is now prMs
  if (!retval.mFrames.empty())
  {
	  double msecs = (retval.mFrames.rbegin()->mTime - retval.mFrames.begin()->mTime);
	  report(QString("Read %1 seconds of us data from %2.").arg(msecs/1000, 0, 'g', 3).arg(fileName));
  }

  return retval;
}

bool UsReconstructionFileReader::valid(USReconstructInputData input)
{
	if (input.mUsRaw->getNumImages() != input.mFrames.size())
	{
		reportError("Mismatch between number of images and number of image positions.\n"
																"Images: " + qstring_cast(input.mUsRaw->getNumImages()) +
																" image positions: " + qstring_cast(input.mFrames.size()));
		return false;
	}
	return true;
}

/**Read the probe data either from the .probedata.xml file,
 * or from ProbeCalibConfigs.xml file for backwards compatibility.
 *
 */
std::pair<QString, ProbeDefinition>  UsReconstructionFileReader::readProbeDefinitionBackwardsCompatible(QString mhdFileName, QString calFilesPath)
{
	std::pair<QString, ProbeDefinition>  retval = this->readProbeDefinitionFromFile(mhdFileName);

	if (retval.second.getType()==ProbeDefinition::tNONE)
	{
		report(QString("Invalid probe in %1, falling back to old format").arg(retval.first));
		QString caliFilename;
		QStringList probeConfigPath;
		this->readCustomMhdTags(mhdFileName, &probeConfigPath, &caliFilename);
		ProbeXmlConfigParser::Configuration configuration = this->readProbeConfiguration(calFilesPath, probeConfigPath);
		retval.second = createProbeDefinitionFromConfiguration(configuration);
	}

	return retval;
}

ImagePtr UsReconstructionFileReader::generateMask(USReconstructInputData data)
{
  Eigen::Array3i dim(data.mUsRaw->getDimensions());
  dim[2] = 1;
  Vector3D spacing(data.mUsRaw->getSpacing());

  vtkImageDataPtr raw = generateVtkImageData(dim, spacing, 255);

  ImagePtr image = ImagePtr(new Image("mask", raw, "mask")) ;
  return image;
}

void UsReconstructionFileReader::readCustomMhdTags(QString mhdFileName, QStringList* probeConfigPath, QString* calFileName)
{
  //Read XML info from mhd file
  //Stored in ConfigurationID tag
  QFile file(mhdFileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    reportWarning("Error in Reconstructer::readUsDataFile(): Can't open file: "
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
    reportWarning(QString("Error in Reconstructer::readUsDataFile(): ")
                                       + "Can't find ConfigurationID in file: "
                                       + mhdFileName);
  }
  if(!foundCalFile)
  {
    reportWarning(QString("Error in Reconstructer::readUsDataFile(): ")
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
  ProbeXmlConfigParserPtr xmlConfigParser(new ProbeXmlConfigParserImpl(xmlPath));

  ProbeXmlConfigParser::Configuration configuration;
  configuration = xmlConfigParser->getConfiguration(
      probeConfigPath[0],
      probeConfigPath[1],
      probeConfigPath[2],
      probeConfigPath[3]);

  return configuration;
}

std::pair<QString, ProbeDefinition> UsReconstructionFileReader::readProbeDefinitionFromFile(QString mhdFileName)
{
	std::pair<QString, ProbeDefinition>  retval;
	QString filename = changeExtension(mhdFileName, "probedata.xml");

	if (!QFileInfo(filename).exists())
	{
		reportWarning("File not found: " + filename + ", failed to load probe data.");
		return retval;
	}

	XmlOptionFile file = XmlOptionFile(filename);
	retval.second.parseXml(file.getElement("configuration"));

	retval.first = file.getElement("tool").toElement().attribute("toolID");

	return retval;
}

USFrameDataPtr UsReconstructionFileReader::readUsDataFile(QString mhdFileName)
{
	return USFrameData::create(mhdFileName);
}

std::vector<TimedPosition> UsReconstructionFileReader::readFrameTimestamps(QString fileName)
{
  bool useOldFormat = !QFileInfo(changeExtension(fileName, "fts")).exists();
  std::vector<TimedPosition> retval;

  if (useOldFormat)
  {
    this->readTimeStampsFile(changeExtension(fileName, "tim"), &retval);
  }
  else
  {
    this->readTimeStampsFile(changeExtension(fileName, "fts"), &retval);
  }
  return retval;
}

std::vector<TimedPosition> UsReconstructionFileReader::readPositions(QString fileName)
{
  bool useOldFormat = !QFileInfo(changeExtension(fileName, "fts")).exists();
  std::vector<TimedPosition> retval;

  if (useOldFormat)
  {
    this->readPositionFile(changeExtension(fileName, "pos"), true, &retval);
  }
  else
  {
    this->readPositionFile(changeExtension(fileName, "tp"), false, &retval);
    this->readTimeStampsFile(changeExtension(fileName, "tts"), &retval);
  }
  return retval;
}

void UsReconstructionFileReader::readTimeStampsFile(QString fileName,
                                       std::vector<TimedPosition>* timedPos)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly))
  {
    reportWarning("Can't open file: " + fileName);
    return;
  }
  bool ok = true;

  unsigned int i = 0;
  while (!file.atEnd())
  {
    if (i>=timedPos->size())
    {
      timedPos->push_back(TimedPosition());
    }

    QByteArray array = file.readLine();
    double time = QString(array).toDouble(&ok);
    if (!ok)
    {
      reportWarning("Can't read double in file: " + fileName);
      return;
    }
    timedPos->at(i).mTime = time;
    i++;
  }

}

void UsReconstructionFileReader::readPositionFile(QString posFile, bool alsoReadTimestamps, std::vector<TimedPosition>* timedPos)
{
  QFile file(posFile);
  if(!file.open(QIODevice::ReadOnly))
  {
    reportWarning("Can't open file: "
                                       + posFile);
    return;
  }
  bool ok = true;

  unsigned i = 0;
  while (!file.atEnd())
  {
    if (i>=timedPos->size())
    {
      timedPos->push_back(TimedPosition());
    }

    TimedPosition position;
    position.mTime = 0;
    if (alsoReadTimestamps)
    {
      //old format - timestamps embedded in pos file);
      QByteArray array = file.readLine();
      position.mTime = QString(array).toDouble(&ok);
      if (!ok)
      {
        reportWarning("Can't read double in file: "
                                           + posFile);
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
      reportWarning("Can't read position number: "
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

bool UsReconstructionFileReader::readMaskFile(QString mhdFileName, ImagePtr mask)
{
  QString fileName = changeExtension(mhdFileName, "msk");

  if (!QFileInfo(fileName).exists())
    return false;

  vtkImageDataPtr data = mask->getBaseVtkImageData();


  QFile file(fileName);
  file.open(QIODevice::ReadOnly);
  QDataStream stream(&file);

  unsigned char* dataPtr = static_cast<unsigned char*>(data->GetScalarPointer());
  char *rawchars = reinterpret_cast<char*>(dataPtr);

  stream.readRawData(rawchars, file.size());
  setDeepModified(data);

  return true;
}


} // namespace cx
