#ifndef CXREGISTRATIONHISTORYWIDGET_H_
#define CXREGISTRATIONHISTORYWIDGET_H_

#include <QtGUI>

namespace cx
{

/**
 * \class BrowserWidget
 *
 * Buttons for rolling back to previous registrations.
 * Might be enhanced to a graphical timeline.
 *
 * \date 2010.03.17
 * \author: Christian Askeland, SINTEF
 */
class RegistrationHistoryWidget : public QWidget
{
  Q_OBJECT

public:
  RegistrationHistoryWidget(QWidget* parent);
  virtual ~RegistrationHistoryWidget();

signals:

protected slots:

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void closeEvent(QCloseEvent* event); ///<disconnects stuff

private:
  RegistrationHistoryWidget();
};

}//end namespace cx

#endif /* CXREGISTRATIONHISTORYWIDGET_H_ */
