/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXCOLORSELECTWIDGET_H_
#define CXCOLORSELECTWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QGridLayout>
#include "cxOptimizedUpdateWidget.h"
#include "cxColorPropertyBase.h"
#include "cxColorSelectButton.h"

namespace cx
{

/**\brief Widget for the ColorPropertyBase.
 *
 * \ingroup cx_resource_widgets
 * \date 11/22/2012, 2012
 * \author christiana
 */
class cxResourceWidgets_EXPORT ColorSelectWidget: public OptimizedUpdateWidget
{
Q_OBJECT
public:
    ColorSelectWidget(QWidget* parent, ColorPropertyBasePtr data, QGridLayout* gridLayout = 0, int row = 0);

private slots:
    void prePaintEvent();
    void valueChanged(QColor val);

private:
    QLabel* mLabel;
    cx::ColorSelectButton* mColorButton;
    ColorPropertyBasePtr mData;
};

} // namespace cx

#endif // CXCOLORSELECTWIDGET_H_
