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

UsReconstructionFileMaker::UsReconstructionFileMaker(QString sessionDescription, QString activepatientPath) :
    mSessionDescription(sessionDescription),
    mActivepatientPath(activepatientPath),
    mDeleteFilesOnRelease(false)
{
	mFolderName = this->findFolderName(mActivepatientPath, mSessionDescription);
}

UsReconstructionFileMaker::~UsReconstructionFileMaker()
{
	if (mDeleteFilesOnRelease)
	{
		SavingVideoRecorder::deleteFolder(mFolderName);
	}
}

void UsReconstructionFileMaker::setData(ssc::TimedTransformMap trackerRecordedData,
                                        SavingVideoRecorderPtr videoRecorder,
                                        ssc::ToolPtr tool,
                                        QString calibFilename,
                                        bool writeColor)
{
	mVideoRecorder = videoRecorder;
	mReconstructData = this->getReconstructData(trackerRecordedData,
	                                            tool,
	                                            calibFilename,
	                                            writeColor);
}

void UsReconstructionFileMaker::setDeleteFilesOnRelease(bool on)
{
	mDeleteFilesOnRelease = on;
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
                                                                          ssc::ToolPtr tool,
                                                                          QString calibFilename,
                                                                          bool writeColor)
{
	if(trackerRecordedData.empty())
		ssc::messageManager()->sendWarning("No tracking data for writing to reconstruction file.");
	if(mFolderName.isEmpty())
		ssc::messageManager()->sendWarning("Active patient folder given to reconstruction file maker is empty.");

	ssc::USReconstructInputData retval;

	retval.mFilename = this->getMhdFilename(mFolderName);
	retval.mUsRaw = ssc::USFrameData::create(retval.mFilename, mVideoRecorder->getImageData());


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

QString UsReconstructionFileMaker::write(ssc::USReconstructInputData data)
{
	ssc::TimeKeeper timer;
	QString reconstructionFolder = QFileInfo(data.mFilename).absolutePath();
	QDir dir;
	dir.mkpath(reconstructionFolder);
	dir.cd(reconstructionFolder);
	mReport << "Made reconstruction folder: " + dir.absolutePath();
	QString session = mSessionDescription;

	this->writeTrackerTimestamps2(reconstructionFolder, session, data.mPositions);
	this->writeTrackerTransforms2(reconstructionFolder, session, data.mPositions);
	this->writeProbeConfiguration2(reconstructionFolder, session, data.mProbeData.mData, data.mProbeUid);

	mVideoRecorder->completeSave();

	this->report();
	mReport.clear();
	timer.printElapsedms("UsReconstructionFileMaker::write");

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

QString UsReconstructionFileMaker::write()
{
	this->write(this->getReconstructData());
	return mFolderName;
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

void UsReconstructionFileMaker::report()
{
	foreach(QString string, mReport)
	{
		ssc::messageManager()->sendSuccess(string, true);
	}
}

void UsReconstructionFileMaker::writeUSImages(QString path, std::vector<QString> images, bool compression)
{
	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();

	for (unsigned i=0; i<images.size(); ++i)
	{
		vtkImageDataPtr currentImage = ssc::MetaImageReader().load(images[i]);
		QString filename = path + "/" + QFileInfo(images[i]).fileName();

		writer->SetInput(currentImage);
		writer->SetFileName(cstring_cast(filename));
		writer->SetCompression(compression);
		writer->Write();
	}
}

QString UsReconstructionFileMaker::writeToNewFolder(QString activepatientPath, bool compression)
{
	ssc::TimeKeeper timer;

	QString path = this->findFolderName(activepatientPath, mSessionDescription);

	mReport.clear();
	mReport << "Made reconstruction folder: " + path;
	QString session = mSessionDescription;

	this->writeTrackerTimestamps2(path, session, mReconstructData.mPositions);
	this->writeTrackerTransforms2(path, session, mReconstructData.mPositions);
	this->writeUSTimestamps2(path, session, mReconstructData.mFrames);
	this->writeProbeConfiguration2(path, session, mReconstructData.mProbeData.mData, mReconstructData.mProbeUid);

	this->writeUSImages(path, mVideoRecorder->getImageData(), compression);

	this->report();
	mReport.clear();
	timer.printElapsedms("UsReconstructionFileMaker::write new");

	return path;
}



}//namespace cx
