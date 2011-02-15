#ifndef CXRECORDBASEWIDGET_H_
#define CXRECORDBASEWIDGET_H_

#include <QWidget>
#include "sscTool.h"
#include "cxDataInterface.h"
#include "sscRTSourceRecorder.h"
#include "sscReconstructer.h"
#include "cxUsReconstructionFileMaker.h"

class QLabel;
class QVBoxLayout;
class QDoubleSpinBox;
class QPushButton;

namespace cx
{

/** Interface to the tool offset of the dominant tool
 */
class DoubleDataAdapterTimeCalibration : public ssc::DoubleDataAdapter
{
  Q_OBJECT
public:
  static ssc::DoubleDataAdapterPtr New();
  DoubleDataAdapterTimeCalibration();
  virtual ~DoubleDataAdapterTimeCalibration() {}
  virtual QString getValueName() const { return "Temporal Calibration"; }
  virtual double getValue() const;
  virtual QString getHelp() const;
  virtual bool setValue(double val);
  ssc::DoubleRange getValueRange() const;

private slots:
  void dominantToolChanged();

private:
  ssc::ToolPtr mTool;
};

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
  RecordSessionWidget* mRecordSessionWidget;

private:
  QLabel* mInfoLabel;
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

signals:
  void toolChanged();

protected slots:
  virtual void checkIfReadySlot() = 0;
  virtual void postProcessingSlot(QString sessionId) = 0;
  virtual void startedSlot() = 0;
  virtual void stoppedSlot() = 0;

protected:
  virtual ssc::TimedTransformMap getRecording(RecordSessionPtr session) = 0; ///< gets the tracking data from all relevant tool for the given session
  void setTool(ToolPtr tool);
  ToolPtr getTool();

private:
  ToolPtr mTool;
};

}//namespace cx
#endif /* CXRECORDBASEWIDGET_H_ */
