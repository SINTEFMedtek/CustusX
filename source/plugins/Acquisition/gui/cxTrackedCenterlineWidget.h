#ifndef CXTRACKEDCENTERLINEWIDGET_H_
#define CXTRACKEDCENTERLINEWIDGET_H_

#include "cxRecordBaseWidget.h"
#include "cxTool.h"

namespace cx
{
/**
* \file
* \addtogroup cxPluginAcquisition
* @{
*/

/**
 * TrackedCenterlineWidget
 *
 * \brief NOT IN USE. TEST!!!
 *
 * \date Dec 9, 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class TrackedCenterlineWidget : public TrackedRecordWidget
{
  Q_OBJECT
public:
  TrackedCenterlineWidget(AcquisitionDataPtr pluginData, QWidget* parent);
  virtual ~TrackedCenterlineWidget();
  virtual QString defaultWhatsThis() const;

protected slots:
  void checkIfReadySlot();
  void postProcessingSlot(QString sessionId);
  void startedSlot(QString sessionId);
  void stoppedSlot(bool);

  void centerlineFinishedSlot();
  void preprocessResampler();

private:
  virtual ssc::TimedTransformMap getRecording(RecordSessionPtr session); ///< gets the tracking data from all relevant tool for the given session
  ToolPtr findTool(double startTime, double stopTime);

//  Centerline  mCenterlineAlgorithm;
  QString mSessionID;
};

/**
* @}
*/
}//namespace cx

#endif /* CXTRACKEDCENTERLINEWIDGET_H_ */
