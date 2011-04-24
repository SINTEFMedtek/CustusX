#include "cxUsReconstructionFileMaker.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <vtkImageChangeInformation.h>
#include <vtkImageLuminance.h>
#include "vtkImageAppend.h"
#include "vtkMetaImageWriter.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxDataLocations.h"
#include "cxSettings.h"

typedef vtkSmartPointer<vtkImageAppend> vtkImageAppendPtr;

namespace cx
{
UsReconstructionFileMaker::UsReconstructionFileMaker(ssc::TimedTransformMap trackerRecordedData, ssc::RTSourceRecorder::DataType streamRecordedData, QString sessionDescription, QString activepatientPath, ssc::ToolPtr tool) :
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
  QString reconstructionFolder = this->makeFolder(mActivepatientPath, mSessionDescription);

  this->writeTrackerTimestamps(reconstructionFolder);
  this->writeTrackerTransforms(reconstructionFolder);
  this->writeUSTimestamps(reconstructionFolder);
  QString calibrationFile = this->copyCalibrationFile(reconstructionFolder);
  this->writeUSImages(reconstructionFolder, calibrationFile);
  this->copyProbeCalibConfigsXml(reconstructionFolder);

  this->report();

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
    newPathName = subfolderAbsolutePath+"_"+QString::number(i++);
  }
  patientDir.cd(newPathName);

  retval = patientDir.absolutePath();
  return retval;
}

bool UsReconstructionFileMaker::createSubfolder(QString subfolderAbsolutePath)
{
  QDir dir;
  if(dir.exists(subfolderAbsolutePath))
    return false;

  dir.mkpath(subfolderAbsolutePath);
  dir.cd(subfolderAbsolutePath);
  mReport << "Made reconstruction folder: "+dir.absolutePath();
  return true;
}

bool UsReconstructionFileMaker::writeTrackerTimestamps(QString reconstructionFolder)
{
  bool success = false;

  QFile file(reconstructionFolder+"/"+mSessionDescription+".tts");
  if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    ssc::messageManager()->sendError("Cannot open "+file.fileName());
    return success;
  }
  QTextStream stream(&file);

  ssc::TimedTransformMap::iterator it = mTrackerRecordedData.begin();
  for(; it != mTrackerRecordedData.end(); ++it)
  {
    stream << qstring_cast(it->first);
    stream << endl;
  }
  file.close();
  success = true;

  QFileInfo info(file);
  mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(mTrackerRecordedData.size())+" tracking timestamps.";

  return success;
}

bool UsReconstructionFileMaker::writeTrackerTransforms(QString reconstructionFolder)
{
  bool success = false;
  QFile file(reconstructionFolder+"/"+mSessionDescription+".tp");
  if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    ssc::messageManager()->sendError("Cannot open "+file.fileName());
    return success;
  }
  QTextStream stream(&file);

  ssc::TimedTransformMap::iterator it = mTrackerRecordedData.begin();
  for(; it != mTrackerRecordedData.end(); ++it)
  {
    ssc::Transform3D transform = it->second;
    stream << transform(0,0) << " ";
    stream << transform(0,1) << " ";
    stream << transform(0,2) << " ";
    stream << transform(0,3);
    stream << endl;
    stream << transform(1,0) << " ";
    stream << transform(1,1) << " ";
    stream << transform(1,2) << " ";
    stream << transform(1,3);
    stream << endl;
    stream << transform(2,0) << " ";
    stream << transform(2,1) << " ";
    stream << transform(2,2) << " ";
    stream << transform(2,3);
    stream << endl;
  }
  file.close();
  success = true;

  QFileInfo info(file);
  mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(mTrackerRecordedData.size())+" tracking transforms.";

  return success;
}

bool UsReconstructionFileMaker::writeUSTimestamps(QString reconstructionFolder)
{
  bool success = false;

  QFile file(reconstructionFolder+"/"+mSessionDescription+".fts");
  if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    ssc::messageManager()->sendError("Cannot open "+file.fileName());
    return success;
  }
  QTextStream stream(&file);

  ssc::RTSourceRecorder::DataType::iterator it = mStreamRecordedData.begin();
  for(; it != mStreamRecordedData.end(); ++it)
  {
    stream << qstring_cast(it->first);
    stream << endl;
  }
  file.close();
  success = true;

  QFileInfo info(file);
  mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(mStreamRecordedData.size())+" frame timestamps.";

  return success;
}

/** Merge all us frames into one vtkImageData
 *
 */
vtkImageDataPtr UsReconstructionFileMaker::mergeFrames()
{
  vtkImageAppendPtr filter = vtkImageAppendPtr::New();
  filter->SetAppendAxis(2); // append along z-axis

  bool bw = settings()->value("Ultrasound/8bitAcquisitionData").toBool();

  int i=0;
  for(ssc::RTSourceRecorder::DataType::iterator it = mStreamRecordedData.begin(); it != mStreamRecordedData.end(); ++it)
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
  return filter->GetOutput();
}

bool UsReconstructionFileMaker::writeUSImages(QString reconstructionFolder, QString calibrationFile)
{
  bool success = false;

  QString mhdFilename = this->getMhdFilename(reconstructionFolder);
  vtkImageDataPtr usData = this->mergeFrames();

  vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
  writer->SetInput(usData);
  writer->SetFileName(cstring_cast(mhdFilename));
  writer->SetCompression(false);
  writer->Write();

  //mhd - custom fields
  QFile mhdFile(mhdFilename);
  if(!mhdFile.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    ssc::messageManager()->sendError("Cannot open "+mhdFile.fileName());
    return success;
  }
  QTextStream mhdStream(&mhdFile);
  if (mTool)
  {
    mhdStream << "ConfigurationID = " << mTool->getProbe()->getConfigurationPath() << '\n';
    mhdStream << "ProbeCalibration = " << calibrationFile << '\n';
  }
  mhdFile.close();
  success = true;

  QFileInfo mhdInfo(mhdFile);
  mReport << mhdInfo.fileName()+", "+qstring_cast(mhdInfo.size())+" bytes";

  QString rawFileName = mhdFilename.replace(QString(".mhd"), QString(".raw"));
  QFile rawFile(rawFileName);
  QFileInfo info(rawFile);
  mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(mStreamRecordedData.size())+" frames.";

  writer = NULL; // ensure file is closed (might not be necessary)
  return success;

}

QString UsReconstructionFileMaker::copyCalibrationFile(QString reconstructionFolder)
{
  ToolPtr cxTool = boost::dynamic_pointer_cast<Tool>(mTool);
  if (!mTool)
    return "";

  QString calibFileName = cxTool->getCalibrationFileName();
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

void UsReconstructionFileMaker::report()
{
  foreach(QString string, mReport)
  {
    ssc::messageManager()->sendSuccess(string, true);
  }
}

}//namespace cx
