#ifndef CXWHATSTHISWIDGET_H_
#define CXWHATSTHISWIDGET_H_

#include <QWidget>
#include <QTabWidget>
#include <QGroupBox>
#include <QVBoxLayout>

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
  QGroupBox* createGroupBox(QWidget* widget, QString boxname, bool checkable = false, bool checked = true)
  {
    QGroupBox* groupbox = new QGroupBox(boxname, this);

    groupbox->setCheckable(checkable);
    if(groupbox->isCheckable())
    {
      groupbox->setChecked(checked);
      connect(groupbox, SIGNAL(clicked(bool)), widget, SLOT(setVisible(bool)));
    }

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(widget);
    groupbox->setLayout(vbox);

    return groupbox;
  }
};
}
#endif /* CXWHATSTHISWIDGET_H_ */
