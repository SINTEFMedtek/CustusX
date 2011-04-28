#include "cxSelectionGroupBox.h"

#include <QComboBox>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>

namespace cx
{
SelectionGroupBox::SelectionGroupBox(QString title, QStringList selectionList, bool exclusive, QWidget* parent) :
    QGroupBox(parent),
    mSelectionList(selectionList),
    mButtonGroup(new QButtonGroup(parent))
{
  mLayout = new QHBoxLayout(this);
  this->setTitle(title);

  this->populate(exclusive);
}

SelectionGroupBox::~SelectionGroupBox()
{}

QStringList SelectionGroupBox::getSelected()
{
  QStringList retval;
  QList<QAbstractButton*> applicationButtonList = mButtonGroup->buttons();
  foreach(QAbstractButton* button, applicationButtonList)
  {
    if(button->isChecked())
      retval << button->text();
  }
  return retval;
}

void SelectionGroupBox::setSelected(QStringList selectedlist)
{
  this->filter(selectedlist);
}

void SelectionGroupBox::setEnabledButtons(bool value)
{
  QList<QAbstractButton*> applicationButtonList = mButtonGroup->buttons();
  foreach(QAbstractButton* button, applicationButtonList)
  {
    button->setEnabled(value);
  }
}

void SelectionGroupBox::populate(bool exclusive)
{
  mButtonGroup->setExclusive(exclusive);
  foreach(QString string, mSelectionList)
  {
    if(string.isEmpty())
      continue;

    string = string.toLower();
    string[0] = string[0].toUpper();

    QCheckBox* box = new QCheckBox(string);
    mButtonGroup->addButton(box);
    mLayout->addWidget(box);

    //need to tell the outside world that the state of a button changed
    connect(box, SIGNAL(stateChanged(int)), this, SIGNAL(selectionChanged()));
    connect(box, SIGNAL(clicked(bool)), this, SIGNAL(userClicked()));
  }
  mLayout->addStretch();
}

void SelectionGroupBox::filter(QStringList filter)
{
  bool exclusive = mButtonGroup->exclusive();

  mButtonGroup->setExclusive(false);
  QList<QAbstractButton*> applicationButtonList = mButtonGroup->buttons();
  foreach(QAbstractButton* button, applicationButtonList)
  {
    bool on = filter.contains(button->text(), Qt::CaseInsensitive);
    button->setChecked(on);
  }
  mButtonGroup->setExclusive(exclusive);
}
}//namespace cx

