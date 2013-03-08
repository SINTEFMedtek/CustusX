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
#include "probeXmlConfigParser.h"
#include "sscProbeSector.h"
#include "sscForwardDeclarations.h"
#include "cxUSReconstructInputData.h"

namespace cx
{

/**
* \file
* \addtogroup cxResourceUtilities
* @{
*/

/** \brief Reader class for the US Acquisition files.
 *
 * \sa UsReconstructionFileMaker
 *
 * \section us_acq_file_format_description Format description
 *
 * All files describing one acquisition lie the same folder.  The files all have
 * the name format US-Acq_<index>_<TS><stream>.<type>,
 * where
 *  - <index> is a running index, for convenience.
 *  - <TS> is a timestamp
 *  - <stream> is the uid of the video stream. Not used prior to cx3.5.0.
 *  - <type> is the format of that specific file.
 *
 * Together, the files contains information about the us images and their
 * timestamps, the tracking positions and their timestamps, and the probe
 * calibration.
 *
 * In the following, we use <filebase> = US-Acq_<index>_<TS><stream>.
 *
 *
 * \subsection us_acq_file_format_mhd <filebase>.mhd (obsolete)
 *
 * Used prior to version cx3.4.0.
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
 * \subsection us_acq_file_format_mhd_indexed <filebase>_<index>.mhd
 *
 * A sequence of files in the metaheader file format containing the image data, one file
 * for each frame. The frame index is given by the index in the file name.
 * See http://www.itk.org/Wiki/MetaIO/Documentation for more.
 * Replaces \subsection us_acq_file_format_mhd .
 *
 * \subsection us_acq_file_format_file_xml ProbeCalibConfigs.xml (obsolete)
 *
 * This file contains the probe definition, and is copied from the
 * config/tool/Tools folder.
 *
 * \subsection us_acq_file_format_file_probedata <filebase>.probedata.xml
 *
 * This file contains the probe definition. Replaces \subsection us_acq_file_format_file_xml .
 *
 * \subsection us_acq_file_format_file_fts <filebase>.fts
 *
 * This file contains the frame timestamps. This is a sequence of
 * newline-separated floating-point numbers in milliceconds. The starting point
 * is irrelevant. The number of timestamps must equal the number of us frames.
 *
 *
 * \subsection us_acq_file_format_tp <filebase>.tp
 *
 * This file contains the tracking positions. This is a newline-separated
 * sequence of matrices, one for each tracking sample. Each matrix is the prMt,
 * i.e. the transform from tool to patient reference (See \ref ssc_page_coords ).
 * The last line of the matrix (always containing 0 0 0 1) is omitted. The matrix
 * numbers is whitespace-separated with newline between rows. Thus the number of
 * lines in this file is (# tracking positions) x 3.
 *
 *
 * \subsection us_acq_file_format_tts <filebase>.tts
 *
 * This file contains the tracking timestamps. The format equals \ref us_acq_file_format_fts ,
 * but the number of timestamps equals the number of tracking positions.
 *
 * \subsection us_acq_file_format_tp <filebase>.fp
 *
 * This file contains the frame positions. This is a newline-separated
 * sequence of matrices, one for each US frame. Each matrix is the rMu,
 * i.e. the transform from lower-left centered image space to
 * global reference (See \ref ssc_page_coords and \ref ssc::ProbeData).
 * The last line of the matrix (always containing 0 0 0 1) is omitted. The matrix
 * numbers is whitespace-separated with newline between rows. Thus the number of
 * lines in this file is (# tracking positions) x 3.
 *
 *
 */
class UsReconstructionFileReader
{
public:

public:
	UsReconstructionFileReader();

	ssc::USReconstructInputData readAllFiles(QString fileName, QString calFilesPath = "");

	std::vector<ssc::TimedPosition> readFrameTimestamps(QString fileName);
	/**
	  * Read probe data from the probedata config file attached to the mhd file,
	  * named <mhdfilename-base>.probedata.xml
	  */
	static std::pair<QString, ssc::ProbeData>  readProbeDataFromFile(QString mhdFileName);

private:
	std::vector<ssc::TimedPosition> readPositions(QString fileName);
	bool readMaskFile(QString mhdFileName, ssc::ImagePtr mask);
	ssc::USFrameDataPtr readUsDataFile(QString mhdFileName);

	void readPositionFile(QString posFile, bool alsoReadTimestamps, std::vector<ssc::TimedPosition>* timedPos);
	void readTimeStampsFile(QString fileName, std::vector<ssc::TimedPosition>* timedPos);
	void readCustomMhdTags(QString mhdFileName, QStringList* probeConfigPath, QString* calFileName);
	ProbeXmlConfigParser::Configuration readProbeConfiguration(QString calFilesPath, QStringList probeConfigPath);
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
