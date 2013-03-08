#ifndef CXPRESETWIDGET_H_
#define CXPRESETWIDGET_H_

#include "cxBaseWidget.h"

namespace cx {

/*
 * cxPresetWidget.h
 *
 * \date Mar 8, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class PresetWidget : public BaseWidget
{
	  Q_OBJECT

public:
	PresetWidget(QWidget* parent, QString objectName, QString windowTitle);
	virtual ~PresetWidget();
	virtual QString defaultWhatsThis() const;
};

} /* namespace cx */
#endif /* CXPRESETWIDGET_H_ */
