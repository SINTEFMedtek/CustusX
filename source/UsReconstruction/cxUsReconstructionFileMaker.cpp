#include "cxUsReconstructionFileMaker.h"

#include <QDir>
#include "sscTypeConversions.h"
#include "sscMessageManager.h"

namespace cx
{
UsReconstructionFileMaker::UsReconstructionFileMaker(ssc::TimedTransformMap trackerRecordedData, ssc::RealTimeStreamSourceRecorder::DataType streamRecordedData, RecordSessionPtr session, QString activepatientPath, ToolPtr tool) :
    mTrackerRecordedData(trackerRecordedData),
    mStreamRecordedData(streamRecordedData),
    mSession(session),
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

void UsReconstructionFileMaker::write()
{
  QString reconstructionFolder = this->makeFolder(mActivepatientPath, mSession);

  this->writeTrackerTimestamps(reconstructionFolder);
  this->writeTrackerTransforms(reconstructionFolder);
  this->writeUSTimestamps(reconstructionFolder);
  this->writeUSImages(reconstructionFolder);
}

QString UsReconstructionFileMaker::makeFolder(QString patientFolder, RecordSessionPtr session)
{
  QString retval("");
  QDir patientDir(patientFolder);
  std::cout << "patientDir: " << string_cast(patientDir.absolutePath()) << std::endl;
  QString subfolder = session->getDescription();
  if(patientDir.mkdir(subfolder))
  {
    std::cout << "patientDir (sub?): " << patientDir.absolutePath() << std::endl;
    ssc::messageManager()->sendInfo("Made reconstruction folder: "+patientDir.absolutePath());
  }
  else
  {
    ssc::messageManager()->sendError("Could not create the reconstruction folder, putting files in patient folder.");
  }
  return  retval = patientDir.absolutePath();;
}

void UsReconstructionFileMaker::writeTrackerTimestamps(QString reconstructionFolder)
{
  QFile file(reconstructionFolder+"/"+mSession->getDescription()+".tts");
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
  QFile file(reconstructionFolder+"/"+mSession->getDescription()+".tp");
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
  QFile file(reconstructionFolder+"/"+mSession->getDescription()+".fts");
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

void UsReconstructionFileMaker::writeUSImages(QString reconstructionFolder)
{
  QFile mhdFile(reconstructionFolder+"/"+mSession->getDescription()+".mdh");
  QFile rawFile(reconstructionFolder+"/"+mSession->getDescription()+".raw");
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
    mhdStream << "ElementType = MET_UCHAR" << '\n'; //32 bit RGBA
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

  mhdStream << "ConfigurationID = " << mTool->getProbeSectorConfigurationString() << '\n';

  mhdStream << "ProbeCalibration = " << mTool->getCalibrationFileName() << '\n';
  //--------------------------------------------------------------------------------------------------------------------
  mhdFile.close();

  //raw
  //--------------------------------------------------------------------------------------------------------------------
  QDataStream rawStream(&rawFile);

  unsigned int nBytes = (frameDims[0]*frameDims[1])*scalarComponents;
  if(image->GetScalarSize() != 8)
    ssc::messageManager()->sendError("One scalar is not 8 bit, something is wrong!!!");

  it = mStreamRecordedData.begin();
  for(; it != mStreamRecordedData.end(); ++it)
  {
    image = it->second;
    const char* pointer = static_cast<const char*>(image->GetScalarPointer());
    rawStream.writeBytes(pointer, nBytes);
  }
  //--------------------------------------------------------------------------------------------------------------------
  rawFile.close();
}
}//namespace cx
