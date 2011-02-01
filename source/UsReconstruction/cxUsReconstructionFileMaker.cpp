#include "cxUsReconstructionFileMaker.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <vtkImageChangeInformation.h>
#include <vtkImageLuminance.h>
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "vtkImageAppend.h"
#include "vtkMetaImageWriter.h"
#include "cxDataLocations.h"

typedef vtkSmartPointer<vtkImageAppend> vtkImageAppendPtr;

namespace cx
{
UsReconstructionFileMaker::UsReconstructionFileMaker(ssc::TimedTransformMap trackerRecordedData, ssc::RealTimeStreamSourceRecorder::DataType streamRecordedData, QString sessionDescription, QString activepatientPath, ToolPtr tool) :
    mTrackerRecordedData(trackerRecordedData),
    mStreamRecordedData(streamRecordedData),
    mSessionDescription(sessionDescription),
    mActivepatientPath(activepatientPath),
    mTool(tool)
{
  if(mTrackerRecordedData.empty())
    ssc::messageManager()->sendWarning("No tracking data for writing to reconstruction file.");
  if(mStreamRecordedData.empty())
    ssc::messageManager()->sendWarning("No real time streaming data for writing to reconstruction file.");
  if(mActivepatientPath.isEmpty())
    ssc::messageManager()->sendWarning("Active patient folder given to reconstruction file maker is empty.");
}

UsReconstructionFileMaker::~UsReconstructionFileMaker()
{}

QString UsReconstructionFileMaker::write()
{
//  std::cout << "UsReconstructionFileMaker::write() start" << std::endl;
  QString reconstructionFolder = this->makeFolder(mActivepatientPath, mSessionDescription);

  this->writeTrackerTimestamps(reconstructionFolder);
  this->writeTrackerTransforms(reconstructionFolder);
  this->writeUSTimestamps(reconstructionFolder);
  QString calibrationFile = this->copyCalibrationFile(reconstructionFolder);
  this->writeUSImages(reconstructionFolder, calibrationFile);
  this->copyProbeCalibConfigsXml(reconstructionFolder);

//  std::cout << "UsReconstructionFileMaker::write() stop" << std::endl;
  return reconstructionFolder;
}

QString UsReconstructionFileMaker::getMhdFilename(QString reconstructionFolder)
{
  QString mhdFilename = reconstructionFolder+"/"+mSessionDescription+".mhd";
  return mhdFilename;
}

QString UsReconstructionFileMaker::makeFolder(QString patientFolder, QString sessionDescription)
{
  QString retval("");
  QDir patientDir(patientFolder + "/US_Acq");

  QString subfolder = sessionDescription;
  QString subfolderAbsolutePath = patientDir.absolutePath()+"/"+subfolder;
  QString newPathName = subfolderAbsolutePath;
  int i=1;
  while(!this->createSubfolder(newPathName))
  {
    //newPathName = newPathName.append("_").append(QString::number(i++));
    newPathName = subfolderAbsolutePath+"_"+QString::number(i++);
  }
  patientDir.cd(newPathName);
  return  retval = patientDir.absolutePath();;
}

bool UsReconstructionFileMaker::createSubfolder(QString subfolderAbsolutePath)
{
  QDir dir;
  if(dir.exists(subfolderAbsolutePath))
    return false;

  dir.mkpath(subfolderAbsolutePath);
  dir.cd(subfolderAbsolutePath);
  ssc::messageManager()->sendInfo("Made reconstruction folder: "+dir.absolutePath());
  return true;
}

void UsReconstructionFileMaker::writeTrackerTimestamps(QString reconstructionFolder)
{
  QFile file(reconstructionFolder+"/"+mSessionDescription+".tts");
  if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    ssc::messageManager()->sendError("Cannot open "+file.fileName());
    return;
  }
  QTextStream stream(&file);

  ssc::TimedTransformMap::iterator it = mTrackerRecordedData.begin();
  for(; it != mTrackerRecordedData.end(); ++it)
  {
    stream << qstring_cast(it->first);
    stream << endl;
  }
  file.close();
}

void UsReconstructionFileMaker::writeTrackerTransforms(QString reconstructionFolder)
{
  QFile file(reconstructionFolder+"/"+mSessionDescription+".tp");
  if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    ssc::messageManager()->sendError("Cannot open "+file.fileName());
    return;
  }
  QTextStream stream(&file);

  ssc::TimedTransformMap::iterator it = mTrackerRecordedData.begin();
  for(; it != mTrackerRecordedData.end(); ++it)
  {
    ssc::Transform3D transform = it->second;
    stream << transform[0][0] << " ";
    stream << transform[0][1] << " ";
    stream << transform[0][2] << " ";
    stream << transform[0][3];
    stream << endl;
    stream << transform[1][0] << " ";
    stream << transform[1][1] << " ";
    stream << transform[1][2] << " ";
    stream << transform[1][3];
    stream << endl;
    stream << transform[2][0] << " ";
    stream << transform[2][1] << " ";
    stream << transform[2][2] << " ";
    stream << transform[2][3];
    stream << endl;
  }
  file.close();
}

void UsReconstructionFileMaker::writeUSTimestamps(QString reconstructionFolder)
{
  QFile file(reconstructionFolder+"/"+mSessionDescription+".fts");
  if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    ssc::messageManager()->sendError("Cannot open "+file.fileName());
    return;
  }
  QTextStream stream(&file);

  ssc::RealTimeStreamSourceRecorder::DataType::iterator it = mStreamRecordedData.begin();
  for(; it != mStreamRecordedData.end(); ++it)
  {
    stream << qstring_cast(it->first);
    stream << endl;
  }
  file.close();
}

/** Merge all us frames into one vtkImageData
 *
 */
vtkImageDataPtr UsReconstructionFileMaker::mergeFrames()
{
//  std::cout << "UsReconstructionFileMaker::mergeFrames() start " << std::endl;

  vtkImageAppendPtr filter = vtkImageAppendPtr::New();
  filter->SetAppendAxis(2); // append along z-axis

  bool bw = DataLocations::getSettings()->value("Ultrasound/8bitAcquisitionData").toBool();

  int i=0;
  for(ssc::RealTimeStreamSourceRecorder::DataType::iterator it = mStreamRecordedData.begin(); it != mStreamRecordedData.end(); ++it)
  {
    vtkImageDataPtr input = it->second;
    if (bw)
    {
      if (it->second->GetNumberOfScalarComponents()>2) // color
      {
        vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
        luminance->SetInput(input);
        input = luminance->GetOutput();
      }
    }

    filter->SetInput(i++, input);
  }

  filter->Update();
//  std::cout << "UsReconstructionFileMaker::mergeFrames() stop " << std::endl;
  return filter->GetOutput();
}

void UsReconstructionFileMaker::writeUSImages(QString reconstructionFolder, QString calibrationFile)
{
  QString mhdFilename = this->getMhdFilename(reconstructionFolder);
//  QString mhdFilename = reconstructionFolder+"/"+mSessionDescription->getDescription()+".mhd";

  vtkImageDataPtr usData = this->mergeFrames();

  if(mTool)
  {
    vtkImageChangeInformationPtr redirecter = vtkImageChangeInformationPtr::New();
    redirecter->SetInput(usData);
    redirecter->SetOutputSpacing(mTool->getProbeSector().mImage.mSpacing.begin());
    usData = redirecter->GetOutput();
    usData->Update();
  }


  //std::cout << "start write mhd file " << mhdFilename << std::endl;
  // write file to disk
  vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
  writer->SetInput(usData);
  writer->SetFileName(cstring_cast(mhdFilename));
  writer->SetCompression(false);
//  std::cout << "UsReconstructionFileMaker::writeUSImages start write" << std::endl;
  writer->Write();
//  std::cout << "UsReconstructionFileMaker::writeUSImages stop write" << std::endl;
  //std::cout << "finished write mhd file " << mhdFilename << std::endl;
  writer = NULL; // ensure file is closed (might not be necessary)

  //mhd - custom fields
  //--------------------------------------------------------------------------------------------------------------------
  QFile mhdFile(mhdFilename);
  if(!mhdFile.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    ssc::messageManager()->sendError("Cannot open "+mhdFile.fileName());
    return;
  }
  QTextStream mhdStream(&mhdFile);
  if (mTool)
  {
    mhdStream << "ConfigurationID = " << mTool->getConfigurationString() << '\n';
    mhdStream << "ProbeCalibration = " << calibrationFile << '\n';
  }
  //--------------------------------------------------------------------------------------------------------------------
  mhdFile.close();

}

QString UsReconstructionFileMaker::copyCalibrationFile(QString reconstructionFolder)
{
  if (!mTool)
    return "";
  QString calibFileName = mTool->getCalibrationFileName();
  QFile calibFile(calibFileName);
  QFileInfo info(calibFile);
  const QString filename = info.fileName();
  QString newFilePath = reconstructionFolder+"/"+filename;
  if(calibFile.exists())
  {
    if(!calibFile.copy(newFilePath))
      ssc::messageManager()->sendWarning("Could not copy calibration file ("+calibFileName+") to reconstruction folder. Maybe it already exitst in the destinbation folder?");
  }
  return filename;

}

void UsReconstructionFileMaker::copyProbeCalibConfigsXml(QString reconstructionFolder)
{
  QString xmlFileName = cx::DataLocations::getRootConfigPath()+QString("/tool/ProbeCalibConfigs.xml");
  QFile xmlFile(xmlFileName);
  QFileInfo info(xmlFile);
  const QString filename = info.fileName();
  if(xmlFile.exists())
  {
    if(!xmlFile.copy(reconstructionFolder+"/"+filename))
      ssc::messageManager()->sendWarning("Could not copy xml file ("+xmlFileName+") to reconstruction folder. Maybe it already exitst in the destinbation folder?");
  }
}


/*void UsReconstructionFileMaker::writeUSImages(QString reconstructionFolder, QString calibrationFile)
{
  //std::cout << "UsReconstructionFileMaker::writeUSImages sta" << std::endl;

  QFile mhdFile(reconstructionFolder+"/"+mSessionDescription->getDescription()+".mhd");
  QFile rawFile(reconstructionFolder+"/"+mSessionDescription->getDescription()+".raw");
  if(!mhdFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    ssc::messageManager()->sendError("Cannot open "+mhdFile.fileName());
    return;
  }
  if(!rawFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    ssc::messageManager()->sendError("Cannot open "+rawFile.fileName());
    return;
  }

  //mhd
  //--------------------------------------------------------------------------------------------------------------------
  QTextStream mhdStream(&mhdFile);

  ssc::RealTimeStreamSourceRecorder::DataType::iterator it = mStreamRecordedData.begin();
  if(it == mStreamRecordedData.end())
  {
    //no data to write, what to do?
    return;
  }
  vtkImageDataPtr image = it->second;

  int scalarComponents = image->GetNumberOfScalarComponents();
  int* frameDims = image->GetDimensions();
  int frameCount = mStreamRecordedData.size();
  int scalarType = image->GetScalarType();
  double* spacing = image->GetSpacing();
  QFileInfo rawInfo(rawFile);


  mhdStream << "NDims = 3" << '\n';

  mhdStream << "DimSize = ";
  mhdStream << frameDims[0] << " ";
  mhdStream << frameDims[1] << " ";
  mhdStream << frameCount;
  mhdStream << '\n';

  if(scalarType == VTK_UNSIGNED_CHAR && scalarComponents == 1)
    mhdStream << "ElementType = MET_UCHAR" << '\n'; //8 bit gray
  if(scalarType == VTK_UNSIGNED_CHAR  && scalarComponents == 4)
    mhdStream << "ElementType = MET_UINT" << '\n'; //32 bit RGBA
  if(scalarType == VTK_SHORT)
    mhdStream << "ElementType = MET_SHORT" << '\n';
  if(scalarType == VTK_UNSIGNED_SHORT)
    mhdStream << "ElementType = MET_USHORT" << '\n';
  if(scalarType == VTK_FLOAT)
    mhdStream << "ElementType = MET_FLOAT" << '\n';
  if(scalarType == VTK_DOUBLE)
    mhdStream << "ElementType = MET_DOUBLE" << '\n';

  mhdStream << "ElementSpacing = " << spacing[0] << " " << spacing[1] << " " << spacing[2]<< '\n';

  mhdStream << "ElementByteOrderMSB = false" << '\n';

  mhdStream << "ElementDataFile = " << rawInfo.fileName() <<   '\n';

  if (mTool)
  {
    mhdStream << "ConfigurationID = " << mTool->getConfigurationString() << '\n';
    mhdStream << "ProbeCalibration = " << mTool->getCalibrationFileName() << '\n';
  }
  //--------------------------------------------------------------------------------------------------------------------
  mhdFile.close();

  //raw
  //--------------------------------------------------------------------------------------------------------------------
  QDataStream rawStream(&rawFile);

  std::cout << "UsReconstructionFileMaker::writeUSImages " << rawFile.fileName() << std::endl;

  unsigned int nBytes = (frameDims[0]*frameDims[1])*scalarComponents;
  if(image->GetScalarSize() != 8)
    ssc::messageManager()->sendError("One scalar is not 8 bit, something is wrong!!!");
  ssc::messageManager()->sendDebug("image->GetScalarSize(): "+qstring_cast(image->GetScalarSize()));

  it = mStreamRecordedData.begin();
  for(; it != mStreamRecordedData.end(); ++it)
  {
    image = it->second;
    const char* pointer = static_cast<const char*>(image->GetScalarPointer());
    rawStream.writeBytes(pointer, nBytes);
  }
  //--------------------------------------------------------------------------------------------------------------------
  rawFile.close();
}*/


}//namespace cx
