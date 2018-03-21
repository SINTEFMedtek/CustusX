/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPresetWidget.h"

#include <QComboBox>
#include <QInputDialog>
#include "cxLogger.h"
#include "cxSettings.h"

namespace cx {

PresetWidget::PresetWidget(QWidget* parent) :
	BaseWidget(parent, "PresetWidget", "Presets"), mLayout(new QVBoxLayout(this))
{
	this->setToolTip("Select a predefined set of options");
	mPresetsComboBox = new QComboBox(this);
	mPresetsComboBox->setToolTip("Select a preset to use");
	connect(mPresetsComboBox, SIGNAL(currentIndexChanged(const QString&)), this,
			SLOT(presetsBoxChangedSlot(const QString&)));

	mActionGroup = new QActionGroup(this);

	this->createAction(mActionGroup,
	                QIcon(":/icons/preset_reset.png"),
	                "Reset all transfer function values to the defaults", "",
	                SLOT(resetSlot()));

	this->createAction(mActionGroup,
	                QIcon(":/icons/preset_remove.png"),
					"Delete the current preset", "",
	                SLOT(deleteSlot()));

	this->createAction(mActionGroup,
		            QIcon(":/icons/preset_save.png"),
					"Add the current setting as a preset", "",
	                SLOT(saveSlot()));

	mLayout->addWidget(mPresetsComboBox);

	mButtonLayout = NULL;
	this->populateButtonLayout();

	this->setLayout(mLayout);
}

bool PresetWidget::requestSetCurrentPreset(QString name)
{
	if(mPresetsComboBox->findText(name) == -1)
		return false;

	mPresetsComboBox->setCurrentIndex(mPresetsComboBox->findText(name));
	return true;
}

QString PresetWidget::getCurrentPreset()
{
	return mPresetsComboBox->currentText();
}

void PresetWidget::showDetailed(bool detailed)
{
	if(!mButtonLayout)
		return;

	for(int i=0; i < mButtonLayout->count(); ++i)
	{
		QWidget* widget = mButtonLayout->itemAt(i)->widget();
		if(!widget)
			continue;
		if(detailed)
			widget->show();
		else
			widget->hide();
	}
}

void PresetWidget::setPresets(PresetsPtr presets)
{
	if(!presets)
	{
		reportError("Trying to set presets to null...");
		return;
	}
	//TODO disconnect old stuff

	mPresets = presets;
	connect(mPresets.get(), SIGNAL(changed()), this, SLOT(populatePresetListSlot()));

	this->populatePresetListSlot();
    this->selectLastUsedPreset();
}

QString PresetWidget::getLastUsedPresetNameFromSettingsFile() const
{
    QString id = mPresets->getId();
    settings()->fillDefault(id, mPresets->getPresetList().first());

    QString lastUsedPresetName = settings()->value(id).toString();
    return lastUsedPresetName;
}

void PresetWidget::resetSlot()
{
	mPresetsComboBox->setCurrentIndex(0);
}

void PresetWidget::saveSlot()
{
	mPresets->save();
	this->populatePresetListSlot();
}

void PresetWidget::deleteSlot()
{
	mPresets->remove();
	this->populatePresetListSlot();
}

void PresetWidget::populatePresetListSlot()
{
	this->populatePresetList(mPresets->getPresetList(""));
}

void PresetWidget::presetsBoxChangedSlot(const QString& name)
{
    settings()->setValue(mPresets->getId(), name);
	emit presetSelected(name);
}

void PresetWidget::populateButtonLayout()
{
	//delete old stuff
	if(mButtonLayout)
	{
		QLayoutItem *child;
		while ((child = mButtonLayout->takeAt(0)) != 0)
		{
			// delete both the layoutitem AND the widget. Not auto done because layoutitem is no QObject.
			QWidget* widget = child->widget();
			delete child;
			delete widget;
		}
		delete mButtonLayout;
	}

	//make the new buttons
	mButtonLayout = new QHBoxLayout;
	mLayout->addLayout(mButtonLayout);

	QList<QAction*> actions = mActionGroup->actions();
	for (int i=0; i<actions.size(); ++i)
	{
		QToolButton* button = new QToolButton(this);
		button->setDefaultAction(actions[i]);
		button->show();
		mButtonLayout->addWidget(button);
	}
	mButtonLayout->addStretch();
}

void PresetWidget::populatePresetList(QStringList list)
{
	mPresetsComboBox->blockSignals(true);
	mPresetsComboBox->clear();

	mPresetsComboBox->addItem("<Default preset>");

	mPresetsComboBox->addItems(list);

	mPresetsComboBox->blockSignals(false);
}

QString PresetWidget::getNewPresetName(bool withoutSpaces = false)
{
	QString retval;

	// generate a name suggestion: identical if custom, appended by index if default.
	QString newName = PresetWidget::getCurrentPreset();
	if (!mPresets->getPresetList("").contains(newName))
		newName = "custom preset";
	if (mPresets->isDefaultPreset(newName))
		newName += "(2)";

	bool ok;
	QString text = QInputDialog::getText(this, "Save Preset",
			"Custom Preset Name", QLineEdit::Normal, newName, &ok);
	if (!ok || text.isEmpty())
		text = newName;

	retval = text;
	if(withoutSpaces)
		retval = retval.replace(" ", "-");

    return retval;
}

void PresetWidget::selectLastUsedPreset()
{
    QString lastUsedPreset = this->getLastUsedPresetNameFromSettingsFile();
    this->requestSetCurrentPreset(lastUsedPreset);
}


} /* namespace cx */
