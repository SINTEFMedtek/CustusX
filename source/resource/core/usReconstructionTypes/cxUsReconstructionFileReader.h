/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#ifndef CXUSRECONSTRUCTIONFILEREADER_H_
#define CXUSRECONSTRUCTIONFILEREADER_H_

#include "cxResourceExport.h"

#include <boost/shared_ptr.hpp>
#include <QString>
#include "cxTransform3D.h"
#include "ProbeXmlConfigParserImpl.h"
#include "cxProbeSector.h"
#include "cxForwardDeclarations.h"
#include "cxUSReconstructInputData.h"

namespace cx
{

/**
* \file
* \addtogroup cx_resource_usreconstructiontypes
* @{
*/

/** \brief Reader class for the US Acquisition files.
 *
 * \sa UsReconstructionFileMaker
 *
 * \section us_acq_file_format_description Format description
 *
 * All files describing one acquisition lie the same folder.  The files all have
 * the name format US-Acq_\<index\>\_\<TS\>\<stream\>.\<type\>",
 * where
 *  - \<index\> is a running index, for convenience.
 *  - \<TS\> is a timestamp
 *  - \<stream\> is the uid of the video stream. Not used prior to cx3.5.0.
 *  - \<type\> is the format of that specific file.
 *
 * Together, the files contains information about the us images and their
 * timestamps, the tracking positions and their timestamps, and the probe
 * calibration.
 *
 * In the following, we use \<filebase\> = US-Acq_\<index\>_\<TS\>\<stream\>.
 *
 *
 * \subsection us_acq_file_format_mhd \<filebase\>.mhd (obsolete)
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
 * \subsection us_acq_file_format_mhd_indexed \<filebase\>_\<index\>.mhd
 *
 * A sequence of files in the metaheader file format containing the image data, one file
 * for each frame. The frame index is given by the index in the file name.
 * See http://www.itk.org/Wiki/MetaIO/Documentation for more.
 * Replaces ref us_acq_file_format_mhd .
 *
 * \subsection us_acq_file_format_file_xml ProbeCalibConfigs.xml (obsolete)
 *
 * This file contains the probe definition, and is copied from the
 * config/tool/Tools folder.
 *
 * \subsection us_acq_file_format_file_probedata \<filebase\>.probedata.xml
 *
 * This file contains the probe definition. Replaces ref us_acq_file_format_file_xml .
 *
 * \subsection us_acq_file_format_file_fts \<filebase\>.fts
 *
 * This file contains the frame timestamps. This is a sequence of
 * newline-separated floating-point numbers in milliceconds. The starting point
 * is irrelevant. The number of timestamps must equal the number of us frames.
 *
 *
 * \subsection us_acq_file_format_tp \<filebase\>.tp
 *
 * This file contains the tracking positions. This is a newline-separated
 * sequence of matrices, one for each tracking sample. Each matrix is the prMt,
 * i.e. the transform from tool to patient reference (See \ref coordinate_systems ).
 * The last line of the matrix (always containing 0 0 0 1) is omitted. The matrix
 * numbers is whitespace-separated with newline between rows. Thus the number of
 * lines in this file is (# tracking positions) x 3.
 *
 *
 * \subsection us_acq_file_format_tts \<filebase\>.tts
 *
 * This file contains the tracking timestamps. The format equals \ref us_acq_file_format_file_fts ,
 * but the number of timestamps equals the number of tracking positions.
 *
 * \subsection us_acq_file_format_fp \<filebase\>.fp
 *
 * This file contains the frame positions. This is a newline-separated
 * sequence of matrices, one for each US frame. Each matrix is the rMu,
 * i.e. the transform from lower-left centered image space to
 * global reference (See \ref coordinate_systems and \ref ProbeDefinition).
 * The last line of the matrix (always containing 0 0 0 1) is omitted. The matrix
 * numbers is whitespace-separated with newline between rows. Thus the number of
 * lines in this file is (# tracking positions) x 3.
 *
 * \subsection us_acq_file_format_mask \<filebase\>.mask.mhd
 *
 * This file contains the image mask. The binary image shows what parts
 * of the frame images contain valid US data. This file is only written,
 * not read. It can be constructed from the probe data.
 *
 */
class cxResource_EXPORT UsReconstructionFileReader
{
public:

public:
	UsReconstructionFileReader();

	/** Read all data from the files and return as a FileData object.
	 *
	 * NOTE: The mFrames var will not be initialized with transforms,
	 * they must be generated explicitly.
	 *
	 * the mMask var is filled with data from ProbeDefinition, or from file if present.
	 */
	USReconstructInputData readAllFiles(QString fileName, QString calFilesPath = "");

	std::vector<TimedPosition> readFrameTimestamps(QString fileName);
	/**
	  * Read probe data from the probedata config file attached to the mhd file,
	  * named \<mhdfilename-base\>.probedata.xml
	  */
	static std::pair<QString, ProbeDefinition>  readProbeDefinitionFromFile(QString mhdFileName);

private:
	bool valid(USReconstructInputData input);
	std::vector<TimedPosition> readPositions(QString fileName);
	bool readMaskFile(QString mhdFileName, ImagePtr mask);
	USFrameDataPtr readUsDataFile(QString mhdFileName);

	void readPositionFile(QString posFile, bool alsoReadTimestamps, std::vector<TimedPosition>* timedPos);
	void readTimeStampsFile(QString fileName, std::vector<TimedPosition>* timedPos);
	void readCustomMhdTags(QString mhdFileName, QStringList* probeConfigPath, QString* calFileName);
	ProbeXmlConfigParser::Configuration readProbeConfiguration(QString calFilesPath, QStringList probeConfigPath);
	std::pair<QString, ProbeDefinition>  readProbeDefinitionBackwardsCompatible(QString mhdFileName, QString calFilesPath);

//	ImagePtr createMaskFromConfigParams(USReconstructInputData data);
	ImagePtr generateMask(USReconstructInputData data);
};

typedef boost::shared_ptr<UsReconstructionFileReader> UsReconstructionFileReaderPtr;

/**
* @}
*/

} // namespace cx

#endif /* CXUSRECONSTRUCTIONFILEREADER_H_ */
