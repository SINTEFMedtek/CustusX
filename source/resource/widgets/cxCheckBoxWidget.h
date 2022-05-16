/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/



/*
 * sscCheckBoxWidget.h
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#ifndef CXCHECKBOXWIDGET_H_
#define CXCHECKBOXWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxBoolPropertyBase.h"

#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QGridLayout>
#include "cxOptimizedUpdateWidget.h"

namespace cx
{

/**\brief Widget for the BoolPropertyBase.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT CheckBoxWidget: public OptimizedUpdateWidget
{
Q_OBJECT
public:
	CheckBoxWidget(QWidget* parent, BoolPropertyBasePtr data, QGridLayout* gridLayout = 0, int row = 0);

private slots:
	void prePaintEvent();
	void valueChanged(bool val);

private:
	QLabel* mLabel;
	QCheckBox* mCheckBox;
	BoolPropertyBasePtr mData;
};

}

#endif /* CXCHECKBOXWIDGET_H_ */
