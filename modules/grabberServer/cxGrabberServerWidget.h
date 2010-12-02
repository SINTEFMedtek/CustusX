#ifndef CXGRABBERSERVERWIDGET_H_
#define CXGRABBERSERVERWIDGET_H_

#include <QObject>
#include <qwidget.h>
#include "cxGrabberServer.h"

class QPushButton;
class QLabel;
class QLineEdit;

namespace cx
{
/**
 * \class GrabberServerWidget
 *
 * \brief
 *
 * \date 16. nov. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */

class GrabberServerWidget : public QWidget
{
  Q_OBJECT
public:
  GrabberServerWidget(QWidget* parent);
  virtual ~GrabberServerWidget();

private slots:
  void startServerSlot(bool);
  void portChangedSlot(const QString& port);
  void serverReadySlot(bool);


private:
  void updateInfoLabel();

  QWidget*              mPreviewParent;
  QPushButton*          mStartButton;
  QLineEdit*            mPortEdit;
  
  GrabberServerPtr mGrabberServer;
};

}//namespace cx
#endif /* CXGRABBERSERVERWIDGET_H_ */
