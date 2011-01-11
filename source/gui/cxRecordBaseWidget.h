#ifndef CXRECORDBASEWIDGET_H_
#define CXRECORDBASEWIDGET_H_

#include <QWidget>
#include "sscTool.h"
#include "cxDataInterface.h"
#include "sscRealTimeStreamSourceRecorder.h"

class QLabel;
class QVBoxLayout;

namespace cx
{

class RecordSessionWidget;
/**
 * RecordBaseWidget
 *
 * \brief
 *
 * \date Dec 9, 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class RecordBaseWidget : public QWidget
{
  Q_OBJECT

public:
  RecordBaseWidget(QWidget* parent, QString description = "Record Session");
  virtual ~RecordBaseWidget();

signals:
  void ready(bool ready);//emitted when the the prerequisits for recording is met

protected slots:
  virtual void checkIfReadySlot() = 0;
  virtual void postProcessingSlot(QString sessionId) = 0;
  virtual void startedSlot() = 0;
  virtual void stoppedSlot() = 0;

protected:
  void setWhatsMissingInfo(QString info);

  QVBoxLayout* mLayout;

private:
  QLabel* mInfoLabel;
  RecordSessionWidget* mRecordSessionWidget;
};

/**
 * TrackedRecordWidget
 *
 * \brief
 *
 * \date Dec 17, 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class TrackedRecordWidget : public RecordBaseWidget
{
  Q_OBJECT
public:
  TrackedRecordWidget(QWidget* parent, QString description);
  virtual ~TrackedRecordWidget();

protected slots:
  virtual void checkIfReadySlot() = 0;
  virtual void postProcessingSlot(QString sessionId) = 0;
  virtual void startedSlot() = 0;
  virtual void stoppedSlot() = 0;

protected:
  ssc::TimedTransformMap getRecording(RecordSessionPtr session); ///< gets the tracking data from all relevant tool for the given session
  ToolPtr getTool();

private:
  ToolPtr mTool;
};

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
};

/**
 * USAcqusitionWidget
 *
 * \brief
 *
 * \date Dec 9, 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class USAcqusitionWidget : public TrackedRecordWidget
{
  Q_OBJECT
public:
  USAcqusitionWidget(QWidget* parent);
  virtual ~USAcqusitionWidget();

protected slots:
  void checkIfReadySlot();
  void postProcessingSlot(QString sessionId);
  void startedSlot();
  void stoppedSlot();

private slots:
  void rtSourceChangedSlot();

private:
  SelectRTSourceStringDataAdapterPtr mRTSourceDataAdapter;
  ssc::RealTimeStreamSourcePtr mRTSource;
  ssc::RealTimeStreamSourceRecorderPtr mRTRecorder;
};
}//namespace cx
#endif /* CXRECORDBASEWIDGET_H_ */
