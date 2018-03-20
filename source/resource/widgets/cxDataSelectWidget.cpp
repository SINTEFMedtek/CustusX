/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDataSelectWidget.h"

#include "cxHelperWidgets.h"
#include "cxSelectDataStringProperty.h"
#include "cxTypeConversions.h"
#include "cxViewService.h"
#include "cxPatientModelService.h"
#include "cxData.h"
#include "cxViewGroupData.h"
#include "cxLogger.h"

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

	mRemoveButton = new EraseDataToolButton(this);
	connect(mRemoveButton, &EraseDataToolButton::eraseData, this, &DataSelectWidget::eraseData);

    if(gridLayout)
    {
    	gridLayout->setMargin(0);
    	gridLayout->setSpacing(0);
    	QHBoxLayout* lay = new QHBoxLayout;
    	lay->addWidget(toggleShowButton);
		lay->addWidget(mRemoveButton);
    	gridLayout->addLayout(lay, row, 2);
    }else
    {
		layout->addWidget(widget);
		layout->addWidget(toggleShowButton);
		layout->addWidget(mRemoveButton);
    }

	connect(mViewService.get(), SIGNAL(activeViewChanged()), this, SLOT(viewGroupChangedSlot()));
    connect(mData.get(), SIGNAL(changed()), this, SLOT(updateDataVisibility()));

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
	if (mCurrentViewGroupData==group)
        return;

	if (mCurrentViewGroupData)
    {
		disconnect(mCurrentViewGroupData.get(), &ViewGroupData::dataViewPropertiesChanged,
				   this, &DataSelectWidget::updateDataVisibility);
    }

	mCurrentViewGroupData = group;

	if (mCurrentViewGroupData)
    {
		connect(mCurrentViewGroupData.get(), &ViewGroupData::dataViewPropertiesChanged,
				this, &DataSelectWidget::updateDataVisibility);
    }

    this->updateDataVisibility();
}

void DataSelectWidget::updateDataVisibility()
{
	mToggleShowAction->setEnabled(mData->getData() && (mCurrentViewGroupData!=0));
	mRemoveButton->setEnabled(mData->getData() ? true : false);

    bool visible = false;
    if (mData->getData())
    {
        std::vector<DataPtr> visibleData;
		if (mCurrentViewGroupData)
        {
			visibleData = mCurrentViewGroupData->getData();
        }
        visible = std::count(visibleData.begin(), visibleData.end(), mData->getData());
    }
    mToggleShowAction->blockSignals(true);
    mToggleShowAction->setChecked(visible);
    mToggleShowAction->blockSignals(false);
	mRemoveButton->reset();
    this->setShowIcon();
}

void DataSelectWidget::eraseData()
{
	if (!mData->getData())
		return;

	mPatientModelService->removeData(mData->getData()->getUid());
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

void DataSelectWidget::toggleShowData()
{
    if (!mData->getData())
        return;

    if (mToggleShowAction->isChecked())
    {
		mCurrentViewGroupData->addData(mData->getData()->getUid());
    }
    else
    {
		mCurrentViewGroupData->removeData(mData->getData()->getUid());
    }
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

EraseDataToolButton::EraseDataToolButton(QWidget *parent) :
	CXSmallToolButton(parent)
{
	QString tip("<html><h4>Permanently delete data.</h4><p>Press button twice to delete.<br>"
				 "Right-click after the first click to cancel.<p></html>");

	mRemoveAction = new QAction(this);
	mRemoveAction->setToolTip(tip);
	connect(mRemoveAction, &QAction::triggered, this, &EraseDataToolButton::requestEraseData);
	mRemoveAction->setCheckable(true);
	this->setDefaultAction(mRemoveAction);

	this->setRemoveIcon();
}

void EraseDataToolButton::reset()
{
	this->cancelRemovalSlot();
}

void EraseDataToolButton::mousePressEvent(QMouseEvent *e)
{
	CXSmallToolButton::mousePressEvent(e);
	if (e->button() == Qt::RightButton)
		this->cancelRemovalSlot();
}

/** Uncheck the remove button without triggering a remove.
  *
  */
void EraseDataToolButton::cancelRemovalSlot()
{
	mRemoveAction->blockSignals(true);
	mRemoveAction->setChecked(false);
	this->setRemoveIcon();
	mRemoveAction->blockSignals(false);
}

void EraseDataToolButton::setRemoveIcon()
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

/** If button is checked (first press), do nothing, except
  * change button icon to show remove in progress.
  * If button is unchecked (second press), remove data.
  */
void EraseDataToolButton::requestEraseData()
{
	this->setRemoveIcon();

	if (mRemoveAction->isChecked())
		return;

	emit eraseData();
}


} // namespace cx


