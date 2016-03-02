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

QString PresetWidget::getLastUsedPresetNameFromSettingsFile()
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
