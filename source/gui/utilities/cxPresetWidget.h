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

#ifndef CXPRESETWIDGET_H_
#define CXPRESETWIDGET_H_

#include "cxBaseWidget.h"
#include "sscPresets.h"

class QComboBox;

namespace cx {

/**
 * \class PresetWidget
 * \brief Base class for preset handling. Takes care of making
 * a uniform preset system. Contains a preset selector, functionality for
 * resetting to a default preset, saving new presets and deleting presets.
 *
 * \ingroup cxGUI
 *
 * \date Mar 8, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class PresetWidget : public BaseWidget
{
	  Q_OBJECT

public:
	PresetWidget(QWidget* parent);
	virtual ~PresetWidget(){};
	virtual QString defaultWhatsThis() const;

	bool requestSetCurrentPreset(QString name); ///< tries to set the preset to the requested name
	QString getCurrentPreset(); ///< returns the name of the currently selected preset
	void showDetailed(bool detailed); ///< sets the presetwidget in detailed mode or not

	virtual void setPresets(ssc::PresetsPtr presets);

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
	ssc::PresetsPtr mPresets;

private:
	QComboBox* mPresetsComboBox; ///< combobox for selecting presets
	QHBoxLayout* mButtonLayout; ///< the layout where all the buttons are
	QVBoxLayout* mLayout; ///< main layout
};

} /* namespace cx */
#endif /* CXPRESETWIDGET_H_ */
