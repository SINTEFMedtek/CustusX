#ifndef CXGRABBERSERVERPROPERTIESWIDGET_H_
#define CXGRABBERSERVERPROPERTIESWIDGET_H_

#include <QWidget>
#include "cxGrabber.h"
#include "cxServer.h"

class QGridLayout;

namespace cx
{
/**
 * \class GrabberServerPropertiesWidget
 *
 * \brief
 *
 * \date 1. des. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */

class GrabberServerPropertiesWidget : public QWidget
{
  Q_OBJECT

public:
  GrabberServerPropertiesWidget(QWidget* parent);
  ~GrabberServerPropertiesWidget();
  
  void setGrabber(MacGrabberPtr grabber);
  void setServer(OpenIGTLinkServerPtr server);

private:
  QGridLayout*  mLayout;
  MacGrabberPtr mGrabber;
  OpenIGTLinkServerPtr mServer;
};

}//namespace cx
#endif /* CXGRABBERSERVERPROPERTIESWIDGET_H_ */
