#ifndef CXUSACQUSITIONWIDGET_H_
#define CXUSACQUSITIONWIDGET_H_

#include "cxRecordBaseWidget.h"
#include "cxUSAcquisition.h"

namespace ssc
{
	typedef boost::shared_ptr<class ThreadedReconstructer> ThreadedReconstructerPtr;
}

namespace cx
{
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
  ssc::ThreadedReconstructerPtr mThreadedReconstructer;
};


/**
* @}
*/
}//namespace cx

#endif /* CXUSACQUSITIONWIDGET_H_ */
