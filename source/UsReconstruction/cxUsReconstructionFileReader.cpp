/*
 * cxUsReconstructionFileReader.cpp
 *
 *  Created on: Feb 3, 2011
 *      Author: christiana
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

namespace cx
{

UsReconstructionFileReader::UsReconstructionFileReader()
{

}

void UsReconstructionFileReader::readCustomMhdTags(QString mhdFileName, QStringList* probeConfigPath, QString* calFileName)
{
  //Read XML info from mdh file
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
  //Assumes ProbeCalibConfigs.xml file and calfiles have the same path
  ssc::messageManager()->sendInfo("Use mCalFilesPath: " + calFilesPath);
  QString xmlPath = calFilesPath+"ProbeCalibConfigs.xml";
  ProbeXmlConfigParser* xmlConfigParser = new ProbeXmlConfigParser(xmlPath);

  ProbeXmlConfigParser::Configuration configuration;
  configuration = xmlConfigParser->getConfiguration(
      probeConfigPath[0],
      probeConfigPath[1],
      probeConfigPath[2],
      probeConfigPath[3]);

  return configuration;
}


ssc::USFrameDataPtr UsReconstructionFileReader::readUsDataFile(QString mhdFileName)
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
  ssc::ImagePtr UsRaw = boost::shared_dynamic_cast<ssc::Image>(ssc::MetaImageReader().load(fileName, mhdFileName));
  UsRaw->setFilePath(filePath);
  ssc::USFrameDataPtr retval;
  retval.reset(new ssc::USFrameData(UsRaw));
  return retval;
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

//  if(i!=timedPos->size())
//  {
//    ssc::messageManager()->sendWarning(QString("Reconstructer::readTimeStampsFile() ")
//                                       + "timedPos->size(): "
//                                       + qstring_cast(timedPos->size())
//                                       + ", read number of time stamps: "
//                                       + qstring_cast(i));
//  }
//  else
//  {
//    //std::cout << "Reconstructer::readTimeStampsFile() - succes. ";
//    //std::cout << "Number of time stamps: ";
//    //std::cout << timedPos->size() << std::endl;
//  }
//  return;
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
                                         + qstring_cast(position.mPos[0][0]));
      return;
    }
    timedPos->at(i) = position;
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

/**
 * Reads a whitespace separated 4x4 matrix from file
 * \param fileName Input file
 * \return  The matrix
 */
ssc::Transform3D UsReconstructionFileReader::readTransformFromFile(QString fileName)
{
  ssc::Transform3D retval;
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly))
  {
    ssc::messageManager()->sendWarning("Can't open file: "
                                       + fileName);
    return retval;
  }
  bool ok = true;
  QString positionString = file.readLine();
  positionString += " " + file.readLine();
  positionString += " " + file.readLine();
  positionString += " " + file.readLine();
  retval = ssc::Transform3D::fromString(positionString, &ok);
  if (!ok)
  {
    ssc::messageManager()->sendWarning("Can't read calibration from file: "
                                       + fileName
                                       + "values: "
                                       + qstring_cast(retval[0][0]));
    return retval;
  }
  return retval;
}


} // namespace cx
