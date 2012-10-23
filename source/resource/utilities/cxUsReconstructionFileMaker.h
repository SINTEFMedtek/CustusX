#ifndef CXUSRECONSTRUCTIONFILEMAKER_H_
#define CXUSRECONSTRUCTIONFILEMAKER_H_

#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include "boost/shared_ptr.hpp"
#include "sscVideoRecorder.h"
#include "sscTool.h"
#include "sscUSFrameData.h"

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

/**\brief Handles writing files in the format the us reconstruction
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
  UsReconstructionFileMaker(ssc::TimedTransformMap trackerRecordedData, ssc::VideoRecorder::DataType streamRecordedData,
  		QString sessionDescription, QString activepatientPath, ssc::ToolPtr tool, QString calibFilename,
  		bool writeColor = false);
  ~UsReconstructionFileMaker();

  ssc::USReconstructInputData getReconstructData();
  QString write();
  QString getMhdFilename(QString reconstructionFolder);

private:
  QString findFolderName(QString patientFolder, QString sessionDescription);
  bool findNewSubfolder(QString subfolderAbsolutePath);
  vtkImageDataPtr mergeFrames(std::vector<vtkImageDataPtr> input);
  std::vector<vtkImageDataPtr> getFrames();

  bool writeTrackerTimestamps(QString reconstructionFolder);
  bool writeTrackerTransforms(QString reconstructionFolder);
  bool writeUSTimestamps(QString reconstructionFolder);
  bool writeUSImages(QString reconstructionFolder, QString calibrationFile);
  QString copyCalibrationFile(QString reconstructionFolder);
  void copyProbeCalibConfigsXml(QString reconstructionFolder);
  void writeProbeConfiguration(QString reconstructionFolder);

  void report();

  ssc::TimedTransformMap mTrackerRecordedData;
  ssc::VideoRecorder::DataType mStreamRecordedData;
  QString mSessionDescription;
  QString mActivepatientPath;
  ssc::ToolPtr mTool;
  QString mCalibFilename;
  bool mWriteColor;///< If set to true, colors will be saved even if settings is set to 8 bit

  QStringList mReport;
};

typedef boost::shared_ptr<UsReconstructionFileMaker> UsReconstructionFileMakerPtr;

/**
* @}
*/

}
#endif /* CXUSRECONSTRUCTIONFILEMAKER_H_ */
