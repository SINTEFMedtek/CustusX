/*
 * cxUsReconstructionFileReader.h
 *
 *  Created on: Feb 3, 2011
 *      Author: christiana
 */

#ifndef CXUSRECONSTRUCTIONFILEREADER_H_
#define CXUSRECONSTRUCTIONFILEREADER_H_

#include <boost/shared_ptr.hpp>
#include <QString>
#include "sscTransform3D.h"
#include "sscImage.h"
#include "sscUSFrameData.h"
#include "probeXmlConfigParser.h"

namespace cx
{

class UsReconstructionFileReader
{
public:
  UsReconstructionFileReader();

  ssc::Transform3D readTransformFromFile(QString fileName);
  bool readMaskFile(QString mhdFileName, ssc::ImagePtr mask);

  std::vector<ssc::TimedPosition> readFrameTimestamps(QString fileName);
  std::vector<ssc::TimedPosition> readPositions(QString fileName);
  ssc::USFrameDataPtr readUsDataFile(QString mhdFileName);

  void readCustomMhdTags(QString mhdFileName, QStringList* probeConfigPath, QString* calFileName);
  ProbeXmlConfigParser::Configuration readProbeConfiguration(QString calFilesPath, QStringList probeConfigPath);

private:
  void readPositionFile(QString posFile, bool alsoReadTimestamps, std::vector<ssc::TimedPosition>* timedPos);
  void readTimeStampsFile(QString fileName, std::vector<ssc::TimedPosition>* timedPos);
};

typedef boost::shared_ptr<UsReconstructionFileReader> UsReconstructionFileReaderPtr;


} // namespace cx

#endif /* CXUSRECONSTRUCTIONFILEREADER_H_ */
