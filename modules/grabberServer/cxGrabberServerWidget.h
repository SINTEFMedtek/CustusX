#ifndef CXGRABBERSERVERWIDGET_H_
#define CXGRABBERSERVERWIDGET_H_

#include <QObject>
#include <qwidget.h>
#include "cxGrabber.h"
#include "cxServer.h"

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

  MacGrabberPtr getGrabber();
  OpenIGTLinkServerPtr getServer();

private slots:
  void startServerSlot();
  void portChangedSlot(const QString& port);


private:
  void updateInfoLabel();

  QWidget*              mPreviewParent;
  QPushButton*          mStartButton;
  QLineEdit*            mPortEdit;
  
  OpenIGTLinkServerPtr mServer;
  MacGrabberPtr mGrabber;
};

}//namespace cx
#endif /* CXGRABBERSERVERWIDGET_H_ */
