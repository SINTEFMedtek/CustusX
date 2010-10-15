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
  Q_OBJECT

public:
  WhatsThisWidget(QWidget* parent) :
    QWidget(parent)
  {};
  virtual ~WhatsThisWidget(){};
  virtual QString defaultWhatsThis() const = 0; ///< Returns a short description of what this widget will do for you.

  QGroupBox* createGroupBox(QWidget* checkedWidget, QWidget* uncheckedWidget, QString boxname, bool checkable = false, bool checked = true)
  {
    QGroupBox* groupbox = new QGroupBox(boxname, this);
    groupbox->setFlat(true);

    groupbox->setCheckable(checkable);
    if(groupbox->isCheckable())
    {
      groupbox->setChecked(checked);
      checkedWidget->setVisible(checked);
      uncheckedWidget->setHidden(checked);
      connect(groupbox, SIGNAL(clicked(bool)), checkedWidget, SLOT(setVisible(bool)));
      connect(groupbox, SIGNAL(clicked(bool)), uncheckedWidget, SLOT(setHidden(bool)));
    }

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(checkedWidget);
    vbox->addWidget(uncheckedWidget);
    groupbox->setLayout(vbox);

    return groupbox;
  }

public slots:
  void adjustSizeSlot()
  {
    this->parentWidget()->adjustSize();
    this->adjustSize();
  }
};
}
#endif /* CXWHATSTHISWIDGET_H_ */
