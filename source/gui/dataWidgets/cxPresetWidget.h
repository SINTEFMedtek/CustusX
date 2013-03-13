#ifndef CXPRESETWIDGET_H_
#define CXPRESETWIDGET_H_

#include "cxBaseWidget.h"
#include "sscPresets.h"

class QComboBox;

namespace cx {

/*
 * \class PresetWidget
 *
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
	PresetWidget(QWidget* parent, QString objectName, QString windowTitle);
	virtual ~PresetWidget(){};
	virtual QString defaultWhatsThis() const;

public slots:
	virtual void resetSlot();
	virtual void saveSlot() = 0;
	virtual void deleteSlot() = 0;
	virtual void populatePresetListSlot() = 0;  ///< Fill the preset list with the available presets

protected:
	virtual void setPresets(ssc::PresetsPtr presets);
	virtual void populateButtonLayout(); ///< makes buttons based on the actions found in the actiongroup

	QActionGroup* mActionGroup; ///< contains all actions that will have buttons
	QComboBox* mPresetsComboBox; ///< combobox for selecting presets
	ssc::PresetsPtr mPresets;

private:
	QHBoxLayout* mButtonLayout; ///< the layout where all the buttons are
	QVBoxLayout* mLayout; ///< main layout
};

} /* namespace cx */
#endif /* CXPRESETWIDGET_H_ */
