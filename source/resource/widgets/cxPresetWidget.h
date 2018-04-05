/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
