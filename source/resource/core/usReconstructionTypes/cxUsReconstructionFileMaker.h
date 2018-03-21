/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXUSRECONSTRUCTIONFILEMAKER_H_
#define CXUSRECONSTRUCTIONFILEMAKER_H_

#include "cxResourceExport.h"

class QTextStream;
class QDir;

#include "cxTool.h"
#include "cxUSReconstructInputData.h"
#include "cxForwardDeclarations.h"


namespace cx
{
class TimeInfo;
typedef boost::shared_ptr<QTextStream> QTextStreamPtr;
typedef boost::shared_ptr<class ImageDataContainer> ImageDataContainerPtr;
//typedef boost::shared_ptr<class CachedImageDataContainer> CachedImageDataContainerPtr;
typedef boost::shared_ptr<class SavingVideoRecorder> SavingVideoRecorderPtr;


/**
* \file
* \addtogroup cx_resource_usreconstructiontypes
* @{
*/

/** \brief Handles writing files in the format the us reconstruction
 * algorithm wants them.
 *
 *\sa UsReconstructionFileReader
 *
 * \date Dec 17, 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT UsReconstructionFileMaker
{
public:
	UsReconstructionFileMaker(QString sessionDescription);
	~UsReconstructionFileMaker();

	static QString createUniqueFolder(QString patientFolder, QString sessionDescription);
	static QString createFolder(QString patientFolder, QString sessionDescription);
	USReconstructInputData getReconstructData();

	/** Write data to disk. Assume videoRecorder has saved images in another location, reuse filenames from
	* that object to rewrite into new location.
	*/
	QString writeToNewFolder(QString path, bool compression);

	QString getSessionName() const { return mSessionDescription; }


	/**
	 * If writeColor set to true, colors will be saved even if settings is set to 8 bit
	 */
	USReconstructInputData getReconstructData(cx::ImageDataContainerPtr imageData,
											  std::vector<cx::TimeInfo> imageTimestamps,
											  TimedTransformMap trackerRecordedData,
											  std::map<double, ToolPositionMetadata> trackerRecordedMetadata,
											  std::map<double, ToolPositionMetadata> referenceRecordedMetadata,
											  ToolPtr tool,
											  QString streamUid,
											  bool writeColor,
											  Transform3D rMpr);
	void setReconstructData(USReconstructInputData data) { mReconstructData = data; }

private:
	enum TimeStampType
	{
		Modified,
		Scanner,
		SoftwareArrive,
		Unknown
	};
	bool writeUSTimestamps(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts);
	bool writeUSTransforms(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts);
	bool writeTrackerMetadata(QString reconstructionFolder, QString session, const std::map<double, ToolPositionMetadata>& ts);
	bool writeReferenceMetadata(QString reconstructionFolder, QString session, const std::map<double, ToolPositionMetadata>& ts);
	bool writeMetadata(QString filename, const std::map<double, ToolPositionMetadata>& ts, QString type);
	bool writeTrackerTransforms(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts);
	bool writeTrackerTimestamps(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts);
	void writeProbeConfiguration(QString reconstructionFolder, QString session, ProbeDefinition data, QString uid);
	void writeUSImages(QString path, ImageDataContainerPtr images, bool compression, std::vector<TimedPosition> pos);
	void writeMask(QString path, QString session, vtkImageDataPtr mask);
	void writeREADMEFile(QString reconstructionFolder, QString session);
	bool writeTimestamps(QString filename, std::vector<TimedPosition> ts, QString type, TimeStampType timeStampType = Modified);

	bool writeTransforms(QString filename, std::vector<TimedPosition> ts, QString type);
	static bool findNewSubfolder(QString subfolderAbsolutePath);
	void report();
	void fillFramePositions(USReconstructInputData* data) const;

	USReconstructInputData mReconstructData;
	QString mSessionDescription;
	QStringList mReport;
};

typedef boost::shared_ptr<UsReconstructionFileMaker> UsReconstructionFileMakerPtr;

/**
* @}
*/

}
#endif /* CXUSRECONSTRUCTIONFILEMAKER_H_ */
