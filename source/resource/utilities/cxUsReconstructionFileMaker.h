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




//struct USReconstructInputData
//{
//	QString mFilename; ///< filename used for current data read
//
//	ssc::USFrameDataPtr mUsRaw;///<All imported US data frames with pointers to each frame
//	std::vector<ssc::TimedPosition> mFrames;
//	std::vector<ssc::TimedPosition> mPositions;
//	ssc::ImagePtr mMask;///< Clipping mask for the input data
//	ssc::ProbeSector mProbeData;
//};

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
  UsReconstructionFileMaker(QString sessionDescription, QString activepatientPath);
  ~UsReconstructionFileMaker();

  void setData(ssc::TimedTransformMap trackerRecordedData, SavingVideoRecorderPtr videoRecorder,
  		ssc::ToolPtr tool, QString calibFilename, bool writeColor = false);
  ssc::USReconstructInputData getReconstructData();
  QString write();
  QString getMhdFilename(QString reconstructionFolder);


  QString getFolderName() const { return mFolderName; }
  QString getSessionName() const { return mSessionDescription; }


private:
  QString write(ssc::USReconstructInputData data);
//  bool writeUSImages2(QString reconstructionFolder, ssc::USFrameDataPtr data, QString filename);
  bool writeUSTimestamps2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts);
  bool writeTrackerTransforms2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts);
  bool writeTrackerTimestamps2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts);
  void writeProbeConfiguration2(QString reconstructionFolder, QString session, ssc::ProbeData data, QString uid);

  QString findFolderName(QString patientFolder, QString sessionDescription);
  bool findNewSubfolder(QString subfolderAbsolutePath);
//  vtkImageDataPtr mergeFrames(std::vector<vtkImageDataPtr> input);
//  std::vector<vtkImageDataPtr> getFrames(ssc::VideoRecorder::DataType streamRecordedData, bool writeColor);

  void report();

  ssc::USReconstructInputData mReconstructData;

  ssc::USReconstructInputData getReconstructData(ssc::TimedTransformMap trackerRecordedData,
  		SavingVideoRecorder::DataType streamRecordedData,
//  		QString sessionDescription,
//  		QString activepatientPath,
  		ssc::ToolPtr tool,
  		QString calibFilename,
  		bool writeColor);

  QString mSessionDescription;
  QString mActivepatientPath;
  QStringList mReport;
	QString mFolderName;
	SavingVideoRecorderPtr mVideoRecorder;
};

typedef boost::shared_ptr<UsReconstructionFileMaker> UsReconstructionFileMakerPtr;

/**
* @}
*/

}
#endif /* CXUSRECONSTRUCTIONFILEMAKER_H_ */
