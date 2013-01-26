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

  /**
	* If set, image files in created by this object will be deleted when
	* object goes out of scope
	*/
  void setDeleteFilesOnRelease(bool on);
  void setData(ssc::TimedTransformMap trackerRecordedData, SavingVideoRecorderPtr videoRecorder,
  		ssc::ToolPtr tool, QString calibFilename, bool writeColor = false);
  ssc::USReconstructInputData getReconstructData();
  /** Write data to disk. Assume videoRecorder handles the saving of the frames
	*/
  QString write();
  QString getMhdFilename(QString reconstructionFolder);

  /** Change write location, enabling writeRedirected() to be called threaded.
	* TODO: not necessary????
	*/
  void redirectSaveLocation(QString newFolder);
  /** Set compression on/off for the case where this class and not the SavingVideoRecorder saves images.
	*/
  void setImageCompression(bool on);
  /** Write data to disk. Assume videoRecorder has saved images in another location, reuse filenames from
	* that object to rewrite into new location.
	*
	*/
  QString writeToNewFolder(QString activepatientPath, bool compression);

  QString getFolderName() const { return mFolderName; }
  QString getSessionName() const { return mSessionDescription; }


private:
  QString write(ssc::USReconstructInputData data);
  QString write(QString newBaseFolder);
//  bool writeUSImages2(QString reconstructionFolder, ssc::USFrameDataPtr data, QString filename);
  bool writeUSTimestamps2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts);
  bool writeTrackerTransforms2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts);
  bool writeTrackerTimestamps2(QString reconstructionFolder, QString session, std::vector<ssc::TimedPosition> ts);
  void writeProbeConfiguration2(QString reconstructionFolder, QString session, ssc::ProbeData data, QString uid);
  void writeUSImages(QString path, std::vector<QString> images, bool compression);

  QString findFolderName(QString patientFolder, QString sessionDescription);
  bool findNewSubfolder(QString subfolderAbsolutePath);
//  vtkImageDataPtr mergeFrames(std::vector<vtkImageDataPtr> input);
//  std::vector<vtkImageDataPtr> getFrames(ssc::VideoRecorder::DataType streamRecordedData, bool writeColor);

  void report();

  ssc::USReconstructInputData mReconstructData;

  ssc::USReconstructInputData getReconstructData(ssc::TimedTransformMap trackerRecordedData,
//  		SavingVideoRecorder::DataType streamRecordedData,
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
	bool mDeleteFilesOnRelease;
};

typedef boost::shared_ptr<UsReconstructionFileMaker> UsReconstructionFileMakerPtr;

/**
* @}
*/

}
#endif /* CXUSRECONSTRUCTIONFILEMAKER_H_ */
