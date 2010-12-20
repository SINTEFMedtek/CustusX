#ifndef CXUSRECONSTRUCTIONFILEMAKER_H_
#define CXUSRECONSTRUCTIONFILEMAKER_H_

#include <QFile>
#include <QString>
#include <QTextStream>
#include "boost/shared_ptr.hpp"
#include "sscRealTimeStreamSourceRecorder.h"
#include "cxRecordSession.h"
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
  UsReconstructionFileMaker(ssc::TimedTransformMap trackerRecordedData, ssc::RealTimeStreamSourceRecorder::DataType streamRecordedData, RecordSessionPtr session, QString activepatientPath, ToolPtr tool);
  ~UsReconstructionFileMaker();

  void write();

private:
  QString makeFolder(QString patientFolder, RecordSessionPtr session);

  void writeTrackerTimestamps(QString reconstructionFolder);
  void writeTrackerTransforms(QString reconstructionFolder);
  void writeUSTimestamps(QString reconstructionFolder);
  void writeUSImages(QString reconstructionFolder);

  ssc::TimedTransformMap mTrackerRecordedData;
  ssc::RealTimeStreamSourceRecorder::DataType mStreamRecordedData;
  RecordSessionPtr mSession;
  QString mActivepatientPath;
  ToolPtr mTool;

};
}
#endif /* CXUSRECONSTRUCTIONFILEMAKER_H_ */
