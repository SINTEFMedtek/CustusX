/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXLABELEDCOMBOBOXWIDGET_H_
#define CXLABELEDCOMBOBOXWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include "cxStringPropertyBase.h"
#include "cxBaseWidget.h"

namespace cx
{

/**\brief Composite widget for string selection.
 *
 * Consists of <namelabel, combobox>.
 * Insert a subclass of StringPropertyBasePtr to connect to data.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT LabeledComboBoxWidget: public BaseWidget
{
Q_OBJECT
public:
	LabeledComboBoxWidget(QWidget* parent, StringPropertyBasePtr, QGridLayout* gridLayout = 0, int row = 0);
	virtual ~LabeledComboBoxWidget(){};

	void showLabel(bool on);

protected:
	QHBoxLayout* mTopLayout;

private slots:
    void prePaintEvent();
	void comboIndexChanged(int val);

private:
	QIcon getIcon(QString uid);

	QLabel* mLabel;
	QComboBox* mCombo;
	StringPropertyBasePtr mData;
};

} // namespace cx

#endif /* CXLABELEDCOMBOBOXWIDGET_H_ */
