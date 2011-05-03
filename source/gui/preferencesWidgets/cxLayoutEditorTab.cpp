#include "cxLayoutEditorTab.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QActionGroup>
#include "cxViewManager.h"

namespace cx
{

LayoutEditorTab::LayoutEditorTab(QWidget* parent) :
  PreferencesTab(parent),
  mLayoutEditorWidget(new LayoutEditorWidget(this)),
  mLayoutLabel(new QLabel("Custom layouts: ")),
  mLayoutList(new QComboBox()),
  mDeleteButton(new QPushButton(QIcon(":/icons/open_icon_library/png/64x64/actions/edit-delete-2.png"), ""))
{
  mLayoutList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  mDeleteButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

  connect(mLayoutList, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(layoutToEditChangedSlot(const QString&)));
  connect(mDeleteButton, SIGNAL(clicked()), this, SLOT(deleteCustomLayoutSlot()));
}

LayoutEditorTab::~LayoutEditorTab()
{}

void LayoutEditorTab::saveParametersSlot()
{
  //TODO when do we want to save???

  LayoutData data = mLayoutEditorWidget->getLayoutData();
  if (data.getUid().isEmpty())
    return;
  viewManager()->setLayoutData(data);

  if(viewManager()->getActiveLayout() != data.getUid())
   viewManager()->setActiveLayout(data.getUid());
}

void LayoutEditorTab::init()
{
  QGridLayout* layout = new QGridLayout;
  layout->addWidget(mLayoutLabel, 0, 0);
  layout->addWidget(mLayoutList, 0, 1);
  layout->addWidget(mDeleteButton, 0, 2);
  layout->addWidget(mLayoutEditorWidget, 1, 0, 1, 3);
  mTopLayout->addLayout(layout);

  LayoutData data = viewManager()->getLayoutData(viewManager()->getActiveLayout());
  this->populateEditableLayouts(data.getUid());
}

void LayoutEditorTab::populateEditableLayouts(QString selectedLayoutUid)
{
  mLayoutList->clear();

  QString newName = "New layout";
  QString newUid = newName;
  this->addCustomLayoutToDisplayList(newName, newUid);

  std::vector<QString> layoutUids = viewManager()->getAvailableLayouts();
  for(int i=0; i<layoutUids.size();++i)
  {
    QString uid = layoutUids[i];
    if(viewManager()->isCustomLayout(uid))
    {
      QString name = viewManager()->getLayoutData(uid).getName();
      this->addCustomLayoutToDisplayList(name, uid);
    }
  }

  int findIndex = mLayoutList->findData(selectedLayoutUid, Qt::ToolTipRole);
  if(findIndex == -1)
    findIndex = mLayoutList->findData(newUid, Qt::ToolTipRole);
  
  mLayoutList->setCurrentIndex(findIndex);

}

void LayoutEditorTab::layoutToEditChangedSlot(const QString& name)
{
  int index = mLayoutList->findText(name);
  QString uid = mLayoutList->itemData(index, Qt::ToolTipRole).toString();

  LayoutData data;

  //std::cout << "================ " << name << " =================" << std::endl;
  std::vector<QString> layoutUids = viewManager()->getAvailableLayouts();
  std::vector<QString>::iterator it = std::find(layoutUids.begin(), layoutUids.end(), uid);
  if(it != layoutUids.end())
  {
    //std::cout << "Found layout in viewmanager: " << *it << std::endl;
    data = viewManager()->getLayoutData(uid);
  }
  else
  {
    //std::cout << "Did not find layout in viewmanager: " << name << std::endl;
    data = viewManager()->getLayoutData(viewManager()->getActiveLayout());
  }
  
  if(!viewManager()->isCustomLayout(data.getUid()))
  {
    //std::cout << "It is not a custom layout. New layout mode." << std::endl;
    data.resetUid(viewManager()->generateLayoutUid()); //for adding a new layout
  }
  else
  {
    //std::cout << "It is a custom layout. Editing mode." << std::endl;
   data = viewManager()->getLayoutData(data.getUid()); //for editing
  }
  //std::cout << "=================================" << std::endl;
  
  mLayoutEditorWidget->setLayoutData(data);
  viewManager()->setActiveLayout(data.getUid());
}

void LayoutEditorTab::deleteCustomLayoutSlot()
{
  int currentIndex = mLayoutList->currentIndex();
  QString selectedLayoutName = mLayoutList->itemText(currentIndex);
  QString selectedLayoutUid = mLayoutList->itemData(currentIndex, Qt::ToolTipRole).toString();
  if(!viewManager()->isCustomLayout(selectedLayoutUid))
    return;

  //std::cout << "Going to erase " << selectedLayoutUid << std::endl;

  viewManager()->deleteLayoutData(selectedLayoutUid);
  viewManager()->setActiveLayout(viewManager()->getAvailableLayouts().front()); // revert to existing state

  QString uid = viewManager()->getActiveLayout();
  //std::cout << "Active layout uid is now " << uid << std::endl;
  LayoutData activeLayoutData = viewManager()->getLayoutData(uid);
  this->populateEditableLayouts(activeLayoutData.getUid());
}

void LayoutEditorTab::addCustomLayoutToDisplayList( QString displayName, QString uid )
{
  mLayoutList->addItem(displayName);
  int index = mLayoutList->findText(displayName);
  mLayoutList->setItemData(index, uid, Qt::ToolTipRole);
}

}//namespace cx