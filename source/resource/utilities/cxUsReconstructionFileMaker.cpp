#include "cxUsReconstructionFileMaker.h"

#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>
#include "vtkImageAppend.h"
#include "vtkMetaImageWriter.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
#include "sscXmlOptionItem.h"
#include "sscTimeKeeper.h"
#include "sscDataReaderWriter.h"
#include "sscUSFrameData.h"
#include "cxSavingVideoRecorder.h"
#include "cxImageDataContainer.h"
#include "cxUSReconstructInputDataAlgoritms.h"
#include "sscCustomMetaImage.h"
#include "sscLogger.h"

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
ssc::USReconstructInputData UsReconstructionFileMaker::getReconstructData(ImageDataContainerPtr imageData,
                                                                          std::vector<double> imageTimestamps,
                                                                          ssc::TimedTransformMap trackerRecordedData,
                                                                          ssc::ToolPtr tool,
                                                                          bool writeColor, ssc::Transform3D rMpr)
{
	if(trackerRecordedData.empty())
		ssc::messageManager()->sendWarning("No tracking data for writing to reconstruction file.");

	ssc::USReconstructInputData retval;

	retval.mFilename = mSessionDescription; // not saved yet - no filename
	retval.mUsRaw = ssc::USFrameData::create(mSessionDescription, imageData);
	retval.rMpr = rMpr;

	for (ssc::TimedTransformMap::iterator it = trackerRecordedData.begin(); it != trackerRecordedData.end(); ++it)
	{
		ssc::TimedPosition current;
		current.mTime = it->first;
		current.mPos = it->second;
		retval.mPositions.push_back(current);
	}

	std::vector<double> fts = imageTimestamps;
	for (unsigned i=0; i<fts.size(); ++i)
	{
		ssc::TimedPosition current;
		current.mTime = fts[i];
		current.mPos = ssc::Transform3D::Identity();
		// current.mPos = not written - will be found from track positions during reconstruction.
		retval.mFrames.push_back(current);
	}

	if (tool && tool->getProbe())
	{
		retval.mProbeData.setData(tool->getProbe()->getProbeData());
	}

	vtkImageDataPtr mask = retval.mProbeData.getMask();
	if (mask)
	{
		retval.mMask = ssc::ImagePtr(new ssc::Image("mask", mask, "mask")) ;
	}
	if (tool)
		retval.mProbeUid = tool->getUid();

	this->fillFramePositions(&retval);

	return retval;
}

/** Use tool positions to generate positions for each frame, then convert the
  * position format from prMt to rMu. This makes is possible to use the frames
  * externally.
  */
void UsReconstructionFileMaker::fillFramePositions(ssc::USReconstructInputData* data) const
{
	cx::USReconstructInputDataAlgorithm::interpolateFramePositionsFromTracking(data);
	cx::USReconstructInputDataAlgorithm::transformFramePositionsTo_rMu(data);
}

bool UsReconstructionFileMaker::writeTrackerTimestamps(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts)
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

bool UsReconstructionFileMaker::writeUSTransforms(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts)
{
	return this->writeTransforms(reconstructionFolder+"/"+session+".fp", ts, "frame transforms rMu");
}

bool UsReconstructionFileMaker::writeTrackerTransforms(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts)
{
	return this->writeTransforms(reconstructionFolder+"/"+session+".tp", ts, "tracking transforms prMt");
}

bool UsReconstructionFileMaker::writeTransforms(QString filename, std::vector<ssc::TimedPosition> ts, QString type)
{
	bool success = false;
	QFile file(filename);
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
	mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(ts.size())+" " + type + ".";

	return success;
}

bool UsReconstructionFileMaker::writeUSTimestamps(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts)
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
void UsReconstructionFileMaker::writeProbeConfiguration(QString reconstructionFolder, QString session, ssc::ProbeData data, QString uid)
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

QString UsReconstructionFileMaker::createFolder(QString patientFolder, QString sessionDescription)
{
	QString retval("");
	QDir patientDir(patientFolder + "/US_Acq");

	QString subfolder = sessionDescription;
	QString subfolderAbsolutePath = patientDir.absolutePath()+"/"+subfolder;
	QString newPathName = subfolderAbsolutePath;
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

void UsReconstructionFileMaker::writeUSImages(QString path, ImageDataContainerPtr images, bool compression, std::vector<ssc::TimedPosition> pos)
{
	SSC_ASSERT(images->size()==pos.size());
	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();

	for (unsigned i=0; i<images->size(); ++i)
	{
		vtkImageDataPtr currentImage = images->get(i);
		QString filename = QString("%1/%2_%3.mhd").arg(path).arg(mSessionDescription).arg(i);

		writer->SetInput(currentImage);
		writer->SetFileName(cstring_cast(filename));
		writer->SetCompression(compression);
		{
			ssc::StaticMutexVtkLocker lock;
			writer->Write();
		}

		ssc::CustomMetaImagePtr customReader = ssc::CustomMetaImage::create(filename);
		customReader->setTransform(pos[i].mPos);
		customReader->setModality("US");
		customReader->setImageType(mSessionDescription);
	}
}

void UsReconstructionFileMaker::writeMask(QString path, QString session, ssc::ImagePtr mask)
{
	QString filename = QString("%1/%2.mask.mhd").arg(path).arg(session);
	if (!mask)
	{
		ssc::messageManager()->sendWarning(QString("No mask found, ignoring write to %1").arg(filename));
		return;
	}

	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
	writer->SetInput(mask->getBaseVtkImageData());
	writer->SetFileName(cstring_cast(filename));
	writer->SetCompression(false);
	writer->Write();
}


void UsReconstructionFileMaker::writeREADMEFile(QString reconstructionFolder, QString session)
{
	QString text = ""
"* ==== Format description												\n"
"*																		\n"
"* All files describing one acquisition lie the same folder.  The files all have		\n"
"* the name format US-Acq_<index>_<TS><stream>.<type>,					\n"
"* where																\n"
"*  - <index> is a running index, for convenience.						\n"
"*  - <TS> is a timestamp												\n"
"*  - <stream> is the uid of the video stream. Not used prior to cx3.5.0.				\n"
"*  - <type> is the format of that specific file.										\n"
"*																		\n"
"* Together, the files contains information about the us images and their				\n"
"* timestamps, the tracking positions and their timestamps, and the probe				\n"
"* calibration.															\n"
"*																		\n"
"* In the following, we use <filebase> = US-Acq_<index>_<TS><stream>.	\n"
"*																		\n"
"*																		\n"
"* ==== <filebase>.mhd (obsolete)										\n"
"*																		\n"
"* Used prior to version cx3.4.0.										\n"
"* A file in the metaheader file format containing the uncompressed image data.			\n"
"* the z-direction is the time axis, i.e. the z dim is the number of us frames.			\n"
"* See http://www.itk.org/Wiki/MetaIO/Documentation for more.							\n"
"*																		\n"
"* Two extra tags are added:											\n"
"*																		\n"
"ConfigurationID = <path:inside:ProbeCalibConfigs.xml>					\n"
"ProbeCalibration = <not used>											\n"
"*																		\n"
"* The ConfigurationID refers to a specific configuration within		\n"
"* ProbeCalibConfigs.xml, using colon separators.						\n"
"*																		\n"
"* ==== <filebase>_<index>.mhd											\n"
"*																		\n"
"* A sequence of files in the metaheader file format containing the image data, one file\n"
"* for each frame. The frame index is given by the index in the file name.				\n"
"* See http://www.itk.org/Wiki/MetaIO/Documentation for more.							\n"
"* Replaces single image files.											\n"
"*																		\n"
"* ==== ProbeCalibConfigs.xml (obsolete)								\n"
"*																		\n"
"* This file contains the probe definition, and is copied from the		\n"
"* config/tool/Tools folder												\n"
"*																		\n"
"* ==== <filebase>.probedata.xml										\n"
"*																		\n"
"* This file contains the probe definition. Replaces ProbeCalibConfigs.xml.				\n"
"*																		\n"
"* ==== <filebase>.fts													\n"
"*																		\n"
"* This file contains the frame timestamps. This is a sequence of						\n"
"* newline-separated floating-point numbers in milliceconds. The starting point			\n"
"* is irrelevant. The number of timestamps must equal the number of us frames.			\n"
"																		\n"
"*																		\n"
"* ==== <filebase>.tp													\n"
"*																		\n"
"* This file contains the tracking positions. This is a newline-separated				\n"
"* sequence of matrices, one for each tracking sample. Each matrix is the prMt,			\n"
"* i.e. the transform from tool to patient reference.					\n"
"* The last line of the matrix (always containing 0 0 0 1) is omitted. The matrix		\n"
"* numbers is whitespace-separated with newline between rows. Thus the number of		\n"
"* lines in this file is (# tracking positions) x 3.									\n"
"*																		\n"
"*																		\n"
"* ==== <filebase>.tts													\n"
"*																		\n"
"* This file contains the tracking timestamps. The format equals .fts ,					\n"
"* but the number of timestamps equals the number of tracking positions.				\n"
"*																		\n"
"* ==== <filebase>.fp													\n"
"*																		\n"
"* This file contains the frame positions. This is a newline-separated					\n"
"* sequence of matrices, one for each US frame. Each matrix is the rMu,					\n"
"* i.e. the transform from lower-left centered image space to							\n"
"* global reference.													\n"
"* The last line of the matrix (always containing 0 0 0 1) is omitted. The matrix		\n"
"* numbers is whitespace-separated with newline between rows. Thus the number of		\n"
"* lines in this file is (# tracking positions) x 3.									\n"
"*																		\n"
"* ==== <filebase>.mask.mhd												\n"
"*																		\n"
"* This file contains the image mask. The binary image shows what parts	\n"
"* of the frame images contain valid US data. This file is only written,\n"
"* not read. It can be constructed from the probe data.					\n"
"*																		\n"
"*																		\n"
"*/																		\n";

	QFile file(reconstructionFolder+"/"+session+".README.txt");
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		ssc::messageManager()->sendError("Cannot open "+file.fileName());
		return;
	}
	QTextStream stream(&file);
	stream << text;
	file.close();
}

QString UsReconstructionFileMaker::writeToNewFolder(QString path, bool compression)
{
	ssc::TimeKeeper timer;
	mReconstructData.mFilename = path+"/"+mSessionDescription+".fts"; // use fts since this is a single unique file.

	mReport.clear();
	mReport << "Made reconstruction folder: " + path;
	QString session = mSessionDescription;

	this->writeTrackerTimestamps(path, session, mReconstructData.mPositions);
	this->writeTrackerTransforms(path, session, mReconstructData.mPositions);
	this->writeUSTimestamps(path, session, mReconstructData.mFrames);
	this->writeUSTransforms(path, session, mReconstructData.mFrames);
	this->writeProbeConfiguration(path, session, mReconstructData.mProbeData.mData, mReconstructData.mProbeUid);
	this->writeMask(path, session, mReconstructData.mMask);
	this->writeREADMEFile(path, session);

	ImageDataContainerPtr imageData = mReconstructData.mUsRaw->getImageContainer();
	if (imageData)
		this->writeUSImages(path, imageData, compression, mReconstructData.mFrames);
	else
		mReport << "failed to find frame data, save failed.";

	int time = std::max(1, timer.getElapsedms());
	mReport << QString("Completed save to %1. Spent %2s, %3fps").arg(mSessionDescription).arg(time/1000).arg(imageData->size()*1000/time);

	this->report();
	mReport.clear();

	return mReconstructData.mFilename;
}



}//namespace cx
