#ifndef CXUSRECONSTRUCTIONFILEMAKER_H_
#define CXUSRECONSTRUCTIONFILEMAKER_H_

class QTextStream;
class QDir;

#include "sscTool.h"
#include "cxUSReconstructInputData.h"
#include "cxForwardDeclarations.h"


namespace cx
{
typedef boost::shared_ptr<QTextStream> QTextStreamPtr;
typedef boost::shared_ptr<class ImageDataContainer> ImageDataContainerPtr;
//typedef boost::shared_ptr<class CachedImageDataContainer> CachedImageDataContainerPtr;
typedef boost::shared_ptr<class SavingVideoRecorder> SavingVideoRecorderPtr;

/**
* \file
* \addtogroup cxResourceUtilities
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
class UsReconstructionFileMaker
{
public:
	/**
	 * \param writeColor If set to true, colors will be saved even if settings is set to 8 bit
	 */
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


	USReconstructInputData getReconstructData(cx::ImageDataContainerPtr imageData,
												   std::vector<double> imageTimestamps,
	                                               TimedTransformMap trackerRecordedData,
	                                               ToolPtr tool,
	                                               bool writeColor,
	                                               Transform3D rMpr);
	void setReconstructData(USReconstructInputData data) { mReconstructData = data; }

private:
	bool writeUSTimestamps(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts);
	bool writeUSTransforms(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts);
	bool writeTrackerTransforms(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts);
	bool writeTrackerTimestamps(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts);
	void writeProbeConfiguration(QString reconstructionFolder, QString session, ProbeDefinition data, QString uid);
	void writeUSImages(QString path, ImageDataContainerPtr images, bool compression, std::vector<TimedPosition> pos);
	void writeMask(QString path, QString session, vtkImageDataPtr mask);
	void writeREADMEFile(QString reconstructionFolder, QString session);

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
