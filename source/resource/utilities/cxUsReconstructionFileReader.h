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
//#include "sscTool.h"
#include "sscProbeSector.h"
#include "sscForwardDeclarations.h"

namespace cx
{

class UsReconstructionFileReader
{
public:

//	struct FileData
//	{
//		QString mFilename; ///< filename used for current data read
//
//		ssc::USFrameDataPtr mUsRaw;///<All imported US data frames with pointers to each frame
//		std::vector<ssc::TimedPosition> mFrames;
//		std::vector<ssc::TimedPosition> mPositions;
//		ssc::ImagePtr mMask;///< Clipping mask for the input data
//		ssc::ProbeSector mProbeData;
//	};

public:
	UsReconstructionFileReader();

	ssc::USReconstructInputData readAllFiles(QString fileName, QString calFilesPath = "", bool angio = false);

	static ssc::Transform3D readTransformFromFile(QString fileName, bool* ok=NULL);
	bool readMaskFile(QString mhdFileName, ssc::ImagePtr mask);

	std::vector<ssc::TimedPosition> readFrameTimestamps(QString fileName);
	std::vector<ssc::TimedPosition> readPositions(QString fileName);
	ssc::USFrameDataPtr readUsDataFile(QString mhdFileName, bool angio = false);

	void readCustomMhdTags(QString mhdFileName, QStringList* probeConfigPath, QString* calFileName);
	ProbeXmlConfigParser::Configuration readProbeConfiguration(QString calFilesPath, QStringList probeConfigPath);

private:
	void readPositionFile(QString posFile, bool alsoReadTimestamps, std::vector<ssc::TimedPosition>* timedPos);
	void readTimeStampsFile(QString fileName, std::vector<ssc::TimedPosition>* timedPos);

	ssc::ImagePtr createMaskFromConfigParams(ssc::USReconstructInputData data);
	ssc::ImagePtr generateMask(ssc::USReconstructInputData data);
};

typedef boost::shared_ptr<UsReconstructionFileReader> UsReconstructionFileReaderPtr;

} // namespace cx

#endif /* CXUSRECONSTRUCTIONFILEREADER_H_ */
