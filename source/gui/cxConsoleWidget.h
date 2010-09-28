#ifndef CXCONSOLEWIDGET_H_
#define CXCONSOLEWIDGET_H_

#include "cxWhatsThisWidget.h"
#include <QTextCharFormat>
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
class ConsoleWidget : public WhatsThisWidget
{
  typedef ssc::Message Message;

  Q_OBJECT

public:
  ConsoleWidget(QWidget* parent);
  ~ConsoleWidget();
  virtual QString defaultWhatsThis() const;

private slots:
  void printMessage(Message message); ///< prints the message into the console

private:
  void createTextCharFormats(); ///< sets up the formating rules for the message levels
  void format(Message& message); ///< formats the text to suit the message level

  QTextBrowser* mTextBrowser;

  std::map<ssc::MESSAGE_LEVEL, QTextCharFormat> mFormat;
//  QTextCharFormat        mInfoFormat;
//  QTextCharFormat        mWarningFormat;
//  QTextCharFormat        mErrorFormat;
//  QTextCharFormat        mDebugFormat;
};
} // namespace cx
#endif /* CXCONSOLEWIDGET_H_ */
