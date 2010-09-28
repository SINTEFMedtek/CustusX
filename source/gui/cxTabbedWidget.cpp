#include "cxTabbedWidget.h"

#include <QTabWidget>
#include <QVBoxLayout>

namespace cx
{

TabbedWidget::TabbedWidget(QString objectName, QString windowTitle, QWidget* parent) :
    QWidget(parent),
    mTabWidget(new QTabWidget(this))
{
  this->setObjectName(objectName);
  this->setWindowTitle(windowTitle);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mTabWidget);
}

TabbedWidget::~TabbedWidget()
{}

void TabbedWidget::addTab(QWidget* newTab, QString newTabName, QString tabsWhatThis)
{
  int index = mTabWidget->addTab(newTab, newTabName);
  mTabWidget->setTabWhatsThis(index, tabsWhatThis);
}

QString TabbedWidget::defaultWhatThis()
{
  return "";
}

} //namespace cx
