#include "cxPresetWidget.h"

#include <QComboBox>

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
		std::cout << "Trying to set presets to null...  :(" << std::endl;
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
	//TODO
	std::cout << "TODO: IMPLEMENT PresetWidget::saveSlot()" << std::endl;
}

void PresetWidget::deleteSlot()
{
	//TODO
	std::cout << "TODO: IMPLEMENT PresetWidget::deleteSlot()" << std::endl;
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


} /* namespace cx */
