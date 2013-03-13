#include "cxPresetWidget.h"

#include <QComboBox>

namespace cx {

PresetWidget::PresetWidget(QWidget* parent, QString objectName, QString windowTitle) :
	BaseWidget(parent, objectName, windowTitle), mLayout(new QVBoxLayout(this))
{
	mPresetsComboBox = new QComboBox(this);
	mPresetsComboBox->setToolTip("Select a preset to use");

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

void PresetWidget::setPresets(ssc::PresetsPtr presets)
{
	mPresets = presets;
	connect(mPresets.get(), SIGNAL(changed()),
			this, SLOT(populatePresetListSlot()));
}

void PresetWidget::resetSlot()
{
	mPresetsComboBox->setCurrentIndex(0);
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
		mButtonLayout->addWidget(button);
	}
	mButtonLayout->addStretch();
}

//void PresetWidget::populatePresetListSlot()
//{
//	// Re-initialize the list
//  mPresetsComboBox->blockSignals(true);
//  mPresetsComboBox->clear();
//
//  mPresetsComboBox->addItem("Transfer function preset...");
//
//  if (ssc::dataManager()->getActiveImage())
//  	mPresetsComboBox->addItems(mPresets->getPresetList(ssc::dataManager()->getActiveImage()->getModality()));
//  else //No active image, show all available presets for debug/overview purposes
//  	mPresetsComboBox->addItems(mPresets->getPresetList("UNKNOWN"));
//  mPresetsComboBox->blockSignals(false);
//}

} /* namespace cx */
