#ifndef CXWHATSTHISWIDGET_H_
#define CXWHATSTHISWIDGET_H_

#include <QWidget>
#include <QTabWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>

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
  WhatsThisWidget(QWidget* parent, QString objectName, QString windowTitle) :
    QWidget(parent), mObjectName(objectName),mWindowTitle(windowTitle)
  {};

  virtual ~WhatsThisWidget(){};
  virtual QString defaultWhatsThis() const = 0; ///< Returns a short description of what this widget will do for you.

  QWidget* createMethodWidget(QWidget* inputWidget, QWidget* outputWidget, QString methodname, bool inputChecked = false, bool outputVisible = true)
  {
    QWidget* retval = new QWidget(this);
    QVBoxLayout* toplayout = new QVBoxLayout(retval);
    QGridLayout* layout = new QGridLayout();
    toplayout->addLayout(layout);
    toplayout->addStretch();

    QLabel* methodLabel = new QLabel("<b>"+methodname+"</b>");
    QCheckBox* checkBox = new QCheckBox("generate");

    inputWidget->setVisible(inputChecked);
    outputWidget->setVisible(outputVisible);
    connect(checkBox, SIGNAL(clicked(bool)), inputWidget, SLOT(setVisible(bool)));

    layout->addWidget(methodLabel, 0, 0);
    layout->addWidget(checkBox, 0, 1);
    layout->addWidget(inputWidget, 1, 0, 1, 2);
    layout->addWidget(outputWidget, 2, 0, 1, 2);

    return retval;
  }

  QGroupBox* createGroupbox(QWidget* widget, QString boxname)
  {
    QGroupBox* retval = new QGroupBox(this);
    QVBoxLayout* toplayout = new QVBoxLayout(retval);

    QLabel* nameLabel = new QLabel(boxname);
    toplayout->addWidget(nameLabel);
    toplayout->addWidget(widget);

    return retval;
  }

  QFrame* createHorizontalLine() ///< creates a horizontal line witch can be inserted into widgets
  {
    QFrame* retval = new QFrame();
    retval->setFrameStyle( QFrame::Sunken + QFrame::HLine );
    retval->setFixedHeight( 12 );
    return retval;
  }

public slots:
  void adjustSizeSlot()
  {
    this->parentWidget()->adjustSize();
    this->adjustSize();
  }

protected:
  virtual void showEvent(QShowEvent* event)
  {
    QWidget::showEvent(event);
    this->setObjectName(mObjectName);
    this->setWindowTitle(mWindowTitle);
    this->setWhatsThis(this->defaultWhatsThis());
  }

private:
  QString mObjectName;
  QString mWindowTitle;
};
}
#endif /* CXWHATSTHISWIDGET_H_ */
