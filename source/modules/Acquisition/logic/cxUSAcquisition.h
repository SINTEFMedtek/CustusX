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
#ifndef CXUSACQUISITION_H_
#define CXUSACQUISITION_H_

#include "cxPluginAcquisitionExport.h"

#include <vector>
#include "cxForwardDeclarations.h"

namespace cx
{
struct USReconstructInputData;
}
namespace cx
{
typedef boost::shared_ptr<class UsReconstructionFileMaker> UsReconstructionFileMakerPtr;
typedef boost::shared_ptr<class SavingVideoRecorder> SavingVideoRecorderPtr;
typedef boost::shared_ptr<class USSavingRecorder> USSavingRecorderPtr;
typedef boost::shared_ptr<class Acquisition> AcquisitionPtr;


/**
 * \file
 * \addtogroup cx_module_acquisition
 * @{
 */

/**
 * \brief Handles the us acquisition process.
 * \ingroup cx_module_acquisition
 *
 * The USAcquisition object attaches itself to an
 * input Acquisition object and records ultrasound
 * data when the Acquisiton records.
 *
 * After a successful acquisition, the data is both sent to
 * the reconstructer and saved to disk. saveDataCompleted() is
 * emitted after a successful save of each video stream.
 *
 *  \date May 12, 2011
 *  \author christiana
 */
class cxPluginAcquisition_EXPORT USAcquisition : public QObject
{
	Q_OBJECT
public:
	USAcquisition(AcquisitionPtr base, UsReconstructionServicePtr reconstructer, QObject* parent = 0);
	virtual ~USAcquisition();
	int getNumberOfSavingThreads() const;

signals:
	void acquisitionDataReady(); ///< emitted when data is acquired and sent to the reconstruction module
	void saveDataCompleted(QString mhdFilename); ///< emitted when data has been saved to file

private slots:
	void checkIfReadySlot();
	void recordStarted();
	void recordStopped();
	void recordCancelled();

private:
	std::vector<VideoSourcePtr> getRecordingVideoSources(ToolPtr tool);
	bool getWriteColor() const;
	void sendAcquisitionDataToReconstructer();

	AcquisitionPtr mBase;
	UsReconstructionServicePtr mUsReconstructionService;
	USSavingRecorderPtr mCore;
};
typedef boost::shared_ptr<USAcquisition> USAcquisitionPtr;

/**
* @}
*/
}

#endif /* CXUSACQUISITION_H_ */
