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
#include "sscXmlOptionItem.h"

typedef vtkSmartPointer<vtkImageAppend> vtkImageAppendPtr;

namespace cx
{

UsReconstructionFileMaker::UsReconstructionFileMaker(QString sessionDescription, QString activepatientPath) :
    mSessionDescription(sessionDescription),
    mActivepatientPath(activepatientPath)
{
	mFolderName = this->findFolderName(mActivepatientPath, mSessionDescription);
//	mReconstructData = this->getReconstructData(trackerRecordedData,
//			streamRecordedData,
//			sessionDescription,
//			activepatientPath,
//			tool,
//			calibFilename,
//			writeColor);
}

UsReconstructionFileMaker::~UsReconstructionFileMaker()
{
}

void UsReconstructionFileMaker::setData(ssc::TimedTransformMap trackerRecordedData,
		SavingVideoRecorderPtr videoRecorder,
		ssc::ToolPtr tool,
		QString calibFilename,
		bool writeColor)
{
	mVideoRecorder = videoRecorder;
	mReconstructData = this->getReconstructData(trackerRecordedData,
//			videoRecorder->getRecording(),
//			sessionDescription,
//			activepatientPath,
			tool,
			calibFilename,
			writeColor);
}


ssc::USReconstructInputData UsReconstructionFileMaker::getReconstructData()
{
	return mReconstructData;
}


/**Create and return the structure that would have been read by UsReconstructFileReader,
 * if written from this object.
 *
 */
ssc::USReconstructInputData UsReconstructionFileMaker::getReconstructData(ssc::TimedTransformMap trackerRecordedData,
//		SavingVideoRecorder::DataType streamRecordedData,
//		QString sessionDescription,
//		QString activepatientPath,
		ssc::ToolPtr tool,
		QString calibFilename,
		bool writeColor)
{
	  if(trackerRecordedData.empty())
	    ssc::messageManager()->sendWarning("No tracking data for writing to reconstruction file.");
//	  if(streamRecordedData.empty())
//	    ssc::messageManager()->sendWarning("No real time streaming data for writing to reconstruction file. (Computer clocks not synced?)");
	  if(mFolderName.isEmpty())
	    ssc::messageManager()->sendWarning("Active patient folder given to reconstruction file maker is empty.");

	ssc::USReconstructInputData retval;

//	QString reconstructionFolder = this->findFolderName(activepatientPath, sessionDescription);
	retval.mFilename = this->getMhdFilename(mFolderName);
	retval.mUsRaw = ssc::USFrameData::create(retval.mFilename, mVideoRecorder->getImageData());

//	// create image data
////	std::vector<vtkImageDataPtr> frames = this->getFrames(streamRecordedData, writeColor);
//	if (streamRecordedData.size() >= 1)
//	{
////		std::vector<vtkImageDataPtr> frames;
////		for(ssc::VideoRecorder::DataType::iterator it = streamRecordedData.begin(); it != streamRecordedData.end(); ++it)
////			frames.push_back(it->second);
////		std::vector<ssc::ImagePtr> images;
////		for (unsigned i=0; i<frames.size(); ++i)
////			images.push_back(ssc::ImagePtr(new ssc::Image(retval.mFilename+"_"+qstring_cast(i), frames[i])));
////		vtkImageDataPtr imageData = this->mergeFrames(frames);
////		ssc::ImagePtr image(new ssc::Image(retval.mFilename, imageData));
////		image->setFilePath(reconstructionFolder);
////		retval.mUsRaw.reset(new ssc::USFrameDataMonolithic(image));
//
////		retval.mUsRaw = ssc::USFrameData::create(frames, retval.mFilename);
////		std::vector<CachedImageDataPtr> frames = mVideoRecorder->getImageData();
////		retval.mUsRaw = ssc::USFrameData::create(mVideoRecorder->getImageData());
////		retval.mUsRaw = ssc::USFrameData::create(retval.mFilename, mVideoRecorder->getRecording().size(), mVideoRecorder->getRecording());
//	}

	for (ssc::TimedTransformMap::iterator it = trackerRecordedData.begin(); it != trackerRecordedData.end(); ++it)
	{
		ssc::TimedPosition current;
		current.mTime = it->first;
		current.mPos = it->second;
		retval.mPositions.push_back(current);
	}

	std::vector<double> fts = mVideoRecorder->getTimestamps();
	for (unsigned i=0; i<fts.size(); ++i)
	{
		ssc::TimedPosition current;
		current.mTime = fts[i];
		// current.mPos = not written - will be found from track positions during reconstruction.
		retval.mFrames.push_back(current);
	}

//	for (ssc::VideoRecorder::DataType::iterator it = streamRecordedData.begin(); it != streamRecordedData.end(); ++it)
//	{
//		ssc::TimedPosition current;
//		current.mTime = it->first;
//		// current.mPos = not written - will be found from track positions during reconstruction.
//		retval.mFrames.push_back(current);
//	}

	if (tool && tool->getProbe())
	{
		retval.mProbeData.setData(tool->getProbe()->getData());
	}

	vtkImageDataPtr mask = retval.mProbeData.getMask();
	retval.mMask = ssc::ImagePtr(new ssc::Image("mask", mask, "mask")) ;
	if (tool)
		retval.mProbeUid = tool->getUid();

	//test
//	QStringList path = retval.mFilename.split(".");
//	path[path.size()-2] += "_direct";
//	retval.mFilename = path.join(".");
//	this->write(retval);

	return retval;
}

QString UsReconstructionFileMaker::write(ssc::USReconstructInputData data)
{
	QString reconstructionFolder = QFileInfo(data.mFilename).absolutePath();
	QDir dir;
	dir.mkpath(reconstructionFolder);
	dir.cd(reconstructionFolder);
	mReport << "Made reconstruction folder: " + dir.absolutePath();
	QString session = mSessionDescription;

	this->writeTrackerTimestamps2(reconstructionFolder, session, data.mPositions);
	this->writeTrackerTransforms2(reconstructionFolder, session, data.mPositions);
//	this->writeUSTimestamps2(reconstructionFolder, session, data.mFrames);
//	QString calibrationFile = this->copyCalibrationFile(reconstructionFolder);
//	this->writeUSImages2(reconstructionFolder, data.mUsRaw, data.mFilename);
	//  this->copyProbeCalibConfigsXml(reconstructionFolder);
//	this->writeProbeConfiguration(reconstructionFolder);
	this->writeProbeConfiguration2(reconstructionFolder, session, data.mProbeData.mData, data.mProbeUid);

	mVideoRecorder->completeSave();

	this->report();
	mReport.clear();

	return reconstructionFolder;
}

bool UsReconstructionFileMaker::writeTrackerTimestamps2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts)
{
  bool success = false;

  QFile file(reconstructionFolder+"/"+session+".tts");
  if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    ssc::messageManager()->sendError("Cannot open "+file.fileName());
    return success;
  }
  QTextStream stream(&file);

  for (unsigned i=0; i<ts.size(); ++i)
  {
	    stream << qstring_cast(ts[i].mTime);
	    stream << endl;
  }

//  ssc::TimedTransformMap::iterator it = mTrackerRecordedData.begin();
//  for(; it != mTrackerRecordedData.end(); ++it)
//  {
//    stream << qstring_cast(it->first);
//    stream << endl;
//  }
  file.close();
  success = true;

  QFileInfo info(file);
  mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(ts.size())+" tracking timestamps.";

  return success;
}

bool UsReconstructionFileMaker::writeTrackerTransforms2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts)
{
  bool success = false;
  QFile file(reconstructionFolder+"/"+session+".tp");
  if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    ssc::messageManager()->sendError("Cannot open "+file.fileName());
    return success;
  }
  QTextStream stream(&file);

//  ssc::TimedTransformMap::iterator it = mTrackerRecordedData.begin();
//  for(; it != mTrackerRecordedData.end(); ++it)
  for (unsigned i=0; i<ts.size(); ++i)
  {
//    ssc::Transform3D transform = it->second;
	    ssc::Transform3D transform = ts[i].mPos;
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
  mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(ts.size())+" tracking transforms.";

  return success;
}

bool UsReconstructionFileMaker::writeUSTimestamps2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts)
{
	bool success = false;

    QFile file(reconstructionFolder+"/"+session+".fts");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
      ssc::messageManager()->sendError("Cannot open "+file.fileName());
      return success;
    }
    QTextStream stream(&file);

    //ssc::SavingVideoRecorder::DataType::iterator it = mStreamRecordedData.begin();
    //for(; it != mStreamRecordedData.end(); ++it)
    for (unsigned i=0; i<ts.size(); ++i)
    {
      stream << qstring_cast(ts[i].mTime);
      stream << endl;
    }
    file.close();
    success = true;

    QFileInfo info(file);
    mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(ts.size())+" frame timestamps.";

    return success;
}

///**write us images to disk.
// *
// * The images are handled as an array of 2D frames, but written into
// * one 3D image mhd file. Due to memory limitations (one large mem block
// * causes bit trouble), this is done by writing a single frame, and then
// * appending the other frames manually, and then hacking the mhd file to
// * incorporate the correct dimensions.
// *
// */
//bool UsReconstructionFileMaker::writeUSImages2(QString reconstructionFolder, ssc::USFrameDataPtr data, QString filename)
//{
//	if (!data)
//		return false;
//	data->save(filename, settings()->value("Ultrasound/CompressAcquisition", true).toBool());
//	return true;;
//}

/**
 * Write probe configuration to file. This works even for configs not saved to the ProbeCalibConfigs.xml file.
 */
void UsReconstructionFileMaker::writeProbeConfiguration2(QString reconstructionFolder, QString session, ssc::ProbeData data, QString uid)
{
	ssc::XmlOptionFile file = ssc::XmlOptionFile(reconstructionFolder + "/" + session + ".probedata.xml", "navnet");
	data.addXml(file.getElement("configuration"));
	file.getElement("tool").toElement().setAttribute("toolID", uid);
//	file.getElement("tool").toElement().setAttribute("configurationID", mTool->getProbe()->getConfigurationPath());
	file.save();
}

QString UsReconstructionFileMaker::write()
{
	this->write(this->getReconstructData());

	return mFolderName;
//	  QString reconstructionFolder = this->findFolderName(mActivepatientPath, mSessionDescription);
//	  return reconstructionFolder;
}

QString UsReconstructionFileMaker::getMhdFilename(QString reconstructionFolder)
{
  QString mhdFilename = reconstructionFolder+"/"+mSessionDescription+".mhd";
  return mhdFilename;
}

QString UsReconstructionFileMaker::findFolderName(QString patientFolder, QString sessionDescription)
{
  QString retval("");
  QDir patientDir(patientFolder + "/US_Acq");

  QString subfolder = sessionDescription;
  QString subfolderAbsolutePath = patientDir.absolutePath()+"/"+subfolder;
  QString newPathName = subfolderAbsolutePath;
  int i=1;
  while(!this->findNewSubfolder(newPathName))
  {
    newPathName = subfolderAbsolutePath+"_"+QString::number(i++);
  }
  patientDir.mkpath(newPathName);
  patientDir.cd(newPathName);

  retval = patientDir.absolutePath();
  return retval;
}

bool UsReconstructionFileMaker::findNewSubfolder(QString subfolderAbsolutePath)
{
  QDir dir;
  if(dir.exists(subfolderAbsolutePath))
    return false;

  return true;
}

///** Merge all us frames into one vtkImageData
// *
// */
//std::vector<vtkImageDataPtr> UsReconstructionFileMaker::getFrames(ssc::VideoRecorder::DataType streamRecordedData, bool writeColor)
//{
//	std::vector<vtkImageDataPtr> retval;

//  bool bw = settings()->value("Ultrasound/8bitAcquisitionData").toBool();

////  int i=0;
//  for(ssc::VideoRecorder::DataType::iterator it = streamRecordedData.begin(); it != streamRecordedData.end(); ++it)
//  {
//    vtkImageDataPtr input = it->second;
//    if (bw && !writeColor)
//    {
//      if (it->second->GetNumberOfScalarComponents()>2) // color
//      {
//        vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
//        luminance->SetInput(input);
//        input = luminance->GetOutput();
//      }
//    }

//    input->Update();
//    retval.push_back(input);
//  }

//  return retval;
//}

///** Merge all us frames into one vtkImageData
// *
// */
//vtkImageDataPtr UsReconstructionFileMaker::mergeFrames(std::vector<vtkImageDataPtr> input)
//{
//  vtkImageAppendPtr filter = vtkImageAppendPtr::New();
//  filter->SetAppendAxis(2); // append along z-axis

//  for (unsigned i=0; i<input.size(); ++i)
//    filter->SetInput(i, input[i]);

//  filter->Update();
//  return filter->GetOutput();
//}


void UsReconstructionFileMaker::report()
{
  foreach(QString string, mReport)
  {
    ssc::messageManager()->sendSuccess(string, true);
  }
}

}//namespace cx
