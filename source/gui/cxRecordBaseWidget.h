#ifndef CXRECORDBASEWIDGET_H_
#define CXRECORDBASEWIDGET_H_

#include <QWidget>
#include "sscTool.h"
#include "cxDataInterface.h"
#include "sscRealTimeStreamSourceRecorder.h"
#include "sscReconstructer.h"
#include "cxUsReconstructionFileMaker.h"

 #include <QFuture>
 #include <QFutureWatcher>

class QLabel;
class QVBoxLayout;
class QDoubleSpinBox;
class QPushButton;

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
  void reconstructFinishedSlot();
  void fileMakerWriteFinished();

private:
  SelectRTSourceStringDataAdapterPtr mRTSourceDataAdapter;
  ssc::RealTimeStreamSourcePtr mRTSource;
  ssc::RealTimeStreamSourceRecorderPtr mRTRecorder;
  ssc::ThreadedReconstructerPtr mThreadedReconstructer;

  QFuture<QString> mFileMakerFuture;
  QFutureWatcher<QString> mFileMakerFutureWatcher;
  UsReconstructionFileMakerPtr mFileMaker;
};

class SoundSpeedConverterWidget : public QWidget
{
  Q_OBJECT

public:
  SoundSpeedConverterWidget(QWidget* parent);
  ~SoundSpeedConverterWidget();

  double getSoundSpeedCompensationFactor(); ///< calculates the sound speed conversion factor
  double getWaterSoundSpeed(); ///<

public slots:
  void applySoundSpeedCompensationFactorSlot(); ///< sets the sounds speed conversion factor on the rt source

private slots:
  void waterSoundSpeedChangedSlot();
  void waterDegreeChangedSlot();

private:
  const double mFromSoundSpeed; //m/s
  double mToSoundSpeed; //m/s

  QPushButton*    mApplyButton; //applies the compensation on the rt source
  QDoubleSpinBox* mSoundSpeedSpinBox; //m/s
  QDoubleSpinBox* mWaterDegreeSpinBox; //celsius
};
}//namespace cx
#endif /* CXRECORDBASEWIDGET_H_ */
