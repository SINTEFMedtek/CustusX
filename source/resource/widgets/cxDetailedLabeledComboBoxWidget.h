/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDETAILEDLABELEDCOMBOBOXWIDGET_H_
#define CXDETAILEDLABELEDCOMBOBOXWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxLabeledComboBoxWidget.h"

namespace cx
{
/**\brief Composite widget for string selection with .
 *
 * Consists of <namelabel, combobox, detailsbutton>.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT DetailedLabeledComboBoxWidget : public LabeledComboBoxWidget
{
	Q_OBJECT

public:
	DetailedLabeledComboBoxWidget(QWidget* parent, StringPropertyBasePtr, QGridLayout* gridLayout = 0, int row = 0);
	virtual ~DetailedLabeledComboBoxWidget();

signals:
	void detailsTriggered();

private slots:
	void toggleDetailsSlot();
};

} /* namespace cxtest */

#endif /* CXDETAILEDLABELEDCOMBOBOXWIDGET_H_ */
