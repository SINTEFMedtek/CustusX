#include "cxUsReconstructionFileMaker.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <vtkImageChangeInformation.h>
#include <vtkImageLuminance.h>
#include <vtkImageData.h>
#include "vtkImageAppend.h"
#include "vtkMetaImageWriter.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxDataLocations.h"
#include "cxSettings.h"

typedef vtkSmartPointer<vtkImageAppend> vtkImageAppendPtr;

namespace cx
{
UsReconstructionFileMaker::UsReconstructionFileMaker(ssc::TimedTransformMap trackerRecordedData, ssc::VideoRecorder::DataType streamRecordedData, QString sessionDescription, QString activepatientPath, ssc::ToolPtr tool, QString calibFilename) :
    mTrackerRecordedData(trackerRecordedData),
    mStreamRecordedData(streamRecordedData),
    mSessionDescription(sessionDescription),
    mActivepatientPath(activepatientPath),
    mTool(tool),
    mCalibFilename(calibFilename)
{
  if(mTrackerRecordedData.empty())
    ssc::messageManager()->sendWarning("No tracking data for writing to reconstruction file.");
  if(mStreamRecordedData.empty())
    ssc::messageManager()->sendWarning("No real time streaming data for writing to reconstruction file. (Computer clocks not synced?)");
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

  ssc::VideoRecorder::DataType::iterator it = mStreamRecordedData.begin();
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
std::vector<vtkImageDataPtr> UsReconstructionFileMaker::getFrames()
{
	std::vector<vtkImageDataPtr> retval;

  bool bw = settings()->value("Ultrasound/8bitAcquisitionData").toBool();

//  int i=0;
  for(ssc::VideoRecorder::DataType::iterator it = mStreamRecordedData.begin(); it != mStreamRecordedData.end(); ++it)
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

    input->Update();
    retval.push_back(input);
  }

  return retval;
}

/** Merge all us frames into one vtkImageData
 *
 */
vtkImageDataPtr UsReconstructionFileMaker::mergeFrames(std::vector<vtkImageDataPtr> input)
{
  vtkImageAppendPtr filter = vtkImageAppendPtr::New();
  filter->SetAppendAxis(2); // append along z-axis

  for (unsigned i=0; i<input.size(); ++i)
    filter->SetInput(i, input[i]);

  filter->Update();
  return filter->GetOutput();
}

/**write us images to disk.
 *
 * The images are handled as an array of 2D frames, but written into
 * one 3D image mhd file. Due to memory limitations (one large mem block
 * causes bit trouble), this is done by writing a single frame, and then
 * appending the other frames manually, and then hacking the mhd file to
 * incorporate the correct dimensions.
 *
 */
bool UsReconstructionFileMaker::writeUSImages(QString reconstructionFolder, QString calibrationFile)
{
	ssc::messageManager()->sendInfo("USAcq Start write");

  QString baseMhdFilename = this->getMhdFilename(reconstructionFolder);
  std::vector<vtkImageDataPtr> frames = this->getFrames();

  if (frames.size()<2)
  	return true;

  vtkImageDataPtr firstPair = this->mergeFrames(std::vector<vtkImageDataPtr>(frames.begin(), frames.begin()+2));

	QString mhdName = baseMhdFilename;

  vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
  writer->SetInput(firstPair);
  writer->SetFileName(cstring_cast(mhdName));
  writer->SetCompression(false);
  writer->Write();

	//mhd - custom fields
	QFile mhdFile(mhdName);
	if(!mhdFile.open(QIODevice::ReadWrite))
	{
		ssc::messageManager()->sendError("Cannot open "+mhdFile.fileName());
		return false;
	}

	QString fileData(mhdFile.readAll());
	fileData += "\n";
	fileData += QString("ConfigurationID = %1\n").arg(mTool->getProbe()->getConfigurationPath());
	fileData += QString("ProbeCalibration = %1\n").arg(calibrationFile);
	QStringList fileLines = fileData.split("\n");
	for (int j=0; j<fileLines.size(); ++j)
	{
		if (fileLines[j].startsWith("DimSize"))
		{
		  QStringList dimline = fileLines[j].split(" ");
		  dimline.back() = QString(" %1").arg(frames.size());;
		  fileLines[j] = dimline.join(" ");
		}
	}

	fileData = fileLines.join("\n");

	mhdFile.resize(0);
	mhdFile.write(fileData.toAscii());
	mhdFile.close();

	QString rawFileName = QString(mhdName).replace(".mhd", ".raw");

	QFile rawFile(rawFileName);
	if(!rawFile.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		ssc::messageManager()->sendError("Cannot open "+rawFile.fileName());
		return false;
	}

	// write all frames except the first two.
	for (unsigned i=2; i<frames.size(); ++i)
	{
		const char* ptr = reinterpret_cast<const char*>(frames[i]->GetScalarPointer());
//		frames[i]->GetScalarSize();
		unsigned N = frames[i]->GetDimensions()[0] * frames[i]->GetDimensions()[1] * frames[i]->GetScalarSize() * frames[i]->GetNumberOfScalarComponents();
		rawFile.write(ptr, N);
	}

  QFileInfo mhdInfo(mhdFile);
  mReport << mhdInfo.fileName()+", "+qstring_cast(mhdInfo.size())+" bytes";

  QFileInfo info(rawFile);
  mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(mStreamRecordedData.size())+" frames.";

//  writer = NULL; // ensure file is closed (might not be necessary)
	ssc::messageManager()->sendInfo(QString("completed write of %1 frames").arg( frames.size() ));
  return true;;

}

QString UsReconstructionFileMaker::copyCalibrationFile(QString reconstructionFolder)
{
  QString calibFileName = mCalibFilename;
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
