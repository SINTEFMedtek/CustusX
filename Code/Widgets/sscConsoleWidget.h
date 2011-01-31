#ifndef SSCCONSOLEWIDGET_H_
#define SSCCONSOLEWIDGET_H_

#include <QTextBrowser>
#include <QTextCharFormat>
#include "sscMessageManager.h"

class QContextMenuEvent;
class QAction;

namespace ssc
{
/**
 * \class ConsoleWidget
 *
 * \brief
 *
 * \date 24. aug. 2010
 * \author: Janne Beate Bakeng, SINTEF Medisinsk Teknologi
 */
class ConsoleWidget : public QTextBrowser
{
  typedef ssc::Message Message;

  Q_OBJECT

public:
  ConsoleWidget(QWidget* parent);
  ~ConsoleWidget();
  virtual QString defaultWhatsThis() const;

protected slots:
  void contextMenuEvent(QContextMenuEvent* event);
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget

private slots:
  void printMessage(Message message); ///< prints the message into the console
  void lineWrappingSlot(bool checked);

private:
  void createTextCharFormats(); ///< sets up the formating rules for the message levels
  void format(Message& message); ///< formats the text to suit the message level

  QAction* mLineWrappingAction;

  std::map<ssc::MESSAGE_LEVEL, QTextCharFormat> mFormat;
};
} // namespace ssc
#endif /* SSCCONSOLEWIDGET_H_ */
