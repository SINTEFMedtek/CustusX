#ifndef CXCONSOLEWIDGET_H_
#define CXCONSOLEWIDGET_H_

#include <QWidget>
#include "sscMessageManager.h"

class QTextBrowser;

namespace cx
{
/**
 * \class ConsoleWidget
 *
 * \brief
 *
 * \date 24. aug. 2010
 * \author: Janne Beate Bakeng, SINTEF Medisinsk Teknologi
 */
class ConsoleWidget : public QWidget
{
  typedef ssc::Message Message;

  Q_OBJECT

public:
  ConsoleWidget(QWidget* parent);
  ~ConsoleWidget();

private slots:
  void printMessage(Message message);

private:
  QTextBrowser* mTextBrowser;
};
} // namespace cx
#endif /* CXCONSOLEWIDGET_H_ */
