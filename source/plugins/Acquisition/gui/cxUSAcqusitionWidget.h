#ifndef CXUSACQUSITIONWIDGET_H_
#define CXUSACQUSITIONWIDGET_H_

#include "cxRecordBaseWidget.h"
#include "cxUSAcquisition.h"
#include "cxDisplayTimerWidget.h"

namespace ssc
{
	typedef boost::shared_ptr<class ThreadedTimedReconstructer> ThreadedTimedReconstructerPtr;
}

namespace cx
{
class TimedAlgorithmProgressBar;

/**
* \file
* \addtogroup cxPluginAcquisition
* @{
*/



/**
 * USAcqusitionWidget
 *
 * \brief
 *
 * \date Dec 9, 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class USAcqusitionWidget : public RecordBaseWidget
{
  Q_OBJECT
public:
  USAcqusitionWidget(AcquisitionDataPtr pluginData, QWidget* parent);
  virtual ~USAcqusitionWidget();
  virtual QString defaultWhatsThis() const;

protected slots:

  void postProcessingSlot(QString sessionId);
  void startedSlot();
  void stoppedSlot();

private slots:
  void reconstructFinishedSlot();
  void saveDataCompletedSlot(QString mhdFilename);

private:
  USAcquisitionPtr mAcquisition;
//  ssc::ThreadedReconstructerPtr mThreadedReconstructer;
  ssc::ThreadedTimedReconstructerPtr mThreadedTimedReconstructer;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;
	DisplayTimerWidget* mDisplayTimerWidget;
};


/**
* @}
*/
}//namespace cx

#endif /* CXUSACQUSITIONWIDGET_H_ */
