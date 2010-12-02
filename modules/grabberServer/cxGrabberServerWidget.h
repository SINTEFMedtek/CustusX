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
  virtual ~GrabberServerWidget(){};

protected:
  virtual void connectGrabberServer() = 0;

  GrabberServerPtr mGrabberServer;

protected slots:
  void portChangedSlot(const QString& port);

private slots:
  void startServerSlot(bool);
  void grabberServerReadySlot(bool);

private:
  QWidget*              mPreviewParent;
  QPushButton*          mStartButton;
  QLineEdit*            mPortEdit;
};

/**
 * \class MacGrabberServerWidget
 *
 * \brief
 *
 * \date 16. nov. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */

class MacGrabberServerWidget : public GrabberServerWidget
{
  Q_OBJECT

public:
  MacGrabberServerWidget(QWidget* parent);
  virtual ~MacGrabberServerWidget(){};

protected:
  virtual void connectGrabberServer();
};

}//namespace cx
#endif /* CXGRABBERSERVERWIDGET_H_ */
