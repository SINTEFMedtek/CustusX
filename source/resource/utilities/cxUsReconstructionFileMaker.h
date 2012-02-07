#ifndef CXUSRECONSTRUCTIONFILEMAKER_H_
#define CXUSRECONSTRUCTIONFILEMAKER_H_

#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include "boost/shared_ptr.hpp"
#include "sscVideoRecorder.h"
#include "sscTool.h"

class QDir;

namespace cx
{
typedef boost::shared_ptr<QTextStream> QTextStreamPtr;

/**
* \file
* \addtogroup cxResourceUtilities
* @{
*/

/**
 * UsReconstructionFileMaker
 *
 * \brief Handels writing files in the format the us reconstruction
 * algorithm wants them.
 *
 * \date Dec 17, 2010
 * \author Janne Beate Bakeng, SINTEF
 */

class UsReconstructionFileMaker
{
public:
  UsReconstructionFileMaker(ssc::TimedTransformMap trackerRecordedData, ssc::VideoRecorder::DataType streamRecordedData, QString sessionDescription, QString activepatientPath, ssc::ToolPtr tool, QString calibFilename);
  ~UsReconstructionFileMaker();

  QString write();
  QString getMhdFilename(QString reconstructionFolder);

private:
  QString makeFolder(QString patientFolder, QString sessionDescription);
  bool createSubfolder(QString subfolderAbsolutePath);
//  vtkImageDataPtr mergeFrames();
  vtkImageDataPtr mergeFrames(std::vector<vtkImageDataPtr> input);
  std::vector<vtkImageDataPtr> getFrames();

  bool writeTrackerTimestamps(QString reconstructionFolder);
  bool writeTrackerTransforms(QString reconstructionFolder);
  bool writeUSTimestamps(QString reconstructionFolder);
  bool writeUSImages(QString reconstructionFolder, QString calibrationFile);
  QString copyCalibrationFile(QString reconstructionFolder);
  void copyProbeCalibConfigsXml(QString reconstructionFolder);

  void report();

  ssc::TimedTransformMap mTrackerRecordedData;
  ssc::VideoRecorder::DataType mStreamRecordedData;
  QString mSessionDescription;
  QString mActivepatientPath;
  ssc::ToolPtr mTool;
  QString mCalibFilename;

  QStringList mReport;
};

typedef boost::shared_ptr<UsReconstructionFileMaker> UsReconstructionFileMakerPtr;

/**
* @}
*/

}
#endif /* CXUSRECONSTRUCTIONFILEMAKER_H_ */
