#ifndef CXRECORDSESSIONWIDGET_H_
#define CXRECORDSESSIONWIDGET_H_

#include "cxBaseWidget.h"
#include "cxAcquisitionData.h"

class QPushButton;
class QLineEdit;
class QLabel;

namespace cx
{
/**
* \file
* \addtogroup cxPluginAcquisition
* @{
*/

/**
 * \class RecordSessionWidget
 *
 * \brief
 *
 * \date Dec 8, 2010
 * \author Janne Beate Bakeng
 */
class RecordSessionWidget : public BaseWidget
{
  Q_OBJECT

public:
  RecordSessionWidget(AcquisitionPtr base, QWidget* parent, QString defaultDescription = "Record Session");
  virtual ~RecordSessionWidget();

  virtual QString defaultWhatsThis() const;

  void setDescription(QString text);
  void setDescriptionVisibility(bool value);

//  void startPostProcessing(QString description);
//  void stopPostProcessing();

//signals:
//  void started(QString);
//  void stopped(bool canceled=false);
//  void newSession(QString);

public slots:
	void setReady(bool val, QString text);

//protected:
//  void changeEvent(QEvent* event);

private slots:
  void startStopSlot(bool);
  void cancelSlot();
  void recordStateChangedSlot();
  void readinessChangedSlot();

private:
//  void startRecording();
//  void stopRecording();
//  bool isRecording();

//  void reset();

  AcquisitionPtr mBase;
//  AcquisitionDataPtr mPluginData;
  QLabel* mInfoLabel;
  QPushButton* mStartStopButton;
  QPushButton* mCancelButton;
  QLabel* mDescriptionLabel;
  QLineEdit* mDescriptionLine;
//  double mStartTimeMSec;
//  double mStopTimeMSec;
//  bool mPostProcessing;

//  RecordSessionPtr mCurrentSession;
};

/**
* @}
*/
}//namespace cx
#endif /* CXRECORDSESSIONWIDGET_H_ */
