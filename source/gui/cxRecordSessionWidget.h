#ifndef CXRECORDSESSIONWIDGET_H_
#define CXRECORDSESSIONWIDGET_H_

#include <QWidget>

class QPushButton;
class QLineEdit;

namespace cx
{
/**
 * RecordSessionWidget
 *
 * \brief
 *
 * \date Dec 8, 2010
 * \author Janne Beate Bakeng
 */
class RecordSessionWidget : public QWidget
{
  Q_OBJECT

public:
  RecordSessionWidget(QWidget* parent, QString defaultDescription = "Record Session");
  virtual ~RecordSessionWidget();

signals:
  void started();
  void stopped();
  void newSession(QString);

protected:
  void changeEvent(QEvent* event);

private slots:
  void startStopSlot(bool);
  void cancelSlot();

private:
  void startRecording();
  void stopRecording();
  bool isRecording();

  void reset();

  QPushButton* mStartStopButton;
  QPushButton* mCancelButton;
  QLineEdit* mDescriptionLine;
  double mStartTimeMSec;
  double mStopTimeMSec;
};

}//namespace cx
#endif /* CXRECORDSESSIONWIDGET_H_ */
