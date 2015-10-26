/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
