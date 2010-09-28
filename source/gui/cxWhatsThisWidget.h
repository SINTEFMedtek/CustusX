#ifndef CXWHATSTHISWIDGET_H_
#define CXWHATSTHISWIDGET_H_

#include <QWidget>
#include <QTabWidget>

namespace cx
{
/**
 * \class WhatsThisWidget
 *
 * \brief Interface for QWidget which forces the user to write a whats this message.
 *
 * \date 28. sep. 2010
 * \author: Janne Beate Bakeng
 */

class WhatsThisWidget : public QWidget
{
public:
  WhatsThisWidget(QWidget* parent) :
    QWidget(parent)
  {};
  virtual ~WhatsThisWidget(){};
  virtual QString defaultWhatsThis() const = 0; ///< Returns a short description of what this widget will do for you.
};
}
#endif /* CXWHATSTHISWIDGET_H_ */
