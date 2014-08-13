// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef CXLABELEDCOMBOBOXWIDGET_H_
#define CXLABELEDCOMBOBOXWIDGET_H_

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include "cxStringDataAdapter.h"
#include "cxBaseWidget.h"

namespace cx
{

/**\brief Composite widget for string selection.
 *
 * Consists of <namelabel, combobox>.
 * Insert a subclass of StringDataAdStringDataAdapter to connect to data.
 *
 * \ingroup cx_resource_widgets
 */
class LabeledComboBoxWidget: public BaseWidget
{
Q_OBJECT
public:
	LabeledComboBoxWidget(QWidget* parent, StringDataAdapterPtr, QGridLayout* gridLayout = 0, int row = 0);
	virtual ~LabeledComboBoxWidget(){};

	virtual QString defaultWhatsThis() const;

	void showLabel(bool on);

protected:
	QHBoxLayout* mTopLayout;

private slots:
    void prePaintEvent();
	void comboIndexChanged(int val);

private:
	QLabel* mLabel;
	QComboBox* mCombo;
	StringDataAdapterPtr mData;
};

} // namespace cx

#endif /* CXLABELEDCOMBOBOXWIDGET_H_ */
