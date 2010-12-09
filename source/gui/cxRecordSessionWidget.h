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
  ~RecordSessionWidget();

private slots:
  void startStopSlot(bool);

private:
  void startRecording();
  void stopRecording();

  QPushButton* mStartStopButton;
  QLineEdit* mDescriptionLine;
  double mStartTime; //in milliseconds since epoch
  double mStopTime; //in milliseconds since epoch
};

}//namespace cx
#endif /* CXRECORDSESSIONWIDGET_H_ */
