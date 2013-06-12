#ifdef __APPLE__
#ifndef CXMACGRABBERSERVERWIDGET_H_
#define CXMACGRABBERSERVERWIDGET_H_

#include "cxGrabberServerWidget.h"

namespace cx
{

/**
 * \class MacGrabberServerWidget
 *
 * \brief Gui for interacting with a MacGrabber and a OpenIGTLinkServer.
 *
 * \date 16. nov. 2010
 * \\author Janne Beate Bakeng, SINTEF
 */

class MacGrabberServerWidget : public GrabberServerWidget
{
  Q_OBJECT

public:
  MacGrabberServerWidget(QWidget* parent);
  virtual ~MacGrabberServerWidget(){};

signals:
  void queueInfo(int size, int dropped);

protected:
  virtual void connectGrabberServer(); ///< Connects to a MacGrabberServer
};

}//namespace cx
#endif /* CXMACGRABBERSERVERWIDGET_H_ */
#endif /*APPLE*/
