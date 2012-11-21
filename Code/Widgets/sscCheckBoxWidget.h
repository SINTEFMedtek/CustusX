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


/*
 * sscCheckBoxWidget.h
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#ifndef SSCCHECKBOXWIDGET_H_
#define SSCCHECKBOXWIDGET_H_

#include "sscBoolDataAdapter.h"

#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QGridLayout>
#include "sscOptimizedUpdateWidget.h"

namespace ssc
{

/**\brief Widget for the BoolDataAdapter.
 *
 * \ingroup sscWidget
 */
class CheckBoxWidget: public OptimizedUpdateWidget
{
Q_OBJECT
public:
	CheckBoxWidget(QWidget* parent, BoolDataAdapterPtr data, QGridLayout* gridLayout = 0, int row = 0);
private slots:
    void prePaintEvent();
	void valueChanged(bool val);

private:
    QLabel* mLabel;
    QCheckBox* mCheckBox;
	BoolDataAdapterPtr mData;
};

}

#endif /* SSCCHECKBOXWIDGET_H_ */
