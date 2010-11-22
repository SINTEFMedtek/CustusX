#ifndef CXGRABBERSERVERWIDGET_H_
#define CXGRABBERSERVERWIDGET_H_

#include <QObject>
#include <qwidget.h>

#include "cxGrabber.h"

class QPushButton;
class QLabel;

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
  void startServerSlot();

private:
  void updateInfoLabel();

  QWidget*     mPreviewParent;
  QPushButton* mStartButton;
  QLabel*      mInfoLabel;
  
//  GrabberPreviewWidget* mPreviewWidget;
//  OpenIGTLinkServerPtr mServer;
  MacGrabber* mGrabber;
};

}//namespace cx
#endif /* CXGRABBERSERVERWIDGET_H_ */
