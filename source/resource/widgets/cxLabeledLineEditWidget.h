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

#ifndef CXLABELEDLINEEDITWIDGET_H_
#define CXLABELEDLINEEDITWIDGET_H_

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include "cxStringDataAdapter.h"
#include "cxOptimizedUpdateWidget.h"

namespace cx
{

/**\brief Composite widget for string edit.
 *
 *  Accepts a StringDataAdapterPtr that has getAllowOnlyValuesInRange()==false
 *
 * \ingroup cx_resource_widgets
 */
class LabeledLineEditWidget: public OptimizedUpdateWidget
{
Q_OBJECT
public:
	LabeledLineEditWidget(QWidget* parent, StringDataAdapterPtr, QGridLayout* gridLayout = 0, int row = 0);
private slots:
    virtual void prePaintEvent();
	void editingFinished();
private:
	QLabel* mLabel;
	QLineEdit* mLine;
	StringDataAdapterPtr mData;
};

} // namespace cx

#endif /* CXLABELEDLINEEDITWIDGET_H_ */