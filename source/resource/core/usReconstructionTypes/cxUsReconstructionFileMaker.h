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

#ifndef CXUSRECONSTRUCTIONFILEMAKER_H_
#define CXUSRECONSTRUCTIONFILEMAKER_H_

#include "cxResourceExport.h"

class QTextStream;
class QDir;

#include "cxTool.h"
#include "cxUSReconstructInputData.h"
#include "cxForwardDeclarations.h"


namespace cx
{
class TimeInfo;
typedef boost::shared_ptr<QTextStream> QTextStreamPtr;
typedef boost::shared_ptr<class ImageDataContainer> ImageDataContainerPtr;
//typedef boost::shared_ptr<class CachedImageDataContainer> CachedImageDataContainerPtr;
typedef boost::shared_ptr<class SavingVideoRecorder> SavingVideoRecorderPtr;


/**
* \file
* \addtogroup cx_resource_usreconstructiontypes
* @{
*/

/** \brief Handles writing files in the format the us reconstruction
 * algorithm wants them.
 *
 *\sa UsReconstructionFileReader
 *
 * \date Dec 17, 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT UsReconstructionFileMaker
{
public:
	UsReconstructionFileMaker(QString sessionDescription);
	~UsReconstructionFileMaker();

	static QString createUniqueFolder(QString patientFolder, QString sessionDescription);
	static QString createFolder(QString patientFolder, QString sessionDescription);
	USReconstructInputData getReconstructData();

	/** Write data to disk. Assume videoRecorder has saved images in another location, reuse filenames from
	* that object to rewrite into new location.
	*/
	QString writeToNewFolder(QString path, bool compression);

	QString getSessionName() const { return mSessionDescription; }


	/**
	 * If writeColor set to true, colors will be saved even if settings is set to 8 bit
	 */
	USReconstructInputData getReconstructData(cx::ImageDataContainerPtr imageData,
												   std::vector<cx::TimeInfo> imageTimestamps,
	                                               TimedTransformMap trackerRecordedData,
	                                               ToolPtr tool,
	                                               bool writeColor,
	                                               Transform3D rMpr);
	void setReconstructData(USReconstructInputData data) { mReconstructData = data; }

private:
	enum TimeStampType
	{
		Modified,
		Scanner,
		SoftwareArrive,
		Unknown
	};
	bool writeUSTimestamps(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts);
	bool writeUSTransforms(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts);
	bool writeTrackerTransforms(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts);
	bool writeTrackerTimestamps(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts);
	void writeProbeConfiguration(QString reconstructionFolder, QString session, ProbeDefinition data, QString uid);
	void writeUSImages(QString path, ImageDataContainerPtr images, bool compression, std::vector<TimedPosition> pos);
	void writeMask(QString path, QString session, vtkImageDataPtr mask);
	void writeREADMEFile(QString reconstructionFolder, QString session);
	bool writeTimestamps(QString filename, std::vector<TimedPosition> ts, QString type, TimeStampType timeStampType = Modified);

	bool writeTransforms(QString filename, std::vector<TimedPosition> ts, QString type);
	static bool findNewSubfolder(QString subfolderAbsolutePath);
	void report();
	void fillFramePositions(USReconstructInputData* data) const;

	USReconstructInputData mReconstructData;
	QString mSessionDescription;
	QStringList mReport;
};

typedef boost::shared_ptr<UsReconstructionFileMaker> UsReconstructionFileMakerPtr;

/**
* @}
*/

}
#endif /* CXUSRECONSTRUCTIONFILEMAKER_H_ */
