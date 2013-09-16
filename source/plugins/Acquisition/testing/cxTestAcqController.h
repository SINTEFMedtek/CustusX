/*
 * cxTestAcqXController.h
 *
 *  \date Oct 19, 2010
 *      \author christiana
 */
#ifndef CXTESTACQCONTROLLER_H_
#define CXTESTACQCONTROLLER_H_

#include <QApplication>
#include "sscForwardDeclarations.h"
#include "cxAcquisitionData.h"
#include "cxUSAcquisition.h"
#include "cxUSReconstructInputData.h"

/**Helper object for automated control of the CustusX application.
 *
 */
class TestAcqController : public QObject
{
	Q_OBJECT

public:
	TestAcqController(QObject* parent);
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


#endif /* CXTESTACQCONTROLLER_H_ */
