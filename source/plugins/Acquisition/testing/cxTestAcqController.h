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

private slots:
  void newFrameSlot();
  void start();
  void stop();

  void setReady(bool ok, QString text);
  void saveDataCompletedSlot(QString name);
  void acquisitionDataReadySlot();

private:
  ssc::ReconstructManagerPtr createReconstructionManager();
  ssc::VideoSourcePtr mVideoSource;
  cx::AcquisitionDataPtr mAcquisitionData;
  cx::USAcquisitionPtr mAcquisition;
  cx::RecordSessionPtr mRecordSession;
};


#endif /* CXTESTCUSTUSXCONTROLLER_H_ */
