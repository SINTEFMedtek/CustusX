#ifdef WIN32
#ifndef CXWINGRABBERSERVERWIDGET_H_
#define CXWINGRABBERSERVERWIDGET_H_

#include "cxGrabberServerWidget.h"

namespace cx
{

/**
 * \class WinGrabberServerWidget
 *
 * \brief Gui for interacting with a WinGrabber and a OpenIGTLinkServer.
 *
 * \date 16. nov. 2010
 * \\author Janne Beate Bakeng, SINTEF
 */

class WinGrabberServerWidget : public GrabberServerWidget
{
  Q_OBJECT

public:
  WinGrabberServerWidget(QWidget* parent);
  virtual ~WinGrabberServerWidget(){};

signals:
  void queueInfo(int size, int dropped);

protected:
  virtual void connectGrabberServer(); ///< Connects to a WinGrabberServer
};

}//namespace cx
#endif /* CXWINGRABBERSERVERWIDGET_H_ */
#endif /*WIN32*/
