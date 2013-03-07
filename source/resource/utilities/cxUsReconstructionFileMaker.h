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
typedef boost::shared_ptr<class CachedImageDataContainer> CachedImageDataContainerPtr;
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
	ssc::USReconstructInputData getReconstructData();

	/** Write data to disk. Assume videoRecorder has saved images in another location, reuse filenames from
	* that object to rewrite into new location.
	*/
	QString writeToNewFolder(QString path, bool compression);

	QString getSessionName() const { return mSessionDescription; }

	ssc::USReconstructInputData getReconstructData(SavingVideoRecorderPtr videoRecorder,
	                                               ssc::TimedTransformMap trackerRecordedData,
	                                               ssc::ToolPtr tool,
	                                               bool writeColor);
	void setReconstructData(ssc::USReconstructInputData data) { mReconstructData = data; }

private:
	bool writeUSTimestamps2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts);
	bool writeUSTransforms(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts);
	bool writeTrackerTransforms2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts);
	bool writeTrackerTimestamps2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts);
	void writeProbeConfiguration2(QString reconstructionFolder, QString session, ssc::ProbeData data, QString uid);
	void writeUSImages(QString path, CachedImageDataContainerPtr images, bool compression);

	bool writeTransforms(QString filename, std::vector<ssc::TimedPosition> ts, QString type);
	static bool findNewSubfolder(QString subfolderAbsolutePath);
	void report();
	void fillFramePositions(ssc::USReconstructInputData* data) const;

	ssc::USReconstructInputData mReconstructData;

	QString mSessionDescription;
	QStringList mReport;
};

typedef boost::shared_ptr<UsReconstructionFileMaker> UsReconstructionFileMakerPtr;

/**
* @}
*/

}
#endif /* CXUSRECONSTRUCTIONFILEMAKER_H_ */
