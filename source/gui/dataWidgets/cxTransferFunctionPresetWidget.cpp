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

#include "cxTransferFunctionPresetWidget.h"

#include <QInputDialog>
#include <QMessageBox>

#include "cxSettings.h"
#include "cxActiveImageProxy.h"
#include "cxPatientModelService.h"
#include "cxLogger.h"
#include "cxActiveData.h"

namespace cx {

TransferFunctionPresetWidget::TransferFunctionPresetWidget(PatientModelServicePtr patientModelService, QWidget* parent, bool is3D) :
	PresetWidget(parent), mIs3D(is3D),
	mActiveData(patientModelService->getActiveData())
{
	this->setToolTip("Select a predefined transfer function");
	this->setObjectName("TransferFunctionPresetWidget");
	this->setPresets(patientModelService->getPresetTransferFunctions3D());
	QString toggleText = "Toggle between apply presets,\neither on %1\nor both 2D and 3D\ntransfer functions.";
	if (is3D)
		toggleText = toggleText.arg("3D");
	else
		toggleText = toggleText.arg("2D");

	mToggleAction = this->createAction(mActionGroup,
			QIcon(":/icons/preset_2D_and_3D.png"), toggleText, "",
			SLOT(toggleSlot()));
	this->populateButtonLayout();

	mApplyToAll = settings()->value("applyTransferFunctionPresetsToAll").toBool();
	this->updateToggles();

	mActiveImageProxy = ActiveImageProxy::New(mActiveData);
	connect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this,
			&TransferFunctionPresetWidget::populatePresetListSlot);
	connect(mActiveImageProxy.get(), &ActiveImageProxy::propertiesChanged, this,
			&TransferFunctionPresetWidget::populatePresetListSlot);
}

void TransferFunctionPresetWidget::toggleSlot() {
	mApplyToAll = !mApplyToAll;
	settings()->setValue("applyTransferFunctionPresetsToAll", mApplyToAll);

	this->updateToggles();
	this->populatePresetListSlot();
}

void TransferFunctionPresetWidget::updateToggles()
{
	if (mApplyToAll)
		mToggleAction->setIcon(QIcon(":/icons/preset_2D_and_3D.png"));
	else
	{
		if (mIs3D)
			mToggleAction->setIcon(QIcon(":/icons/preset_3D.png"));
		else
			mToggleAction->setIcon(QIcon(":/icons/preset_2D.png"));
	}
}

void TransferFunctionPresetWidget::populatePresetListSlot()
{
	if (mActiveData->getActive<Image>())
		PresetWidget::populatePresetList(mPresets->getPresetList(mActiveData->getActive<Image>()->getModality()));
	else
		//No active image, show all available presets for debug/overview purposes
		PresetWidget::populatePresetList(mPresets->getPresetList("UNKNOWN"));
}

void TransferFunctionPresetWidget::presetsBoxChangedSlot(const QString& presetName)
{
	ImagePtr activeImage = mActiveData->getActive<Image>();
	if (activeImage) {
		TransferFunctions3DPresetsPtr preset = boost::dynamic_pointer_cast<TransferFunctions3DPresets>(mPresets);
		preset->load(presetName, activeImage, this->use2D(), this->use3D());
	}
}

void TransferFunctionPresetWidget::resetSlot()
{
	PresetWidget::resetSlot();
	ImagePtr activeImage = mActiveData->getActive<Image>();
	activeImage->resetTransferFunctions(this->use2D(), this->use3D());
}

void TransferFunctionPresetWidget::saveSlot()
{
	// generate a name suggestion: identical if custom, appended by index if default.
	QString newName = PresetWidget::getCurrentPreset();
	TransferFunctions3DPresetsPtr preset = boost::dynamic_pointer_cast<TransferFunctions3DPresets>(mPresets);
	if (!preset->getPresetList("").contains(newName))
		newName = "custom preset";
	if (preset->isDefaultPreset(newName))
		newName += "(2)";

	bool ok;
	QString text = QInputDialog::getText(this, "Save Preset",
			"Custom Preset Name", QLineEdit::Normal, newName, &ok);
	if (!ok || text.isEmpty())
		return;

	ImagePtr activeImage = mActiveData->getActive<Image>();
	preset->save(text, activeImage, this->use2D(), this->use3D());

	this->populatePresetListSlot();
	PresetWidget::requestSetCurrentPreset(text);
}

bool TransferFunctionPresetWidget::use2D() const
{

	return !mIs3D || mApplyToAll;
}

bool TransferFunctionPresetWidget::use3D() const
{
	return mIs3D || mApplyToAll;
}

void TransferFunctionPresetWidget::deleteSlot()
{
	if (mPresets->isDefaultPreset(PresetWidget::getCurrentPreset())) {
		reportWarning("It is not possible to delete one of the default presets");
		return;
	}

	TransferFunctions3DPresetsPtr preset = boost::dynamic_pointer_cast<TransferFunctions3DPresets>(mPresets);
	if (QMessageBox::question(this, "Delete current preset", "Do you really want to delete the current preset?", QMessageBox::Cancel | QMessageBox::Ok) != QMessageBox::Ok)
		return;
	preset->deletePresetData(PresetWidget::getCurrentPreset(), this->use2D(), this->use3D());

	this->populatePresetListSlot();
	this->resetSlot();
}

} //namespace cx
