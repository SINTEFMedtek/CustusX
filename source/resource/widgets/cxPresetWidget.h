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

#ifndef CXPRESETWIDGET_H_
#define CXPRESETWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxBaseWidget.h"
#include "cxPresets.h"

class QComboBox;

namespace cx {

/**
 * \class PresetWidget
 * \brief Base class for preset handling. Takes care of making
 * a uniform preset system. Contains a preset selector, functionality for
 * resetting to a default preset, saving new presets and deleting presets.
 *
 * \ingroup cx_resource_widgets
 * \date Mar 8, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResourceWidgets_EXPORT PresetWidget : public BaseWidget
{
	  Q_OBJECT

public:
	PresetWidget(QWidget* parent);
	virtual ~PresetWidget(){};

	bool requestSetCurrentPreset(QString name); ///< tries to set the preset to the requested name
	QString getCurrentPreset(); ///< returns the name of the currently selected preset
	void showDetailed(bool detailed); ///< sets the presetwidget in detailed mode or not

	virtual void setPresets(PresetsPtr presets);

    QString getLastUsedPresetNameFromSettingsFile() const;

signals:
	void presetSelected(QString name);

public slots:
	virtual void resetSlot();
	virtual void saveSlot();
	virtual void deleteSlot();
	virtual void populatePresetListSlot(); ///< Fill the preset list with the available presets

protected slots:
	virtual void presetsBoxChangedSlot(const QString&);

protected:
	virtual void populateButtonLayout(); ///< makes buttons based on the actions found in the actiongroup
	void populatePresetList(QStringList list); ///< populates the preset combobox
	QString getNewPresetName(bool withoutSpaces);

	QActionGroup* mActionGroup; ///< contains all actions that will have buttons
	PresetsPtr mPresets;

private:
    void selectLastUsedPreset(); ///< reads settings file to select last used preset
	QComboBox* mPresetsComboBox; ///< combobox for selecting presets
	QHBoxLayout* mButtonLayout; ///< the layout where all the buttons are
	QVBoxLayout* mLayout; ///< main layout
};

} /* namespace cx */
#endif /* CXPRESETWIDGET_H_ */
