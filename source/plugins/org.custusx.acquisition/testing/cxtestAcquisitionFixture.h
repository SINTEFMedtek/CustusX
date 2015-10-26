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
#ifndef CXTESTACQUISITIONFIXTURE_H_
#define CXTESTACQUISITIONFIXTURE_H_

#include "cxtest_org_custusx_acquisition_export.h"

#include "cxForwardDeclarations.h"
#include "cxAcquisitionData.h"
#include "cxUSAcquisition.h"
#include "cxUSReconstructInputData.h"
#include "cxXmlOptionItem.h"

namespace cxtest
{

/**Helper object for automated control of the CustusX application.
 *
 */
class CXTEST_ORG_CUSTUSX_ACQUISITION_EXPORT AcquisitionFixture : public QObject
{
	Q_OBJECT

public:
	AcquisitionFixture(QObject* parent=NULL);
	~AcquisitionFixture();
	void initialize();
	void verify();
	cx::PropertyPtr getOption(QString uid);

//	QString mAdditionalGrabberArg;
	int mNumberOfExpectedStreams;

private slots:
	void newFrameSlot();
	void start();
	void stop();

	void saveDataCompletedSlot(QString name);
	void acquisitionDataReadySlot();
	void readinessChangedSlot();
	void videoConnectedSlot();

	void setupVideo();
	void setupProbe();

protected:
	cx::XmlOptionFile mOptions;

private:
	void setUp();
	void tearDown();

	void initVideo();
	cx::AcquisitionService::TYPES getContext();
	void verifyFileData(cx::USReconstructInputData data);

	cx::USReconstructInputData mMemOutputData;
	std::vector<cx::USReconstructInputData> mFileOutputData;
	QString mAcqDataFilename;
	QString mConnectionMethod;

	double mRecordDuration; ///< duration of recording in ms.
	cx::VideoSourcePtr mVideoSource;
	cx::UsReconstructionServicePtr mUsReconstructionService;
	cx::AcquisitionServicePtr mAcquisitionService;
};

} // namespace cxtest

#endif /* CXTESTACQUISITIONFIXTURE_H_ */
