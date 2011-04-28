#ifndef CXTRACKEDCENTERLINEWIDGET_H_
#define CXTRACKEDCENTERLINEWIDGET_H_

#include "cxRecordBaseWidget.h"
#include "cxCenterline.h"

namespace cx
{
/**
 * TrackedCenterlineWidget
 *
 * \brief
 *
 * \date Dec 9, 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class TrackedCenterlineWidget : public TrackedRecordWidget
{
  Q_OBJECT
public:
  TrackedCenterlineWidget(QWidget* parent);
  virtual ~TrackedCenterlineWidget();

protected slots:
  void checkIfReadySlot();
  void postProcessingSlot(QString sessionId);
  void startedSlot();
  void stoppedSlot();

  void centerlineFinishedSlot();

private:
  virtual ssc::TimedTransformMap getRecording(RecordSessionPtr session); ///< gets the tracking data from all relevant tool for the given session
  ToolPtr findTool(double startTime, double stopTime);

  Centerline  mCenterlineAlgorithm;
};
}//namespace cx

#endif /* CXTRACKEDCENTERLINEWIDGET_H_ */
