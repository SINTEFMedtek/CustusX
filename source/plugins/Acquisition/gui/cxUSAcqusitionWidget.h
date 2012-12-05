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
  void startedSlot(QString sessionId);
  void stoppedSlot();

private slots:
	void reconstructStartedSlot();
  void reconstructFinishedSlot();
//  void saveDataCompletedSlot(QString mhdFilename);
  void toggleDetailsSlot();
  void acquisitionDataReadySlot();
  void reconstructAboutToStartSlot();

private:
  USAcquisitionPtr mAcquisition;
//  ssc::ThreadedReconstructerPtr mThreadedReconstructer;
//  ssc::ThreadedTimedReconstructerPtr mThreadedTimedReconstructer;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;
	DisplayTimerWidget* mDisplayTimerWidget;
	QWidget* mOptionsWidget;
	QWidget* createOptionsWidget();
	QWidget* wrapVerticalStretch(QWidget* input);
	QWidget* wrapGroupBox(QWidget* input, QString name, QString tip);
};


/**
* @}
*/
}//namespace cx

#endif /* CXUSACQUSITIONWIDGET_H_ */
