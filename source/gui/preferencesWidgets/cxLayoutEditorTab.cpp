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
	PreferenceTab(parent),
  mLayoutEditorWidget(new LayoutEditorWidget(this)),
  mLayoutLabel(new QLabel("Custom layouts: ")),
  mLayoutList(new QComboBox()),
  mDeleteButton(new QPushButton(QIcon(":/icons/open_icon_library/png/64x64/actions/edit-delete-2.png"), "")),
  mSaveLayoutButton(new QPushButton("Save layout"))
{
  mLayoutList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  mDeleteButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

  connect(mLayoutList, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(layoutToEditChangedSlot(const QString&)));
  connect(mDeleteButton, SIGNAL(clicked()), this, SLOT(deleteCustomLayoutSlot()));
  connect(mSaveLayoutButton, SIGNAL(clicked()), this, SLOT(saveLayoutSlot()));
}

LayoutEditorTab::~LayoutEditorTab()
{}

void LayoutEditorTab::saveParametersSlot()
{
}

void LayoutEditorTab::init()
{
  QGridLayout* layout = new QGridLayout;
  layout->addWidget(mLayoutLabel, 0, 0);
  layout->addWidget(mLayoutList, 0, 1);
  layout->addWidget(mDeleteButton, 0, 2);
  layout->addWidget(mLayoutEditorWidget, 1, 0, 1, 3);
  layout->addWidget(mSaveLayoutButton, 2, 2, 1, 1);
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
  for(unsigned i=0; i<layoutUids.size();++i)
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
  mSaveLayoutButton->setEnabled(name == "New layout");

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
  QString selectedLayoutUid = mLayoutList->itemData(currentIndex, Qt::ToolTipRole).toString();
  if(!viewManager()->isCustomLayout(selectedLayoutUid))
    return;

  viewManager()->deleteLayoutData(selectedLayoutUid);
  viewManager()->setActiveLayout(viewManager()->getAvailableLayouts().front()); // revert to existing state

  this->setToActiveLayout();

}

void LayoutEditorTab::addCustomLayoutToDisplayList( QString displayName, QString uid )
{
  mLayoutList->addItem(displayName);
  int index = mLayoutList->findText(displayName);
  mLayoutList->setItemData(index, uid, Qt::ToolTipRole);
}

void LayoutEditorTab::saveLayoutSlot()
{
  LayoutData data = mLayoutEditorWidget->getLayoutData();
  if (data.getUid().isEmpty())
    return;
  viewManager()->setLayoutData(data);

  if(viewManager()->getActiveLayout() != data.getUid())
    viewManager()->setActiveLayout(data.getUid());

  this->setToActiveLayout();
}

void LayoutEditorTab::setToActiveLayout()
{
  QString uid = viewManager()->getActiveLayout();
  LayoutData activeLayoutData = viewManager()->getLayoutData(uid);
  this->populateEditableLayouts(activeLayoutData.getUid());
}

}//namespace cx
