/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxUsReconstructionFileMaker.h"

#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>
#include "vtkImageAppend.h"
#include "vtkMetaImageWriter.h"
#include <vtkPolyData.h>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxSettings.h"
#include "cxXmlOptionItem.h"
#include "cxTimeKeeper.h"
#include "cxUSFrameData.h"
#include "cxSavingVideoRecorder.h"
#include "cxImageDataContainer.h"
#include "cxUSReconstructInputDataAlgoritms.h"
#include "cxCustomMetaImage.h"
#include "cxErrorObserver.h"


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

USReconstructInputData UsReconstructionFileMaker::getReconstructData()
{
	return mReconstructData;
}

/**Create and return the structure that would have been read by UsReconstructFileReader,
 * if written from this object.
 *
 */
USReconstructInputData UsReconstructionFileMaker::getReconstructData(ImageDataContainerPtr imageData,
																	 std::vector<TimeInfo> imageTimestamps,
																	 TimedTransformMap trackerRecordedData,
																	 std::map<double, ToolPositionMetadata> trackerRecordedMetadata,
																	 std::map<double, ToolPositionMetadata> referenceRecordedMetadata,
																	 ToolPtr tool, QString streamUid,
																	 bool writeColor, Transform3D rMpr,
																	 ToolPtr reference)
{
	if(trackerRecordedData.empty())
		reportWarning("No tracking data for writing to reconstruction file.");

	USReconstructInputData retval;

	retval.mFilename = mSessionDescription; // not saved yet - no filename
	retval.mUsRaw = USFrameData::create(mSessionDescription, imageData);
	retval.rMpr = rMpr;
	retval.mTrackerRecordedMetadata = trackerRecordedMetadata;
	retval.mReferenceRecordedMetadata = referenceRecordedMetadata;

	for (TimedTransformMap::iterator it = trackerRecordedData.begin(); it != trackerRecordedData.end(); ++it)
	{
		TimedPosition current;
		current.mTime = it->first;
		current.mTimeInfo.setAcquisitionTime(it->first);
		current.mPos = it->second;
		retval.mPositions.push_back(current);
	}

	std::vector<TimeInfo> fts = imageTimestamps;
	for (unsigned i=0; i<fts.size(); ++i)
	{
		TimedPosition current;
		current.mTimeInfo = fts[i];
		current.mTime = current.mTimeInfo.getAcquisitionTime();
		current.mPos = Transform3D::Identity();
		// current.mPos = not written - will be found from track positions during reconstruction.
		retval.mFrames.push_back(current);
	}

	if (tool && tool->getProbe())
	{
		retval.mProbeDefinition.setData(tool->getProbe()->getProbeDefinition(streamUid));
	}

	if (tool)
		retval.mProbeUid = tool->getUid();
	retval.mRecordingInfo = createRecordingInfo(tool, reference, rMpr);

	this->fillFramePositions(&retval);

	return retval;
}

USRecordingInfo UsReconstructionFileMaker::createRecordingInfo(ToolPtr tool, ToolPtr reference, Transform3D rMpr)
{
	USRecordingInfo retval;
	retval.rMpr = rMpr;
	if (tool)
	{
		retval.mProbeOpenIGTLinkTransformId = tool->getOpenIGTLinkTransformId();
		retval.mProbeOpenIGTLinkImageId = tool->getOpenIGTLinkImageId();
		retval.mProbe_sMt = tool->getCalibration_sMt();
	}
	if (reference)
	{
		retval.mReferenceUid = reference->getUid();
		retval.mReferenceOpenIGTLinkTransformId = reference->getOpenIGTLinkTransformId();
	}
	return retval;
}

/** Use tool positions to generate positions for each frame, then convert the
  * position format from prMt to rMu. This makes is possible to use the frames
  * externally.
  */
void UsReconstructionFileMaker::fillFramePositions(USReconstructInputData* data) const
{
	cx::USReconstructInputDataAlgorithm::interpolateFramePositionsFromTracking(data);
	cx::USReconstructInputDataAlgorithm::transformFramePositionsTo_rMu(data);
}

bool UsReconstructionFileMaker::writeTrackerTimestamps(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts)
{
	return this->writeTimestamps(reconstructionFolder+"/"+session+".tts",
								 ts, "tracking timestamps");
}

bool UsReconstructionFileMaker::writeUSTimestamps(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts)
{
	bool retval = true;

	retval = this->writeTimestamps(reconstructionFolder+"/"+session+".fts",
								 ts, "frame timestamps", Modified);

	retval &= this->writeTimestamps(reconstructionFolder+"/"+session+".scanner.frameTimestamps",
								 ts, "frame scanner timestamps", Scanner);

	retval &= this->writeTimestamps(reconstructionFolder+"/"+session+".softwareArrive.frameTimestamps",
								 ts, "frame arrive in software timestamps", SoftwareArrive);
	return retval;
}

bool UsReconstructionFileMaker::writeTimestamps(QString filename, std::vector<TimedPosition> ts, QString type, TimeStampType timeStampType)
{
	bool success = false;

	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		reportError("Cannot open "+file.fileName());
		return success;
	}
	QTextStream stream(&file);

	for (unsigned i=0; i<ts.size(); ++i)
	{
		switch(timeStampType)
		{
		case Modified:
			stream << qstring_cast(ts[i].mTimeInfo.getAcquisitionTime());
			break;
		case SoftwareArrive:
			stream << qstring_cast(ts[i].mTimeInfo.getSoftwareAcquisitionTime());
			break;
		case Scanner:
			stream << qstring_cast(ts[i].mTimeInfo.getScannerAcquisitionTime());
			break;
		default:
			stream << qstring_cast(ts[i].mTime); // Is the same as mTimeInfo.getAcquisitionTime()
			break;
		}
		stream << "\n";
	}
	file.close();
	success = true;

	QFileInfo info(file);
	mReport << QString("%1, %2 bytes, %3 %4.")
			   .arg(info.fileName())
			   .arg(info.size())
			   .arg(ts.size())
			   .arg(type);
//	mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(ts.size())+" frame timestamps.";

	return success;
}

bool UsReconstructionFileMaker::writeUSTransforms(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts)
{
	return this->writeTransforms(reconstructionFolder+"/"+session+".fp", ts, "frame transforms rMu");
}

bool UsReconstructionFileMaker::writeTrackerMetadata(QString reconstructionFolder, QString session, const std::map<double, ToolPositionMetadata>& ts)
{
	QString filename = reconstructionFolder+"/"+session+".probe.toolmeta";
	return this->writeMetadata(filename, ts, "tracker metadata");
}

bool UsReconstructionFileMaker::writeReferenceMetadata(QString reconstructionFolder, QString session, const std::map<double, ToolPositionMetadata>& ts)
{
	QString filename = reconstructionFolder+"/"+session+".ref.toolmeta";
	return this->writeMetadata(filename, ts, "tracker metadata");
}

bool UsReconstructionFileMaker::writeMetadata(QString filename, const std::map<double, ToolPositionMetadata>& ts, QString type)
{
	bool success = false;
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		reportError("Cannot open "+file.fileName());
		return success;
	}
	QTextStream stream(&file);

	for (std::map<double, ToolPositionMetadata>::const_iterator i=ts.begin(); i!=ts.end(); ++i)
	{
		stream << i->second.toString() << "\n";
	}
	file.close();
	success = true;

	QFileInfo info(file);
	mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(ts.size())+" " + type + ".";

	return success;
}

bool UsReconstructionFileMaker::writeTrackerTransforms(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts)
{
	return this->writeTransforms(reconstructionFolder+"/"+session+".tp", ts, "tracking transforms prMt");
}

bool UsReconstructionFileMaker::writeTransforms(QString filename, std::vector<TimedPosition> ts, QString type)
{
	bool success = false;
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		reportError("Cannot open "+file.fileName());
		return success;
	}
	QTextStream stream(&file);

	for (unsigned i=0; i<ts.size(); ++i)
	{
		Transform3D transform = ts[i].mPos;
		stream << transform(0,0) << " ";
		stream << transform(0,1) << " ";
		stream << transform(0,2) << " ";
		stream << transform(0,3);
		stream << "\n";
		stream << transform(1,0) << " ";
		stream << transform(1,1) << " ";
		stream << transform(1,2) << " ";
		stream << transform(1,3);
		stream << "\n";
		stream << transform(2,0) << " ";
		stream << transform(2,1) << " ";
		stream << transform(2,2) << " ";
		stream << transform(2,3);
		stream << "\n";
	}
	file.close();
	success = true;

	QFileInfo info(file);
	mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(ts.size())+" " + type + ".";

	return success;
}

bool UsReconstructionFileMaker::writeMatrix(QString filename, Transform3D matrix, QString type)
{
	bool success = false;
	QFile file(filename);
	if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream stream(&file);
		stream.setRealNumberPrecision(12);
		for (int row=0; row<4; ++row)
		{
			stream << matrix(row,0) << " " << matrix(row,1) << " " << matrix(row,2) << " " << matrix(row,3) << "\n";
		}
		stream.flush();
		success = (stream.status() == QTextStream::Ok) && (file.error() == QFileDevice::NoError);
		file.close();
	}
	if (success)
	{
		QFileInfo info(file);
		mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, " + type + ".";
	}
	else
	{
		reportError("Cannot write "+file.fileName());
	}
	return success;
}

/**
 * Write probe configuration to file. This works even for configs not saved to the ProbeCalibConfigs.xml file.
 */
void UsReconstructionFileMaker::writeProbeConfiguration(QString reconstructionFolder, QString session, const USReconstructInputData& data)
{
	const USRecordingInfo& info = data.mRecordingInfo;
	XmlOptionFile file = XmlOptionFile(reconstructionFolder + "/" + session + ".probedata.xml");
	data.mProbeDefinition.mData.addXml(file.getElement("configuration"));
	QDomElement tool = file.getElement("tool");
	tool.setAttribute("toolID", data.mProbeUid);
	tool.setAttribute("openigtlinktransformid", info.mProbeOpenIGTLinkTransformId);
	tool.setAttribute("openigtlinkimageid", info.mProbeOpenIGTLinkImageId);
	if (!info.mReferenceUid.isEmpty())
	{
		QDomElement reference = file.getElement("reference");
		reference.setAttribute("toolID", info.mReferenceUid);
		reference.setAttribute("openigtlinktransformid", info.mReferenceOpenIGTLinkTransformId);
	}
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
		reportSuccess(string);
	}
}

unsigned UsReconstructionFileMaker::writeUSImages(const QString& path, ImageDataContainerPtr images, bool compression, const std::vector<TimedPosition>& pos)
{
	if (images->size() != pos.size())
	{
		reportError(QString("Mismatch between %1 images and %2 frame positions in %3, only images with a position are saved.")
					.arg(images->size()).arg(pos.size()).arg(mSessionDescription));
	}
	unsigned count = std::min<unsigned>(images->size(), pos.size());
	unsigned written = 0;
	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
	writer->SetCompression(compression);

	for (unsigned i=0; i<count; ++i)
	{
		vtkImageDataPtr currentImage = images->get(i);
		if (currentImage)
		{
			QString filename = QString("%1/%2_%3.mhd").arg(path).arg(mSessionDescription).arg(i);
			this->writeUSImage(writer, filename, currentImage, pos[i].mPos);
			++written;
		}
	}

	if (written < count)
	{
		reportError(QString("Failed to read %1 of %2 images in %3 from the recording cache, the saved recording is incomplete.")
					.arg(count-written).arg(count).arg(mSessionDescription));
	}
	return written;
}

void UsReconstructionFileMaker::writeUSImage(vtkMetaImageWriterPtr writer, const QString& filename, vtkImageDataPtr image, const Transform3D& rMu)
{
	writer->SetInputData(image);
	writer->SetFileName(cstring_cast(filename));
	{
		StaticMutexVtkLocker lock;
		writer->Write();
	}

	CustomMetaImagePtr customReader = CustomMetaImage::create(filename);
	customReader->setTransform(rMu);
	customReader->setModality(imUS);
	customReader->setImageType(convertToImageSubType(mSessionDescription));
}

void UsReconstructionFileMaker::writeMask(QString path, QString session, vtkImageDataPtr mask)
{
	QString filename = QString("%1/%2.mask.mhd").arg(path).arg(session);
	if (!mask)
	{
		reportWarning(QString("No mask found, ignoring write to %1").arg(filename));
		return;
	}

	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
	writer->SetInputData(mask);
	writer->SetFileName(cstring_cast(filename));
	writer->SetCompression(false);
	writer->Write();
}


void UsReconstructionFileMaker::writeREADMEFile(QString reconstructionFolder, QString session)
{
	QString text = ""
"* ==== Format description												\n"
"* Refer to 																		    \n"
"*		https://custusx.pages.sintef.no/CustusX/developer_doc/org_custusx_acquisition.html   \n"
"* for a description of the files.                                                      \n"
"*/																		\n";

	QFile file(reconstructionFolder+"/"+session+".README.txt");
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		reportError("Cannot open "+file.fileName());
		return;
	}
	QTextStream stream(&file);
	stream << text;
	file.close();
}

QString UsReconstructionFileMaker::writeToNewFolder(QString path, bool compression)
{
	TimeKeeper timer;
	mReconstructData.mFilename = path+"/"+mSessionDescription+".fts"; // use fts since this is a single unique file.

	mReport.clear();
	mReport << "Made reconstruction folder: " + path;
	QString session = mSessionDescription;

	if (!mReconstructData.mTrackerRecordedMetadata.empty())
	{
		this->writeTrackerMetadata(path, session, mReconstructData.mTrackerRecordedMetadata);
	}
	if (!mReconstructData.mReferenceRecordedMetadata.empty())
	{
		this->writeReferenceMetadata(path, session, mReconstructData.mReferenceRecordedMetadata);
	}
	this->writeTrackerTimestamps(path, session, mReconstructData.mPositions);
	this->writeTrackerTransforms(path, session, mReconstructData.mPositions);
	this->writeUSTimestamps(path, session, mReconstructData.mFrames);
	this->writeUSTransforms(path, session, mReconstructData.mFrames);
	this->writeProbeConfiguration(path, session, mReconstructData);
	if (!mReconstructData.mProbeUid.isEmpty())
	{
		this->writeMatrix(path+"/"+session+".probe.cal", mReconstructData.mRecordingInfo.mProbe_sMt, "probe calibration sMt");
	}
	this->writeMatrix(path+"/"+session+".rMpr", mReconstructData.mRecordingInfo.rMpr, "patient registration rMpr");
	this->writeMask(path, session, mReconstructData.getMask());
	this->writeREADMEFile(path, session);

	ImageDataContainerPtr imageData;
	if (mReconstructData.mUsRaw)
	{
		imageData = mReconstructData.mUsRaw->getImageContainer();
	}
	unsigned imageCount = 0;
	if (imageData)
	{
		imageCount = this->writeUSImages(path, imageData, compression, mReconstructData.mFrames);
	}
	else
	{
		mReport << "failed to find frame data, save failed.";
	}

	int time = std::max(1, timer.getElapsedms());
	mReport << QString("Completed save to %1. Spent %2s, %3fps").arg(mSessionDescription).arg(time/1000).arg(imageCount*1000/time);

	this->report();
	mReport.clear();

	return mReconstructData.mFilename;
}



}//namespace cx
