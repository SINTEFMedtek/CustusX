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
#include "sscTimeKeeper.h"
#include "sscDataManagerImpl.h"

typedef vtkSmartPointer<vtkImageAppend> vtkImageAppendPtr;

namespace cx
{

UsReconstructionFileMaker::UsReconstructionFileMaker(QString sessionDescription) :
    mSessionDescription(sessionDescription)
{
}

UsReconstructionFileMaker::~UsReconstructionFileMaker()
{
}

ssc::USReconstructInputData UsReconstructionFileMaker::getReconstructData()
{
	return mReconstructData;
}

/**Create and return the structure that would have been read by UsReconstructFileReader,
 * if written from this object.
 *
 */
ssc::USReconstructInputData UsReconstructionFileMaker::getReconstructData(SavingVideoRecorderPtr videoRecorder,
                                                                          ssc::TimedTransformMap trackerRecordedData,
                                                                          ssc::ToolPtr tool,
                                                                          bool writeColor)
{
	if(trackerRecordedData.empty())
		ssc::messageManager()->sendWarning("No tracking data for writing to reconstruction file.");

	ssc::USReconstructInputData retval;

	retval.mFilename = mSessionDescription; // not saved yet - no filename
	retval.mUsRaw = ssc::USFrameData::create(mSessionDescription, videoRecorder->getImageData());

	for (ssc::TimedTransformMap::iterator it = trackerRecordedData.begin(); it != trackerRecordedData.end(); ++it)
	{
		ssc::TimedPosition current;
		current.mTime = it->first;
		current.mPos = it->second;
		retval.mPositions.push_back(current);
	}

	std::vector<double> fts = videoRecorder->getTimestamps();
	for (unsigned i=0; i<fts.size(); ++i)
	{
		ssc::TimedPosition current;
		current.mTime = fts[i];
		// current.mPos = not written - will be found from track positions during reconstruction.
		retval.mFrames.push_back(current);
	}

	if (tool && tool->getProbe())
	{
		retval.mProbeData.setData(tool->getProbe()->getData());
	}

	vtkImageDataPtr mask = retval.mProbeData.getMask();
	retval.mMask = ssc::ImagePtr(new ssc::Image("mask", mask, "mask")) ;
	if (tool)
		retval.mProbeUid = tool->getUid();

	return retval;
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

	for (unsigned i=0; i<ts.size(); ++i)
	{
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

/**
 * Write probe configuration to file. This works even for configs not saved to the ProbeCalibConfigs.xml file.
 */
void UsReconstructionFileMaker::writeProbeConfiguration2(QString reconstructionFolder, QString session, ssc::ProbeData data, QString uid)
{
	ssc::XmlOptionFile file = ssc::XmlOptionFile(reconstructionFolder + "/" + session + ".probedata.xml", "navnet");
	data.addXml(file.getElement("configuration"));
	file.getElement("tool").toElement().setAttribute("toolID", uid);
	file.save();
}

QString UsReconstructionFileMaker::createUniqueFolder(QString patientFolder, QString sessionDescription)
{
	QString retval("");
	QDir patientDir(patientFolder + "/US_Acq");

	QString subfolder = sessionDescription;
	QString subfolderAbsolutePath = patientDir.absolutePath()+"/"+subfolder;
	QString newPathName = subfolderAbsolutePath;
	int i=1;
	while(!findNewSubfolder(newPathName))
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

void UsReconstructionFileMaker::report()
{
	foreach(QString string, mReport)
	{
		ssc::messageManager()->sendSuccess(string, true);
	}
}

void UsReconstructionFileMaker::writeUSImages(QString path, CachedImageDataContainerPtr images, bool compression)
{
	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();

	for (unsigned i=0; i<images->size(); ++i)
	{
		vtkImageDataPtr currentImage = images->get(i);
		QString filename = path + "/" + QFileInfo(images->getFilename(i)).fileName();

		writer->SetInput(currentImage);
		writer->SetFileName(cstring_cast(filename));
		writer->SetCompression(compression);
		writer->Write();
	}
}

QString UsReconstructionFileMaker::writeToNewFolder(QString path, bool compression)
{
	ssc::TimeKeeper timer;
	mReconstructData.mFilename = path+"/"+mSessionDescription+".fts"; // use fts since this is a single unique file.

	mReport.clear();
	mReport << "Made reconstruction folder: " + path;
	QString session = mSessionDescription;

	this->writeTrackerTimestamps2(path, session, mReconstructData.mPositions);
	this->writeTrackerTransforms2(path, session, mReconstructData.mPositions);
	this->writeUSTimestamps2(path, session, mReconstructData.mFrames);
	this->writeProbeConfiguration2(path, session, mReconstructData.mProbeData.mData, mReconstructData.mProbeUid);


	CachedImageDataContainerPtr imageData = boost::shared_dynamic_cast<CachedImageDataContainer>(mReconstructData.mUsRaw->getImageContainer());
	if (imageData)
		this->writeUSImages(path, imageData, compression);
	else
		mReport << "failed to find frame data, save failed.";

	int time = timer.getElapsedms();
	mReport << QString("Completed save to %1. Spent %2s, %3fps").arg(mSessionDescription).arg(time/1000).arg(imageData->size()*1000/time);

	this->report();
	mReport.clear();

	return mReconstructData.mFilename;
}



}//namespace cx
