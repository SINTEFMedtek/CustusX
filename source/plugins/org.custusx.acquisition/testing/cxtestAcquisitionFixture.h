/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
