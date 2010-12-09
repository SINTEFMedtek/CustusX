#ifndef CXRECORDSESSIONWIDGET_H_
#define CXRECORDSESSIONWIDGET_H_

#include <QWidget>

class QPushButton;

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
  RecordSessionWidget(QWidget* parent);
  ~RecordSessionWidget();

private slots:
  void startStopSlot(bool);

private:
  void startRecording();
  void stopRecording();

  QPushButton* mStartStopButton;
  double mStartTime; //in MICROseconds since epoch
  double mStopTime; //in MICROseconds since epoch
};

}//namespace cx
#endif /* CXRECORDSESSIONWIDGET_H_ */
