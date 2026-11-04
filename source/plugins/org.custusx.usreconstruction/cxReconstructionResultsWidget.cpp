/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxReconstructionResultsWidget.h"

#include <algorithm>
#include <QAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include "cxBaseWidget.h"
#include "cxColorSelectButton.h"
#include "cxDataSelectWidget.h"
#include "cxMesh.h"
#include "cxPatientModelService.h"
#include "cxViewGroupData.h"
#include "cxViewService.h"

namespace cx
{

ReconstructionResultsWidget::ReconstructionResultsWidget(PatientModelServicePtr patientModel, ViewServicePtr view, QWidget* parent) :
	QWidget(parent),
	mPatientModel(patientModel),
	mView(view)
{
	mLayout = new QVBoxLayout(this);
	mLayout->setContentsMargins(0, 0, 0, 0);
	mLayout->setSpacing(0);
	mLayout->addWidget(new QLabel("Created:", this));
	connect(mPatientModel.get(), &PatientModelService::dataAddedOrRemoved, this, &ReconstructionResultsWidget::dataAddedOrRemoved);
	connect(mView.get(), &ViewService::activeViewChanged, this, &ReconstructionResultsWidget::activeViewChanged);
	this->activeViewChanged();
	this->setVisible(false);
}

void ReconstructionResultsWidget::setData(std::vector<DataPtr> data)
{
	for (unsigned i = 0; i < mRows.size(); ++i)
	{
		mRows[i].mWidget->deleteLater();
	}
	mRows.clear();
	for (unsigned i = 0; i < data.size(); ++i)
	{
		mRows.push_back(this->createRow(data[i]));
		mLayout->addWidget(mRows.back().mWidget);
	}
	this->setVisible(!mRows.empty());
	this->updateShowButtons();
}

QStringList ReconstructionResultsWidget::getUids() const
{
	QStringList retval;
	for (unsigned i = 0; i < mRows.size(); ++i)
	{
		retval << mRows[i].mUid;
	}
	return retval;
}

ReconstructionResultsWidget::Row ReconstructionResultsWidget::createRow(DataPtr data)
{
	Row retval;
	retval.mUid = data->getUid();
	retval.mWidget = new QWidget(this);
	QHBoxLayout* layout = new QHBoxLayout(retval.mWidget);
	layout->setContentsMargins(0, 0, 0, 0);

	QString uid = data->getUid();
	retval.mShow = new QAction(QIcon(":/icons/eye.png"), "Show", retval.mWidget);
	retval.mShow->setToolTip("Show in the active view group");
	retval.mShow->setCheckable(true);
	connect(retval.mShow, &QAction::toggled, this, [this, uid](bool show) { this->toggleShow(uid, show); });
	CXSmallToolButton* showButton = new CXSmallToolButton(retval.mWidget);
	showButton->setDefaultAction(retval.mShow);
	layout->addWidget(showButton);
	QLabel* icon = new QLabel(retval.mWidget);
	icon->setPixmap(data->getIcon().pixmap(16, 16));
	icon->setToolTip(data->getType());
	layout->addWidget(icon);
	layout->addWidget(new QLabel(data->getName(), retval.mWidget), 1);

	QColor color = this->getColor(data);
	if (color.isValid())
	{
		ColorSelectButton* colorButton = new ColorSelectButton("", color, retval.mWidget);
		colorButton->setToolTip("The color of the data");
		connect(colorButton, &ColorSelectButton::colorChanged, this, [this, uid](QColor value) { this->setColor(mPatientModel->getData(uid), value); });
		layout->addWidget(colorButton);
	}
	EraseDataToolButton* eraseButton = new EraseDataToolButton(retval.mWidget);
	connect(eraseButton, &EraseDataToolButton::eraseData, this, [this, uid]() { mPatientModel->removeData(uid); });
	layout->addWidget(eraseButton);
	return retval;
}

void ReconstructionResultsWidget::setColorFunctions(ColorGetter getter, ColorSetter setter)
{
	mColorGetter = getter;
	mColorSetter = setter;
}

/** The color from the color functions, or the color of a mesh. */
QColor ReconstructionResultsWidget::getColor(DataPtr data) const
{
	QColor retval = mColorGetter ? mColorGetter(data) : QColor();
	MeshPtr mesh = std::dynamic_pointer_cast<Mesh>(data);
	if (!retval.isValid() && mesh)
	{
		retval = mesh->getColor();
	}
	return retval;
}

void ReconstructionResultsWidget::setColor(DataPtr data, QColor color)
{
	MeshPtr mesh = std::dynamic_pointer_cast<Mesh>(data);
	if (mColorGetter && mColorSetter && mColorGetter(data).isValid())
	{
		mColorSetter(data, color);
	}
	else if (mesh)
	{
		mesh->setColor(color);
	}
}

void ReconstructionResultsWidget::toggleShow(QString uid, bool show)
{
	if (mViewGroup && show)
	{
		mViewGroup->addData(uid);
	}
	else if (mViewGroup)
	{
		mViewGroup->removeData(uid);
	}
}

ViewGroupDataPtr ReconstructionResultsWidget::getViewGroup() const
{
	return mView->getGroup(std::max(0, mView->getActiveGroupId()));
}

void ReconstructionResultsWidget::activeViewChanged()
{
	if (mViewGroup)
	{
		disconnect(mViewGroup.get(), &ViewGroupData::dataViewPropertiesChanged, this, &ReconstructionResultsWidget::updateShowButtons);
	}
	mViewGroup = this->getViewGroup();
	if (mViewGroup)
	{
		connect(mViewGroup.get(), &ViewGroupData::dataViewPropertiesChanged, this, &ReconstructionResultsWidget::updateShowButtons);
	}
	this->updateShowButtons();
}

void ReconstructionResultsWidget::updateShowButtons()
{
	std::vector<DataPtr> shown = mViewGroup ? mViewGroup->getData() : std::vector<DataPtr>();
	for (unsigned i = 0; i < mRows.size(); ++i)
	{
		DataPtr data = mPatientModel->getData(mRows[i].mUid);
		bool visible = data && std::count(shown.begin(), shown.end(), data);
		mRows[i].mShow->blockSignals(true);
		mRows[i].mShow->setChecked(visible);
		mRows[i].mShow->setIcon(QIcon(visible ? ":/icons/open_icon_library/eye.png.png" : ":/icons/eye.png"));
		mRows[i].mShow->setEnabled(mViewGroup ? true : false);
		mRows[i].mShow->blockSignals(false);
	}
}

/** Removed data are taken out of the list. */
void ReconstructionResultsWidget::dataAddedOrRemoved()
{
	std::vector<DataPtr> remaining;
	for (unsigned i = 0; i < mRows.size(); ++i)
	{
		DataPtr data = mPatientModel->getData(mRows[i].mUid);
		if (data)
		{
			remaining.push_back(data);
		}
	}
	if (remaining.size() != mRows.size())
	{
		this->setData(remaining);
	}
}

} // namespace cx
