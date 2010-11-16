#ifndef CXGRABBERSERVERWIDGET_H_
#define CXGRABBERSERVERWIDGET_H_

#include <QObject>
#include <QMacCocoaViewContainer>

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

class GrabberServerWidget : public QMacCocoaViewContainer
{
  Q_OBJECT
public:
  GrabberServerWidget(QWidget* parent);
  virtual ~GrabberServerWidget();

private slots:
  void startServerSlot();

private:
  void updateInfoLabel();

  QPushButton* mStartButton;
  QLabel*      mInfoLabel;
  
//  GrabberPreviewWidget* mPreviewWidget;
//  OpenIGTLinkServerPtr mServer;
  GrabberPtr mGrabber;
};

}//namespace cx
#endif /* CXGRABBERSERVERWIDGET_H_ */
