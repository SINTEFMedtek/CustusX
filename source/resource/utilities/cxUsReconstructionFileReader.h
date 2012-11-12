/*
 * cxUsReconstructionFileReader.h
 *
 *  \date Feb 3, 2011
 *      \author christiana
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

/**
* \file
* \addtogroup cxResourceUtilities
* @{
*/

/**\brief Reader class for the US Acquisition files.
 *
 * \sa UsReconstructionFileMaker
 *
 * \section us_acq_file_format_description Format description
 *
 * All files describing one acquisition lie the same folder.  The files all have
 * the name format US-Acq_<index>_<TS>.<type>,
 * where
 *  - <index> is a running index, for convenience.
 *  - <TS> is a timestamp
 *  - <type> is the format of that specific file.
 *
 * Together, the files contains information about the us images and their
 * timestamps, the tracking positions and their timestamps, and the probe
 * calibration.
 *
 *
 *
 * \subsection us_acq_file_format_mhd US-Acq_<index>_<TS>.mhd
 *
 * A file in the metaheader file format containing the uncompressed image data.
 * the z-direction is the time axis, i.e. the z dim is the number of us frames.
 * See http://www.itk.org/Wiki/MetaIO/Documentation for more.
 *
 * Two extra tags are added:
 *
 * \verbatim
ConfigurationID = <path:inside:ProbeCalibConfigs.xml>
ProbeCalibration = <not used>
\endverbatim
 *
 * The ConfigurationID refers to a specific configuration within
 * ProbeCalibConfigs.xml, using colon separators.
 *
 *
 * \subsection us_acq_file_format_file_xml ProbeCalibConfigs.xml
 *
 * This file contains the probe definition, and is copied from the
 * config/tool/Tools folder.
 *
 *
 * \subsection us_acq_file_format_file_fts US-Acq_<index>_<TS>.fts
 *
 * This file contains the frame timestamps. This is a sequence of
 * newline-separated floating-point numbers in milliceconds. The starting point
 * is irrelevant. The number of timestamps must equal the number of us frames.
 *
 *
 * \subsection us_acq_file_format_tp US-Acq_<index>_<TS>.tp
 *
 * This file contains the tracking positions. This is a newline-separated
 * sequence of matrices, one for each tracking sample. Each matrix is the prMt,
 * i.e. the transform from tool to patient reference (See \ref ssc_page_coords ).
 * The last line of the matrix (always containing 0 0 0 1) is omitted. The matrix
 * numbers is whitespace-separated with newline between rows. Thus the number of
 * lines in this file is (# tracking positions) x 3.
 *
 *
 * \subsection us_acq_file_format_tts US-Acq_<index>_<TS>.tts
 *
 * This file contains the tracking timestamps. The format equals \ref us_acq_file_format_fts ,
 * but the number of timestamps equals the number of tracking positions.
 *
 */
class UsReconstructionFileReader
{
public:

public:
	UsReconstructionFileReader();

	ssc::USReconstructInputData readAllFiles(QString fileName, QString calFilesPath = "");

	bool readMaskFile(QString mhdFileName, ssc::ImagePtr mask);

	std::vector<ssc::TimedPosition> readFrameTimestamps(QString fileName);
	std::vector<ssc::TimedPosition> readPositions(QString fileName);
	ssc::USFrameDataPtr readUsDataFile(QString mhdFileName);

private:
	void readPositionFile(QString posFile, bool alsoReadTimestamps, std::vector<ssc::TimedPosition>* timedPos);
	void readTimeStampsFile(QString fileName, std::vector<ssc::TimedPosition>* timedPos);
	void readCustomMhdTags(QString mhdFileName, QStringList* probeConfigPath, QString* calFileName);
	ProbeXmlConfigParser::Configuration readProbeConfiguration(QString calFilesPath, QStringList probeConfigPath);
	std::pair<QString, ssc::ProbeData>  readProbeDataFromFile(QString mhdFileName);
	std::pair<QString, ssc::ProbeData>  readProbeDataBackwardsCompatible(QString mhdFileName, QString calFilesPath);

	ssc::ImagePtr createMaskFromConfigParams(ssc::USReconstructInputData data);
	ssc::ImagePtr generateMask(ssc::USReconstructInputData data);
};

typedef boost::shared_ptr<UsReconstructionFileReader> UsReconstructionFileReaderPtr;

/**
* @}
*/

} // namespace cx

#endif /* CXUSRECONSTRUCTIONFILEREADER_H_ */
