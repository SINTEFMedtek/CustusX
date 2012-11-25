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
#ifndef SSCCOLORSELECTWIDGET_H
#define SSCCOLORSELECTWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QGridLayout>
#include "sscOptimizedUpdateWidget.h"
#include "sscColorDataAdapter.h"
#include "cxColorSelectButton.h"

namespace ssc
{

/** 
 *
 *
 * \ingroup sscWidget
 * \date 11/22/2012, 2012
 * \author christiana
 */
/**\brief Widget for the BoolDataAdapter.
 *
 * \ingroup sscWidget
 */
class ColorSelectWidget: public OptimizedUpdateWidget
{
Q_OBJECT
public:
    ColorSelectWidget(QWidget* parent, ColorDataAdapterPtr data, QGridLayout* gridLayout = 0, int row = 0);
private slots:
    void prePaintEvent();
    void valueChanged(QColor val);

private:
    QLabel* mLabel;
    cx::ColorSelectButton* mColorButton;
    ColorDataAdapterPtr mData;
};

} // namespace ssc

#endif // SSCCOLORSELECTWIDGET_H
