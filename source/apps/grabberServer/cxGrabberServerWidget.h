#ifndef CXGRABBERSERVERWIDGET_H_
#define CXGRABBERSERVERWIDGET_H_

#include <QObject>
#include <QWidget>
#include "cxGrabberServer.h"

class QPushButton;
class QLabel;
class QLineEdit;

namespace cx
{
/**
 * \class GrabberServerWidget
 *
 * \brief Abstract interface for gui for interacting with a grabber and a server.
 *
 * \date 16. nov. 2010
 * \\author Janne Beate Bakeng, SINTEF
 */

class GrabberServerWidget : public QWidget
{
  Q_OBJECT

public:
  GrabberServerWidget(QWidget* parent);
  virtual ~GrabberServerWidget(){};

public slots:
  void startServerSlot(bool); ///< Starts/stops the grabberserver

protected:
  virtual void connectGrabberServer() = 0; ///< Connects to a specific grabberserver. Must be implemented by subclasses.

  GrabberServerPtr mGrabberServer;

protected slots:
  void portChangedSlot(const QString& port); ///< Reacts to when the user changes the specific server port

private slots:
  void grabberServerReadySlot(bool); ///< Updates the gui when the grabberserver is ready

private:
  QWidget*              mPreviewParent; ///< Preview of grabbed stream
  QPushButton*          mStartButton; ///< The start/stop button
  QLineEdit*            mPortEdit; ///< Editable line for the port nr
};

}//namespace cx
#endif /* CXGRABBERSERVERWIDGET_H_ */
