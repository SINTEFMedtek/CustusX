// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxDataSelectWidget.h"

#include "cxViewManager.h"
#include "cxViewGroup.h"
#include "cxViewWrapper.h"
#include "sscDataManager.h"
#include "sscHelperWidgets.h"

namespace cx
{

DataSelectWidget::DataSelectWidget(QWidget* parent, SelectDataStringDataAdapterBasePtr data) :
    BaseWidget(parent, "DataSelectWidget", "DataSelectWidget"),
    mData(data)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    QWidget* dataAdapter = ssc::createDataWidget(this, mData);
    layout->addWidget(dataAdapter);

    mToggleShowAction = this->createAction(this,
                    QIcon(":/icons/open_icon_library/png/64x64/others/eye.png.png"),
                    "Toggle show data in view", "",
                    SLOT(toggleShowData()),
                    NULL);
    mToggleShowAction->setCheckable(true);
    CXSmallToolButton* toggleShowButton = new CXSmallToolButton();
    toggleShowButton->setDefaultAction(mToggleShowAction);
    layout->addWidget(toggleShowButton);

    mRemoveAction = this->createAction(this,
                                       QIcon(":/icons/preset_remove.png"),
                                       "<html><h4>Permanently delete data.</h4><p>Press button twice to delete.<br>"
                                       "Right-click after the first click to cancel.<p></html>"
                                       , "",
                                       SLOT(requestEraseData()),
                                       NULL);
    mRemoveAction->setCheckable(true);
    EraseDataToolButton* removeButton = new EraseDataToolButton(this);
    connect(removeButton, SIGNAL(rightClick()), this, SLOT(cancelRemovalSlot()));
    removeButton->setDefaultAction(mRemoveAction);
    layout->addWidget(removeButton);

    connect(viewManager(), SIGNAL(activeViewChanged()), this, SLOT(viewGroupChangedSlot()));
    connect(mData.get(), SIGNAL(changed()), this, SLOT(updateDataVisibility()));
}

ViewGroupDataPtr DataSelectWidget::getActiveViewGroupData()
{
    int groupIdx = viewManager()->getActiveViewGroup();
    if (groupIdx<0)
        groupIdx = 0;
    return viewManager()->getViewGroups()[groupIdx]->getData();
}

void DataSelectWidget::viewGroupChangedSlot()
{
    ViewGroupDataPtr group = this->getActiveViewGroupData();
    if (mCurrentViewGroup==group)
        return;

    if (mCurrentViewGroup)
    {
        disconnect(mCurrentViewGroup.get(), SIGNAL(dataAdded(QString)),   this, SLOT(updateDataVisibility()));
        disconnect(mCurrentViewGroup.get(), SIGNAL(dataRemoved(QString)), this, SLOT(updateDataVisibility()));
    }

    mCurrentViewGroup = group;

    if (mCurrentViewGroup)
    {
        connect(mCurrentViewGroup.get(), SIGNAL(dataAdded(QString)),   this, SLOT(updateDataVisibility()));
        connect(mCurrentViewGroup.get(), SIGNAL(dataRemoved(QString)), this, SLOT(updateDataVisibility()));
    }

    this->updateDataVisibility();
}

void DataSelectWidget::updateDataVisibility()
{
    mToggleShowAction->setEnabled(mData->getData());
    mRemoveAction->setEnabled(mData->getData());
    if (!mData->getData())
        return;

    std::vector<ssc::DataPtr> visibleData = mCurrentViewGroup->getData();
    bool visible = std::count(visibleData.begin(), visibleData.end(), mData->getData());

    mToggleShowAction->setChecked(visible);
    this->cancelRemovalSlot();
}

QString DataSelectWidget::defaultWhatsThis() const
{
    return "";
}

/** If button is checked (first press), do nothing, except
  * change button icon to show remove in progress.
  * If button is unchecked (second press), remove data.
  */
void DataSelectWidget::requestEraseData()
{
    if (mRemoveAction->isChecked())
        return;
    if (!mData->getData())
        return;

//    std::cout << mRemoveAction->isChecked() << " DataSelectWidget::requestEraseData() removing " << mData->getData()->getUid() << std::endl;
    ssc::dataManager()->removeData(mData->getData()->getUid());
}

/** Uncheck the remove button without triggering a remove.
  *
  */
void DataSelectWidget::cancelRemovalSlot()
{
    mRemoveAction->blockSignals(true);
    mRemoveAction->setChecked(false);
    mRemoveAction->blockSignals(false);
}

void DataSelectWidget::toggleShowData()
{
    if (!mData->getData())
        return;

    if (mToggleShowAction->isChecked())
        mCurrentViewGroup->addData(mData->getData());
    else
        mCurrentViewGroup->removeData(mData->getData());
}


} // namespace cx


