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

#include "cxPresetWidget.h"

#include <QComboBox>
#include <QInputDialog>
#include "sscMessageManager.h"

namespace cx {

PresetWidget::PresetWidget(QWidget* parent) :
	BaseWidget(parent, "PresetWidget", "Presets"), mLayout(new QVBoxLayout(this))
{
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

QString PresetWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Presets</h3>"
    "<p>Lets you select a predefined set of options.</p>"
    "<p><i></i></p>"
    "</html>";
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

void PresetWidget::setPresets(ssc::PresetsPtr presets)
{
	if(!presets)
	{
		ssc::messageManager()->sendError("Trying to set presets to null...");
		return;
	}
	//TODO disconnect old stuff

	mPresets = presets;
	connect(mPresets.get(), SIGNAL(changed()), this, SLOT(populatePresetListSlot()));

	this->populatePresetListSlot();
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


} /* namespace cx */
