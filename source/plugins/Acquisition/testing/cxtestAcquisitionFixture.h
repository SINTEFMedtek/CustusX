/*
 * cxTestAcqXController.h
 *
 *  \date Oct 19, 2010
 *      \author christiana
 */
#ifndef CXTESTACQUISITIONFIXTURE_H_
#define CXTESTACQUISITIONFIXTURE_H_

#include "sscForwardDeclarations.h"
#include "cxAcquisitionData.h"
#include "cxUSAcquisition.h"
#include "cxUSReconstructInputData.h"

namespace cxtest
{

/**Helper object for automated control of the CustusX application.
 *
 */
class AcquisitionFixture : public QObject
{
	Q_OBJECT

public:
	AcquisitionFixture(QObject* parent=NULL);
	~AcquisitionFixture();
	void initialize();
	void verify();

	QString mConnectionMethod;
	QString mAdditionalGrabberArg;
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

private:
	void setUp();
	void tearDown();

	cx::ReconstructManagerPtr createReconstructionManager();
	void verifyFileData(cx::USReconstructInputData data);

	cx::USReconstructInputData mMemOutputData;
	std::vector<cx::USReconstructInputData> mFileOutputData;
	QString mAcqDataFilename;

	double mRecordDuration; ///< duration of recording in ms.
	cx::VideoSourcePtr mVideoSource;
	cx::AcquisitionDataPtr mAcquisitionData;
	cx::USAcquisitionPtr mAcquisition;
	cx::AcquisitionPtr mAcquisitionBase;
};

} // namespace cxtest

#endif /* CXTESTACQUISITIONFIXTURE_H_ */
