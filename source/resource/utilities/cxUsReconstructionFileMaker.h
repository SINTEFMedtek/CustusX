#ifndef CXUSRECONSTRUCTIONFILEMAKER_H_
#define CXUSRECONSTRUCTIONFILEMAKER_H_

#include <QFile>
#include <QThread>
#include <QMutex>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <utility>
#include "boost/shared_ptr.hpp"
#include "sscVideoRecorder.h"
#include "sscTool.h"
#include "sscUSFrameData.h"
#include "cxSavingVideoRecorder.h"

class QDir;

namespace cx
{
typedef boost::shared_ptr<QTextStream> QTextStreamPtr;

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
	bool writeTrackerTransforms2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts);
	bool writeTrackerTimestamps2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts);
	void writeProbeConfiguration2(QString reconstructionFolder, QString session, ssc::ProbeData data, QString uid);
	void writeUSImages(QString path, CachedImageDataContainerPtr images, bool compression);

	static bool findNewSubfolder(QString subfolderAbsolutePath);
	void report();

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
