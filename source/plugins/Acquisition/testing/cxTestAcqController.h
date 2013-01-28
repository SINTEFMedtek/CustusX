/*
 * cxTestCustusXController.h
 *
 *  \date Oct 19, 2010
 *      \author christiana
 */
#ifndef CXTESTCUSTUSXCONTROLLER_H_
#define CXTESTCUSTUSXCONTROLLER_H_

#include <QApplication>
#include "cxVideoService.h"
#include "cxAcquisitionData.h"
#include "cxUSAcquisition.h"

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

private slots:
	void newFrameSlot();
	void start();
	void stop();

//	void setReady(bool ok, QString text);
	void saveDataCompletedSlot(QString name);
	void acquisitionDataReadySlot();
	void readinessChangedSlot();

private:
	ssc::ReconstructManagerPtr createReconstructionManager();
	void verifyFileData(ssc::USReconstructInputData data);
	void setupVideo(QString framesFile);
	void setupProbe(QString probedefinition);

	double mRecordDuration; ///< duration of recording in ms.
	ssc::VideoSourcePtr mVideoSource;
	cx::AcquisitionDataPtr mAcquisitionData;
	cx::USAcquisitionPtr mAcquisition;
	cx::AcquisitionPtr mAcquisitionBase;
//	cx::RecordSessionPtr mRecordSession;

	bool mMemDataValid;
	bool mFileDataValid;
};


#endif /* CXTESTCUSTUSXCONTROLLER_H_ */
