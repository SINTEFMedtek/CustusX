/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxDataSelectWidget.h"

#include "cxHelperWidgets.h"
#include "cxSelectDataStringProperty.h"
#include "cxTypeConversions.h"
#include "cxViewService.h"
#include "cxPatientModelService.h"
#include "cxData.h"
#include "cxViewGroupData.h"

namespace cx
{

DataSelectWidget::DataSelectWidget(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget *parent, SelectDataStringPropertyBasePtr data, QGridLayout* gridLayout, int row) :
    BaseWidget(parent, "DataSelectWidget", "DataSelectWidget"),
	mData(data),
	mViewService(viewService),
	mPatientModelService(patientModelService)
{

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

	QWidget* widget = sscCreateDataWidget(this, mData, gridLayout, row);

    mToggleShowAction = this->createAction(this,
                    QIcon(":/icons/open_icon_library/eye.png.png"),
                    "Toggle show data in view", "",
                    SLOT(toggleShowData()),
                    NULL);
    mToggleShowAction->setCheckable(true);
    CXSmallToolButton* toggleShowButton = new CXSmallToolButton();
    toggleShowButton->setDefaultAction(mToggleShowAction);

    mRemoveAction = this->createAction(this,
                                       QIcon(),
                                       "<html><h4>Permanently delete data.</h4><p>Press button twice to delete.<br>"
                                       "Right-click after the first click to cancel.<p></html>"
                                       , "",
                                       SLOT(requestEraseData()),
                                       NULL);
    mRemoveAction->setCheckable(true);
    EraseDataToolButton* removeButton = new EraseDataToolButton(this);
    connect(removeButton, SIGNAL(rightClick()), this, SLOT(cancelRemovalSlot()));
    removeButton->setDefaultAction(mRemoveAction);

    if(gridLayout)
    {
    	gridLayout->setMargin(0);
    	gridLayout->setSpacing(0);
    	QHBoxLayout* lay = new QHBoxLayout;
    	lay->addWidget(toggleShowButton);
    	lay->addWidget(removeButton);
    	gridLayout->addLayout(lay, row, 2);
    }else
    {
		layout->addWidget(widget);
		layout->addWidget(toggleShowButton);
		layout->addWidget(removeButton);
    }

	connect(mViewService.get(), SIGNAL(activeViewChanged()), this, SLOT(viewGroupChangedSlot()));
    connect(mData.get(), SIGNAL(changed()), this, SLOT(updateDataVisibility()));

    this->setRemoveIcon();
	this->viewGroupChangedSlot();
}

DataSelectWidget::~DataSelectWidget()
{
	disconnect(mViewService.get(), SIGNAL(activeViewChanged()), this, SLOT(viewGroupChangedSlot()));
}

ViewGroupDataPtr DataSelectWidget::getActiveViewGroupData()
{
	int groupIdx = mViewService->getActiveGroupId();
    if (groupIdx<0)
        groupIdx = 0;
	return mViewService->getGroup(groupIdx);
}

void DataSelectWidget::viewGroupChangedSlot()
{
    ViewGroupDataPtr group = this->getActiveViewGroupData();
    if (mCurrentViewGroup==group)
        return;

    if (mCurrentViewGroup)
    {
		disconnect(mCurrentViewGroup.get(), &ViewGroupData::dataViewPropertiesChanged,
				   this, &DataSelectWidget::updateDataVisibility);
    }

    mCurrentViewGroup = group;

    if (mCurrentViewGroup)
    {
		connect(mCurrentViewGroup.get(), &ViewGroupData::dataViewPropertiesChanged,
				this, &DataSelectWidget::updateDataVisibility);
    }

    this->updateDataVisibility();
}

void DataSelectWidget::updateDataVisibility()
{
    mToggleShowAction->setEnabled(mData->getData() && (mCurrentViewGroup!=0));
    mRemoveAction->setEnabled(mData->getData() ? true : false);

    bool visible = false;
    if (mData->getData())
    {
        std::vector<DataPtr> visibleData;
        if (mCurrentViewGroup)
        {
			visibleData = mCurrentViewGroup->getData();
        }
        visible = std::count(visibleData.begin(), visibleData.end(), mData->getData());
    }
    mToggleShowAction->blockSignals(true);
    mToggleShowAction->setChecked(visible);
    mToggleShowAction->blockSignals(false);
    this->cancelRemovalSlot();
    this->setShowIcon();
}

/** If button is checked (first press), do nothing, except
  * change button icon to show remove in progress.
  * If button is unchecked (second press), remove data.
  */
void DataSelectWidget::requestEraseData()
{
    this->setRemoveIcon();

    if (mRemoveAction->isChecked())
    {
        return;
    }
    if (!mData->getData())
        return;

	mPatientModelService->removeData(mData->getData()->getUid());
}

void DataSelectWidget::setRemoveIcon()
{
    if (mRemoveAction->isChecked())
    {
        mRemoveAction->setIcon(QIcon(":/icons/preset_remove.png"));
    }
    else
    {
        mRemoveAction->setIcon(QIcon(":/icons/open_icon_library/edit-delete-2.png"));
    }
}

void DataSelectWidget::setShowIcon()
{
    if (mToggleShowAction->isChecked())
    {
        mToggleShowAction->setIcon(QIcon(":/icons/open_icon_library/eye.png.png"));
    }
    else
    {
        mToggleShowAction->setIcon(QIcon(":/icons/eye.png"));
    }
}

/** Uncheck the remove button without triggering a remove.
  *
  */
void DataSelectWidget::cancelRemovalSlot()
{
    mRemoveAction->blockSignals(true);
    mRemoveAction->setChecked(false);
    this->setRemoveIcon();
    mRemoveAction->blockSignals(false);
}

void DataSelectWidget::toggleShowData()
{
    if (!mData->getData())
        return;

    if (mToggleShowAction->isChecked())
    {
		mCurrentViewGroup->addData(mData->getData()->getUid());
    }
    else
    {
		mCurrentViewGroup->removeData(mData->getData()->getUid());
    }
}


} // namespace cx


