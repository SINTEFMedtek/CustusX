#ifndef CXUSRECONSTRUCTIONFILEMAKER_H_
#define CXUSRECONSTRUCTIONFILEMAKER_H_

#include <QFile>
#include <QString>
#include <QTextStream>
#include "boost/shared_ptr.hpp"
#include "sscRTSourceRecorder.h"
//#include "cxRecordSession.h"
#include "cxTool.h"

namespace cx
{
typedef boost::shared_ptr<QTextStream> QTextStreamPtr;

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
  UsReconstructionFileMaker(ssc::TimedTransformMap trackerRecordedData, ssc::RTSourceRecorder::DataType streamRecordedData, QString sessionDescription, QString activepatientPath, ssc::ToolPtr tool);
  ~UsReconstructionFileMaker();

  QString write();
  QString getMhdFilename(QString reconstructionFolder);

private:
  QString makeFolder(QString patientFolder, QString sessionDescription);
  bool createSubfolder(QString subfolderAbsolutePath);
  vtkImageDataPtr mergeFrames();

  void writeTrackerTimestamps(QString reconstructionFolder);
  void writeTrackerTransforms(QString reconstructionFolder);
  void writeUSTimestamps(QString reconstructionFolder);
  void writeUSImages(QString reconstructionFolder, QString calibrationFile);
  QString copyCalibrationFile(QString reconstructionFolder);
  void copyProbeCalibConfigsXml(QString reconstructionFolder);

  ssc::TimedTransformMap mTrackerRecordedData;
  ssc::RTSourceRecorder::DataType mStreamRecordedData;
  //RecordSessionPtr mSession;
  QString mSessionDescription;
  QString mActivepatientPath;
  ssc::ToolPtr mTool;

};

typedef boost::shared_ptr<UsReconstructionFileMaker> UsReconstructionFileMakerPtr;

}
#endif /* CXUSRECONSTRUCTIONFILEMAKER_H_ */
