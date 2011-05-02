#include "cxLayoutEditorTab.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QActionGroup>
#include "cxViewManager.h"

namespace cx
{

LayoutEditorTab::LayoutEditorTab(QWidget* parent) :
  PreferencesTab(parent),
  mLayoutEditorWidget(new LayoutEditorWidget(this)),
  mLayoutLabel(new QLabel("Custom layouts: ")),
  mLayoutList(new QComboBox())
{
  connect(mLayoutList, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(layoutToEditChangedSlot(const QString&)));
}

LayoutEditorTab::~LayoutEditorTab()
{
}

void LayoutEditorTab::saveParametersSlot()
{
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
  layout->addWidget(mLayoutEditorWidget, 1, 0, 1, 2);
  mTopLayout->addLayout(layout);

  LayoutData data = viewManager()->getLayoutData(viewManager()->getActiveLayout());
  this->populateEditableLayouts(data.getUid());
}

void LayoutEditorTab::populateEditableLayouts(QString selectedLayoutUid)
{
  mLayoutList->clear();

  std::vector<QString> layoutUids = viewManager()->getAvailableLayouts();
  for(int i=0; i<layoutUids.size();++i)
  {
    QString uid = layoutUids[i];
    if(viewManager()->isCustomLayout(uid))
    {
      QString name = viewManager()->getLayoutData(uid).getName();
      mLayoutList->addItem(name);
    }
  }

  int findIndex = mLayoutList->findText(selectedLayoutUid);
  if(findIndex != -1)
    mLayoutList->setCurrentIndex(findIndex);
  else
    this->layoutToEditChangedSlot(viewManager()->getActiveLayout());
}

void LayoutEditorTab::layoutToEditChangedSlot(const QString& uid)
{
  LayoutData data;

  std::vector<QString> layoutUids = viewManager()->getAvailableLayouts();
  std::vector<QString>::iterator it = std::find(layoutUids.begin(), layoutUids.end(), uid);
  if(it != layoutUids.end())
  {
    std::cout << "Found layout with uid: " << uid << std::endl;
    data = viewManager()->getLayoutData(uid);
  }
  else
  {
    std::cout << "Did not find layout with uid: " << uid << std::endl;
    data = viewManager()->getLayoutData(viewManager()->getActiveLayout());
  }
  
  if(!viewManager()->isCustomLayout(uid))
    data.resetUid(viewManager()->generateLayoutUid()); //for adding a new layout
  else
   data = viewManager()->getLayoutData(uid); //for editing
  
  mLayoutEditorWidget->setLayoutData(data);
}

}//namespace cx