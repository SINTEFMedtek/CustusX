/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXLABELEDLINEEDITWIDGET_H_
#define CXLABELEDLINEEDITWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include "cxStringPropertyBase.h"
#include "cxOptimizedUpdateWidget.h"

namespace cx
{

/**\brief Composite widget for string edit.
 *
 *  Accepts a StringPropertyBasePtr that has getAllowOnlyValuesInRange()==false
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT LabeledLineEditWidget: public OptimizedUpdateWidget
{
Q_OBJECT
public:
	LabeledLineEditWidget(QWidget* parent, StringPropertyBasePtr, QGridLayout* gridLayout = 0, int row = 0);
private slots:
    virtual void prePaintEvent();
	void editingFinished();
private:
	QLabel* mLabel;
	QLineEdit* mLine;
	StringPropertyBasePtr mData;
};

} // namespace cx

#endif /* CXLABELEDLINEEDITWIDGET_H_ */
