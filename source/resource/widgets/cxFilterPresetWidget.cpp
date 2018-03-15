/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFilterPresetWidget.h"

#include <QMessageBox>
#include "cxLogger.h"
#include "cxFilter.h"

namespace cx {

FilterPresetWidget::FilterPresetWidget(QWidget* parent) :
		PresetWidget(parent)
{}

void FilterPresetWidget::setFilter(FilterPtr filter)
{
	if(!filter->hasPresets()){
		reportError("Cannot use filter "+filter->getName()+" in the FilterPresetWidget because it does not have any presets.");
		return;
	}

	mFilter = filter;
	this->setPresets(mFilter->getPresets());
}

void FilterPresetWidget::saveSlot()
{
	if(!mFilter)
		return;

	QString text = PresetWidget::getNewPresetName(true);
	QDomElement element = mFilter->generatePresetFromCurrentlySetOptions(text);
	mPresets->addCustomPreset(element);

	PresetWidget::saveSlot();
	PresetWidget::requestSetCurrentPreset(text);
}

void FilterPresetWidget::deleteSlot()
{
	if (mPresets->isDefaultPreset(PresetWidget::getCurrentPreset())) {
		reportWarning("It is not possible to delete one of the default presets");
		return;
	}

	if (QMessageBox::question(this, "Delete current preset", "Do you really want to delete the current preset?", QMessageBox::Cancel | QMessageBox::Ok) != QMessageBox::Ok)
		return;
	mPresets->deleteCustomPreset(PresetWidget::getCurrentPreset());

	PresetWidget::deleteSlot();
	//TODO
//	PresetWidget::requestSetCurrentPreset(text);
}

} /* namespace cx */
