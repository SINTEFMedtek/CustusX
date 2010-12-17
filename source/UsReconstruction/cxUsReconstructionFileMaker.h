#ifndef CXUSRECONSTRUCTIONFILEMAKER_H_
#define CXUSRECONSTRUCTIONFILEMAKER_H_

#include <QString>
#include "sscTool.h"
#include "sscRealTimeStreamSourceRecorder.h"

namespace cx
{
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
  UsReconstructionFileMaker(ssc::TimedTransformMap trackerRecordedData, ssc::RealTimeStreamSourceRecorder::DataType streamRecordedData, QString activepatientPath);
  ~UsReconstructionFileMaker();

private:
};
}
#endif /* CXUSRECONSTRUCTIONFILEMAKER_H_ */
