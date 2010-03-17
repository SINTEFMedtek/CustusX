#include "cxRegistrationHistoryWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxMessageManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxView2D.h"
#include "cxView3D.h"

namespace cx
{

RegistrationHistoryWidget::RegistrationHistoryWidget(QWidget* parent) :
    QWidget(parent)
{
  //dock widget
  this->setWindowTitle("RegistrationHistory");

  //layout
  QVBoxLayout* topLayout = new QVBoxLayout(this);
  topLayout->addWidget(new QPushButton("knapp"));
}

RegistrationHistoryWidget::~RegistrationHistoryWidget()
{
}

void RegistrationHistoryWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
//  connect(mDataManager, SIGNAL(dataLoaded()),
//          this, SLOT(populateTreeWidget()));
//  connect(mViewManager, SIGNAL(imageDeletedFromViews(ssc::ImagePtr)),
//          this, SLOT(populateTreeWidget()));
//  this->populateTreeWidget();
}
void RegistrationHistoryWidget::closeEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
//  disconnect(mDataManager, SIGNAL(dataLoaded()),
//             this, SLOT(populateTreeWidget()));
//  disconnect(mViewManager, SIGNAL(imageDeletedFromViews(ssc::ImagePtr)),
//             this, SLOT(populateTreeWidget()));
}


}//end namespace cx
